//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridProcessor.h"

#include "Core/DungeonProp.h"
#include "Core/Utils/MathUtils.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/GridSceneMarkerList.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridLayer.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"
#include "Frameworks/MarkerGenerator/MarkerGenLayer.h"
#include "Frameworks/MarkerGenerator/PatternScript/Impl/PatternScriptNodesImpl.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"

#include "Containers/Queue.h"

namespace MarkerGenGridProcessorLib {
	void TransformMarkers(const FTransform& InTransform, TArray<FDAMarkerInfo>& OutMarkers) {
		for (FDAMarkerInfo& Marker : OutMarkers) {
			Marker.Transform = Marker.Transform * InTransform;
		}
	}

	/**
	 * The pattern should contain at least one rule that performs an action (like emit marker, remove marker etc)
	 * We don't run the pattern over the dungeon marker list otherwise, as it won't affect anything
	 */
	bool ShouldProcessAssembly(UMarkerGenPattern* InPattern) {
		if (!InPattern || InPattern->Rules.Num() == 0) {
			return false;
		}

		for (UMarkerGenPatternRule* Rule : InPattern->Rules) {
			if (!Rule || !Rule->RuleScript) continue;
			
			for (UMGPatternScriptNode* ScriptNode : Rule->RuleScript->Nodes) {
				if (ScriptNode && ScriptNode->IsA<UMGPatternActionScriptNodeBase>() && !ScriptNode->IsA<UMGPatternScriptNode_OnPass>()) {
					// Contains an action node. We need to process this layer
					return true;
				}
			}
		}

		return false;
	}

	bool IsValid(TWeakObjectPtr<UMGPatternScript> InScript) {
		return InScript.IsValid() && InScript->ResultNode.IsValid() && InScript->PassEventNode.IsValid();
	}
}

struct FMGCellHeightCoords {
	int32 GroundCoordZ = 0;
	int32 CornerCoordZ = 0;
	int32 EdgeXCoordZ = 0;
	int32 EdgeYCoordZ = 0;
	
	int32 GetHeight(EMarkerGenGridPatternRuleType InCoordType) const {
		if (InCoordType == EMarkerGenGridPatternRuleType::Ground) {
			return GroundCoordZ;
		}
		else if (InCoordType == EMarkerGenGridPatternRuleType::Corner) {
			return CornerCoordZ;
		}
		else if (InCoordType == EMarkerGenGridPatternRuleType::EdgeX) {
			return EdgeXCoordZ;
		}
		else if (InCoordType == EMarkerGenGridPatternRuleType::EdgeY) {
			return EdgeYCoordZ;
		}
		else {
			check(0); // Not implemented
			return 0;
		}
	}
};

struct FMGPatternGridRuleExecutorState {
	FIntPoint Coord = FIntPoint::ZeroValue;
	EMarkerGenGridPatternRuleType CoordType = EMarkerGenGridPatternRuleType::Ground;
	float BaseAngleRad = 0;
	int32 Rotation90Index = 0;
	FString CopyRotationFromMarker;
};

class FMGPatternGridRuleExecutor : public IMGPatternRuleExecutor {
public:
	FMGPatternGridRuleExecutor(FMGGridSceneMarkerList* InMarkerList)
		: MarkerList(InMarkerList)
	{
		GenerateHeightData();
	}

	void SetState(const FMGPatternGridRuleExecutorState& InState) {
		State = InState;
	}
	
	FMGGridSceneMarkerList* GetMarkerList() const { return MarkerList; }

	virtual bool ContainsMarker(const FString& InMarkerName) const override {
		if (MarkerList) {
			const FMGGridSceneCell* Cell = MarkerList->GetCell(State.Coord);
			if (Cell) {
				return Cell->Contains(InMarkerName, State.CoordType);
			}
		}
		return false;
	}

	FQuat GetWorldRotation() const {
		float Angle = State.BaseAngleRad;
		if (State.Rotation90Index == 0 && State.CoordType == EMarkerGenGridPatternRuleType::EdgeY) { 
			Angle += PI * 0.5f;
		}
		else if (State.Rotation90Index == 2 && State.CoordType == EMarkerGenGridPatternRuleType::EdgeY) { 
			Angle += PI * 1.5f;
		}
		else if (State.Rotation90Index == 1 && State.CoordType == EMarkerGenGridPatternRuleType::EdgeX) { 
			Angle += PI * 0.5f;
		}
		else if (State.Rotation90Index == 3 && State.CoordType == EMarkerGenGridPatternRuleType::EdgeX) { 
			Angle += PI * 1.5f;
		}

		return FQuat(FVector::UpVector, Angle);
	}
	
	
	virtual void EmitMarker(const FString& InMarkerName, const FMGExecEmitMarkerSettings& InSettings) override {
		if (MarkerList) {
			FMGGridSceneCell* Cell = MarkerList->GetCell(State.Coord);
			if (Cell) {
				FMGCellHeightCoords* CellHeights = CellHeightCoords->GetValue(State.Coord);
				check(CellHeights);
				int32 CoordZ = CellHeights->GetHeight(State.CoordType);

				bool bUseRotationOverride = false;
				FQuat RotationOverride = FQuat::Identity;
				if (InSettings.CopyRotationFromMarkers.Num() > 0) {
					TSet<FString> CopyRotMarkerSet(InSettings.CopyRotationFromMarkers);
					const FMGGridSceneCell* ConstCell = Cell;
					if (const TArray<FDAMarkerInfo>* CellMarkerList = ConstCell->GetMarkerList(State.CoordType)) {
						for (const FDAMarkerInfo& MarkerInfo : *CellMarkerList) {
							if (CopyRotMarkerSet.Contains(MarkerInfo.MarkerName)) {
								RotationOverride = MarkerInfo.Transform.GetRotation();
								bUseRotationOverride = true;
								break;
							}
						}
					}
				}

				FQuat MarkerRotation = bUseRotationOverride ? RotationOverride : GetWorldRotation();
				
				// Calculate the transform
				FTransform Transform;
				{
					FVector WorldLocation;
					MarkerList->GetCellToWorldCoords(State.Coord, CoordZ, State.CoordType, WorldLocation);

					if (InSettings.CopyHeightFromMarkers.Num() > 0) {
						TSet<FString> CopyHeightMarkerSet(InSettings.CopyHeightFromMarkers);
						const FMGGridSceneCell* ConstCell = Cell;
						if (const TArray<FDAMarkerInfo>* CellMarkerList = ConstCell->GetMarkerList(State.CoordType)) {
							for (const FDAMarkerInfo& MarkerInfo : *CellMarkerList) {
								if (CopyHeightMarkerSet.Contains(MarkerInfo.MarkerName)) {
									WorldLocation.Z = MarkerInfo.Transform.GetLocation().Z;
									break;
								}
							}
						}
					}
					
					Transform = FTransform(MarkerRotation, WorldLocation);
				}
				
				FDAMarkerInfo NewMarker;
				NewMarker.Id = MarkerList->GenerateNextMarkerId();
				NewMarker.MarkerName = InMarkerName;
				NewMarker.Transform = Transform;

				Cell->Add(NewMarker, State.CoordType);
			}
		}
	}
	
	virtual void RemoveMarker(const FString& InMarkerName) override {
		if (MarkerList) {
			FMGGridSceneCell* Cell = MarkerList->GetCell(State.Coord);
			if (Cell) {
				Cell->Remove(InMarkerName, State.CoordType);
			}
		}
	}

	void GenerateHeightData() {
		const float CellHeight = FMath::Max(1.0f, MarkerList->GetCellHeight());
		CellHeightCoords = MakeShareable(new TMGGridSceneCells<FMGCellHeightCoords>(MarkerList->GetWorldOffset(), MarkerList->GetWorldSize()));
		
		TSet<FIntPoint> Visited;
		struct FQueueItem {
			FIntPoint Coord;
			int32 CoordZ;
		};
		TQueue<FQueueItem> Queue;

		// Run through the initial ground tiles and fill up the queue
		{
			const FIntPoint CoordStart = MarkerList->GetWorldOffset();
			const FIntPoint CoordEnd = MarkerList->GetWorldOffset() + MarkerList->GetWorldSize();
			for (int Y = CoordStart.Y; Y < CoordEnd.Y; Y++) {
				for (int X = CoordStart.X; X < CoordEnd.X; X++) {
					FIntPoint CellCoord(X, Y);
					const FMGGridSceneCell* Cell = MarkerList->GetCell(CellCoord);
					if (!Cell || Cell->GetTileMarkers().Num() == 0) continue;
					
					const FVector& GroundLocation = Cell->GetTileMarkers()[0].Transform.GetLocation();
					const int32 CoordZ = FMath::RoundToInt(GroundLocation.Z / CellHeight);
					Visited.Add(CellCoord);
					Queue.Enqueue({ CellCoord, CoordZ });
				}
			}
		}

		const TArray<FIntPoint> NeighborDeltas = {
			FIntPoint(-1, 0),
			FIntPoint(1, 0),
			FIntPoint(0, -1),
			FIntPoint(0, 1)
		};
		
		// Run a flood fill algorithm on the initial ground tiles
		FQueueItem Front;
		while (Queue.Dequeue(Front)) {
			if (FMGCellHeightCoords* CellCoordsZ = CellHeightCoords->GetValue(Front.Coord)) {
				CellCoordsZ->GroundCoordZ = Front.CoordZ;

				// Add the neighbors
				for (int d = 0; d < 4; d++) {
					FIntPoint NCoord = Front.Coord + NeighborDeltas[d];
					if (CellHeightCoords->IsCoordValid(NCoord) && !Visited.Contains(NCoord)) {
						Visited.Add(NCoord);
						Queue.Enqueue({NCoord, Front.CoordZ });
					}
				}
			}
		}

		// Fill up the rest of the coords (edges, corners) based on the ground tiles
		{
			const FIntPoint CoordStart = MarkerList->GetWorldOffset();
			const FIntPoint CoordEnd = MarkerList->GetWorldOffset() + MarkerList->GetWorldSize();
			for (int Y = CoordStart.Y; Y < CoordEnd.Y; Y++) {
				for (int X = CoordStart.X; X < CoordEnd.X; X++) {
					FIntPoint BaseCoord(X, Y);
					FMGCellHeightCoords* C00 = CellHeightCoords->GetValue(BaseCoord + FIntPoint(0, 0));
					FMGCellHeightCoords* C10 = CellHeightCoords->GetValue(BaseCoord + FIntPoint(-1, 0));
					FMGCellHeightCoords* C01 = CellHeightCoords->GetValue(BaseCoord + FIntPoint(0, -1));
					FMGCellHeightCoords* C11 = CellHeightCoords->GetValue(BaseCoord + FIntPoint(-1, -1));
					check(C00);
					
					int32 XEdgeZ = C00->GroundCoordZ;
					if (C01) XEdgeZ = FMath::Max(XEdgeZ, C01->GroundCoordZ);

					int32 YEdgeZ = C00->GroundCoordZ;
					if (C10) YEdgeZ = FMath::Max(XEdgeZ, C10->GroundCoordZ);

					int32 CornerZ = FMath::Max(XEdgeZ, YEdgeZ);
					if (C11) CornerZ = FMath::Max(CornerZ, C11->GroundCoordZ);

					C00->EdgeXCoordZ = XEdgeZ;
					C00->EdgeYCoordZ = YEdgeZ;
					C00->CornerCoordZ = CornerZ;
				}
			}
		}
	}
	
private:
	FMGPatternGridRuleExecutorState State;
	FMGGridSceneMarkerList* MarkerList = nullptr;
	TSharedPtr<TMGGridSceneCells<FMGCellHeightCoords>> CellHeightCoords;
};

FMarkerGenGridProcessor::FMarkerGenGridProcessor(const FTransform& InDungeonTransform, const FVector& InGridSize)
	: IMarkerGenProcessor(InDungeonTransform)
	, GridSize(InGridSize)
{
}

bool FMarkerGenGridProcessor::Process(const UMarkerGenLayer* InLayer, const TArray<FDAMarkerInfo>& InMarkers, const FRandomStream& InRandom, TArray<FDAMarkerInfo>& OutMarkers) {
	if (!InLayer || !InLayer->Pattern) {
		return false;
	}

	// Only process the compatible layers
	if (!InLayer->IsA<UMarkerGenGridLayer>()) {
		return false;
	}

	const UMarkerGenGridLayer* GridLayer = Cast<UMarkerGenGridLayer>(InLayer);
	
	if (!MarkerGenGridProcessorLib::ShouldProcessAssembly(GridLayer->Pattern)) {
		return false;
	}
	
	TArray<FMGGridPatternAssembly> Assemblies;
	FMGGridPatternAssemblyBuilder::GenerateAssembly(GridLayer->Pattern, Assemblies.AddDefaulted_GetRef());

	if (GridLayer->bRotateToFit) {
		// Rotate by 90 degrees three times so we have the patterns in the rotated setups of (0, 90, 180, 270)
		for (int i = 0; i < 3; i++) {
			const FMGGridPatternAssembly& Asm = Assemblies.Last();
			FMGGridPatternAssembly& RotatedAsm = Assemblies.AddDefaulted_GetRef();
			FMGGridPatternAssemblyBuilder::RotateAssembly90(Asm, RotatedAsm);
		}
	}

	// Bring the markers to origin (by inverse multiplying it from the dungeon marker position
	TArray<FDAMarkerInfo> LocalMarkers = InMarkers;
	MarkerGenGridProcessorLib::TransformMarkers(DungeonTransform.Inverse(), LocalMarkers);

	const int32 BoundsExpansion = GridLayer->bExpandMarkerDomain ? GridLayer->ExpandMarkerDomainAmount : 0;
	FMGGridSceneMarkerList MarkerList(GridSize, LocalMarkers, BoundsExpansion);
	FMGGridSceneCellBoolean OccupancyList(MarkerList.GetWorldOffset(), MarkerList.GetWorldSize());
	const TSharedPtr<FMGPatternGridRuleExecutor> Executor = MakeShareable(new FMGPatternGridRuleExecutor(&MarkerList));

	const FIntPoint WorldOffset = Executor->GetMarkerList()->GetWorldOffset();
	const FIntPoint WorldSize = Executor->GetMarkerList()->GetWorldSize();

	TArray<FMGPatternMatchCoord> PatternMatchCommands;
	for (int32 AsmIdx = 0; AsmIdx < Assemblies.Num(); AsmIdx++) {
		const FMGGridPatternAssembly& PatternAsm = Assemblies[AsmIdx];
		GeneratePatternMatchCommands(PatternAsm, AsmIdx, WorldOffset, WorldSize, PatternMatchCommands);
	}

	if (GridLayer->bRandomizeFittingOrder) {
		FMathUtils::Shuffle(PatternMatchCommands, InRandom);
	}

	for (const FMGPatternMatchCoord& Cmd : PatternMatchCommands) {
		if (InRandom.FRand() <= GridLayer->Probability) {
			ExecutePattern(Cmd.X, Cmd.Y, Assemblies[Cmd.AsmIdx], GridLayer, OccupancyList, Executor);
		}
	}
	
	MarkerList.GenerateMarkerList(OutMarkers);
	MarkerGenGridProcessorLib::TransformMarkers(DungeonTransform, OutMarkers);
	
	return true;
}

void FMarkerGenGridProcessor::GeneratePatternMatchCommands(const FMGGridPatternAssembly& InPattern, int32 InPatternAsmIdx, const FIntPoint& InWorldOffset,
		const FIntPoint& InWorldSize, TArray<FMGPatternMatchCoord>& OutCommands) {
	if (InPattern.Rules.Num() == 0) {
		return;
	}

	const FIntPoint PatternSize = InPattern.BoundMax - InPattern.BoundMin + FIntPoint(1, 1);
	const FIntPoint StartCoord = InWorldOffset - PatternSize;
	const FIntPoint EndCoord = InWorldOffset + InWorldSize;
	
	for (int Y = StartCoord.Y; Y <= EndCoord.Y; Y++) {
		for (int X = StartCoord.X; X <= EndCoord.X; X++) {
			OutCommands.Add({ X, Y, InPatternAsmIdx });
		}
	}
}

void FMarkerGenGridProcessor::ExecutePattern(int BaseX, int BaseY, const FMGGridPatternAssembly& InPattern, const UMarkerGenGridLayer* InGridLayer,
		FMGGridSceneCellBoolean& InOccupancyList, TSharedPtr<FMGPatternGridRuleExecutor> InExecutor) const {
	bool bShouldExecutePattern = true;
	
	// We'll insert a geometry here.  Make sure it wasn't inserted already in this layer
	if (!InGridLayer->bAllowInsertionOverlaps) {
		for (const FMGGridPatternAssemblyRule& Rule : InPattern.Rules) {
			if (Rule.bHintWillInsertAssetHere) {
				FIntPoint WorldCoord = FIntPoint(BaseX, BaseY) + Rule.Coord;
				FMGCellOccupancyInfo* CellOccupancyPtr = InOccupancyList.GetValue(WorldCoord);
				if (CellOccupancyPtr && CellOccupancyPtr->IsOccupied(Rule.RuleType)) {
					// This insertion will overlap with a previous geometry we inserted in this layer
					bShouldExecutePattern = false;
					break;
				}
			}
		}
	}

	// Check if the SameHeight constraint passes
	if (bShouldExecutePattern && InGridLayer->SameHeightMarkers.Num() > 0) {
		FMGGridSceneMarkerList* MarkerList = InExecutor->GetMarkerList();
		if (MarkerList) {
			const TSet<FString> MarkersToCheck(InGridLayer->SameHeightMarkers);
			TMap<FString, int32> MarkerHeights;
			for (const FMGGridPatternAssemblyRule& Rule : InPattern.Rules) {
				FIntPoint WorldCoord = FIntPoint(BaseX, BaseY) + Rule.Coord;
				if (const FMGGridSceneCell* Cell = MarkerList->GetCell(WorldCoord)) {
					if (const TArray<FDAMarkerInfo>* CellMarkers = Cell->GetMarkerList(Rule.RuleType)) {
						for (const FDAMarkerInfo& MarkerInfo : *CellMarkers) {
							if (MarkersToCheck.Contains(MarkerInfo.MarkerName)) {
								int32 MarkerCoordZ = MarkerInfo.Transform.GetLocation().Z / GridSize.Z;
								const int* SearchResult = MarkerHeights.Find(MarkerInfo.MarkerName);
								if (SearchResult) {
									const int32 ExistingCoordZ = *SearchResult;
									if (MarkerCoordZ != ExistingCoordZ) {
										bShouldExecutePattern = false;
										break;
									}
								}
								else {
									MarkerHeights.Add(MarkerInfo.MarkerName, MarkerCoordZ);
								}
							}
						}
					}
				}
				if (!bShouldExecutePattern) {
					break;
				}
			}
		}
	}
	
	if (bShouldExecutePattern) {
		for (const FMGGridPatternAssemblyRule& Rule : InPattern.Rules) {
			FIntPoint WorldCoord = FIntPoint(BaseX, BaseY) + Rule.Coord;
			FMGPatternGridRuleExecutorState ExecState;
			ExecState.Coord = WorldCoord;
			ExecState.CoordType = Rule.RuleType;
			ExecState.BaseAngleRad = InPattern.RotationAngleRad;
			ExecState.Rotation90Index= InPattern.Rotation90Index;
			InExecutor->SetState(ExecState);
			
			if (!MarkerGenGridProcessorLib::IsValid(Rule.RuleScript)) {
				bShouldExecutePattern = false;
				break;
			}

			// Check if the conditional graph for all the rules pass
			FMGConditionExecContext ExecContext;
			ExecContext.Executor = InExecutor;
			if (!Rule.RuleScript->ResultNode->Execute(ExecContext)) {
				bShouldExecutePattern = false;
				break;
			}
		}
	}

	if (bShouldExecutePattern) {
		// Run the action nodes on all the rules
		for (const FMGGridPatternAssemblyRule& Rule : InPattern.Rules) {
			FIntPoint WorldCoord = FIntPoint(BaseX, BaseY) + Rule.Coord;
			FMGPatternGridRuleExecutorState ExecState;
			ExecState.Coord = WorldCoord;
			ExecState.CoordType = Rule.RuleType;
			ExecState.BaseAngleRad = InPattern.RotationAngleRad;
			ExecState.Rotation90Index= InPattern.Rotation90Index;
			InExecutor->SetState(ExecState);

			FMGActionExecContext ExecContext;
			ExecContext.Executor = InExecutor;	
			Rule.RuleScript->PassEventNode->Execute(ExecContext);

			if (Rule.bHintWillInsertAssetHere) {
				FMGCellOccupancyInfo* CellOccupancyPtr = InOccupancyList.GetValue(WorldCoord);
				if (CellOccupancyPtr) {
					CellOccupancyPtr->SetOccupied(Rule.RuleType, true);
				}
			}
		}
	}
}

