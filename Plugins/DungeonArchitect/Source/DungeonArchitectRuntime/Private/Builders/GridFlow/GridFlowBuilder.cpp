//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Builders/GridFlow/GridFlowBuilder.h"

#include "Builders/GridFlow/GridFlowConfig.h"
#include "Builders/GridFlow/GridFlowModel.h"
#include "Builders/GridFlow/GridFlowQuery.h"
#include "Builders/GridFlow/GridFlowSelectorLogic.h"
#include "Builders/GridFlow/GridFlowToolData.h"
#include "Builders/GridFlow/GridFlowTransformLogic.h"
#include "Core/Dungeon.h"
#include "Core/DungeonMarkerNames.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/ExecGraph/FlowExecGraphScript.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTaskStructs.h"
#include "Frameworks/Flow/FlowProcessor.h"
#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/GridFlowAbstractGraph.h"
#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/GridFlowAbstractGraphDomain.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/GridFlowTilemap.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/GridFlowTilemapDomain.h"

#include "Components/BrushComponent.h"
#include "DrawDebugHelpers.h"
#include "EngineUtils.h"
#include <stack>

DEFINE_LOG_CATEGORY(GridFlowBuilderLog);

void UGridFlowBuilder::BuildDungeonImpl(UWorld* World) {
    GridFlowModel = Cast<UGridFlowModel>(model);
    GridFlowConfig = Cast<UGridFlowConfig>(config);
    GridFlowQuery = Cast<UGridFlowQuery>(query);
    WorldMarkers.Reset();

    if (!GridFlowModel) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Invalid dungeon model provided to the grid flow builder"));
        return;
    }

    if (!GridFlowConfig) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Invalid dungeon config provided to the grid flow builder"));
        return;
    }

    if (!ExecuteGraph()) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Failed to execute grid flow graph"));
    }
}

bool UGridFlowBuilder::ExecuteGraph() {
    GridFlowModel->AbstractGraph = nullptr;
    GridFlowModel->Tilemap = nullptr;

    UGridFlowAsset* GridFlowAsset = GridFlowConfig->GridFlow.LoadSynchronous();

    if (!GridFlowAsset) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Missing Grid Flow graph"));
        return false;
    }

    if (!GridFlowAsset->ExecScript) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Invalid Grid Flow graph state. Please resave in editor"));
        return false;
    }

    FFlowProcessor FlowProcessor;
    
    // Register the domains
    {
        FGridFlowProcessDomainExtender Extender;
        Extender.ExtendDomains(FlowProcessor);
    }
    
    const int32 MAX_RETRIES = FMath::Max(1, GridFlowConfig->MaxRetries);
    int32 NumTries = 0;
    FFlowProcessorResult Result;
    while (NumTries < MAX_RETRIES) {
        FFlowProcessorSettings GridFlowProcessorSettings;
        GridFlowProcessorSettings.AttributeList = AttributeList;
        GridFlowProcessorSettings.SerializedAttributeList = GridFlowConfig->ParameterOverrides;
        Result = FlowProcessor.Process(GridFlowAsset->ExecScript, Random, GridFlowProcessorSettings);
        NumTries++;
        if (Result.ExecResult == EFlowTaskExecutionResult::Success) {
            break;
        }
        if (Result.ExecResult == EFlowTaskExecutionResult::FailHalt) {
            break;
        }
    }

    if (Result.ExecResult != EFlowTaskExecutionResult::Success) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Failed to generate grid flow graph"));
        return false;
    }

    if (!GridFlowAsset->ExecScript->ResultNode) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Cannot find result node in the grid flow exec graph. Please resave the grid flow asset in the editor"));
        return false;
    }

    const FGuid ResultNodeId = GridFlowAsset->ExecScript->ResultNode->NodeId;
    if (FlowProcessor.GetNodeExecStage(ResultNodeId) != EFlowTaskExecutionStage::Executed) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Grid Flow Graph execution failed"));
        return false;
    }

    FFlowExecutionOutput ResultNodeState;
    FlowProcessor.GetNodeState(ResultNodeId, ResultNodeState);
    if (ResultNodeState.ExecutionResult != EFlowTaskExecutionResult::Success) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Grid Flow Result node execution did not succeed"));
        return false;
    }

    // Save a copy in the model
    {
        UGridFlowAbstractGraph* TemplateGraph = ResultNodeState.State->GetState<UGridFlowAbstractGraph>(UFlowAbstractGraphBase::StateTypeID);
        UGridFlowTilemap* TemplateTilemap = ResultNodeState.State->GetState<UGridFlowTilemap>(UGridFlowTilemap::StateTypeID);
        GridFlowModel->AbstractGraph = NewObject<UGridFlowAbstractGraph>(GridFlowModel, "AbstractGraph", RF_NoFlags, TemplateGraph);
        GridFlowModel->Tilemap = NewObject<UGridFlowTilemap>(GridFlowModel, "Tilemap", RF_NoFlags, TemplateTilemap);

        UGridFlowTilemapUserData* TilemapUserData = ResultNodeState.State->GetState<UGridFlowTilemapUserData>(UGridFlowTilemapUserData::StateTypeID);
        if (TilemapUserData) {
            GridFlowModel->bWallsAsEdges = TilemapUserData->bWallsAsEdges;
        }
    }
    return true;
}

namespace {
    bool IsWallOnOffset(UGridFlowTilemap* Tilemap, const FFlowTilemapCell& Cell, int32 dx, int32 dy) {
        FFlowTilemapCell* NCell = Tilemap->GetSafe(Cell.TileCoord.X + dx, Cell.TileCoord.Y + dy);
        if (!NCell) return false;
        return NCell->CellType == EFlowTilemapCellType::Wall || NCell->CellType == EFlowTilemapCellType::Door;
    }

    bool IsLayoutTile(UGridFlowTilemap* Tilemap, const FFlowTilemapCell& Cell, int32 dx, int32 dy) {
        FFlowTilemapCell* NCell = Tilemap->GetSafe(Cell.TileCoord.X + dx, Cell.TileCoord.Y + dy);
        return NCell ? NCell->bLayoutCell : false;
    }

}

void UGridFlowBuilder::EmitDungeonMarkers_Implementation() {
    Super::EmitDungeonMarkers_Implementation();

    GridFlowModel = Cast<UGridFlowModel>(model);
    GridFlowConfig = Cast<UGridFlowConfig>(config);

    const FVector& GridSize = GridFlowConfig->GridSize;
    UWorld* World = Dungeon ? Dungeon->GetWorld() : nullptr;
    FTransform DungeonTransform = Dungeon ? Dungeon->GetActorTransform() : FTransform::Identity;
    ClearSockets();

    UGridFlowAbstractGraph* AbstractGraph = GridFlowModel->AbstractGraph;
    UGridFlowTilemap* Tilemap = GridFlowModel->Tilemap;

    if (!AbstractGraph || !Tilemap) {
        UE_LOG(GridFlowBuilderLog, Error, TEXT("Invalid grid flow model state. Cannot build grid flow dungeon"));
        return;
    }

    int32 Width = Tilemap->GetWidth();
    int32 Height = Tilemap->GetHeight();

    int32 OffsetIdxX = 0;
    int32 OffsetIdxY = 0;
    if (GridFlowConfig->bAlignDungeonAtCenter) {
        OffsetIdxX = Width / 2;
        OffsetIdxY = Height / 2;
    }

    GridFlowModel->BuildTileOffset = FIntPoint(OffsetIdxX, OffsetIdxY);

    TMap<FGuid, const UFlowGraphItem*> Items;
    {
        TArray<UFlowGraphItem*> ItemArray;
        GridFlowModel->AbstractGraph->GetAllItems(ItemArray);
        for (const UFlowGraphItem* Item : ItemArray) {
            const UFlowGraphItem*& ItemRef = Items.FindOrAdd(Item->ItemId);
            ItemRef = Item;
        }
    }

    FVector TileExtent = GridSize * 0.5f;
    TileExtent.Z = 0;
    for (int32 y = 0; y < Height; y++) {
        for (int32 x = 0; x < Width; x++) {
            bool bEmitFloor = false;
            const FFlowTilemapCell& Cell = Tilemap->Get(x, y);
            if (Cell.CellType != EFlowTilemapCellType::Empty) {
                FString MarkerName;
                float Angle = 0;

                if (Cell.CellType == EFlowTilemapCellType::Floor) {
                    bEmitFloor = true;
                }
                else if (Cell.CellType == EFlowTilemapCellType::Door) {
                    bEmitFloor = true;
                    MarkerName = FGridFlowBuilderMarkers::MARKER_DOOR;

                    FFlowTilemapCellDoorInfo DoorInfo;
                    if (Tilemap->GetDoorMeta(FFlowTilemapCoord(Cell.TileCoord), DoorInfo)) {
                        if (DoorInfo.bOneWay) {
                            MarkerName = FGridFlowBuilderMarkers::MARKER_DOOR_ONEWAY;
                        }
                        else if (DoorInfo.bLocked) {
                            // Remove the door marker. This will be replaced with the lock marker
                            MarkerName = "";
                        }
                        Angle = DoorInfo.Angle;
                    }
                }
                else if (Cell.CellType == EFlowTilemapCellType::Custom) {
                    MarkerName = Cell.CustomCellInfo.MarkerName;
                }
                else if (Cell.CellType == EFlowTilemapCellType::Wall) {
                    MarkerName = FGridFlowBuilderMarkers::MARKER_WALL;
                    bool bLeft = IsWallOnOffset(Tilemap, Cell, -1, 0);
                    bool bRight = IsWallOnOffset(Tilemap, Cell, 1, 0);
                    bool bTop = IsWallOnOffset(Tilemap, Cell, 0, -1);
                    bool bBottom = IsWallOnOffset(Tilemap, Cell, 0, 1);

                    int32 NWallCount = 0;
                    if (bLeft) NWallCount++;
                    if (bRight) NWallCount++;
                    if (bTop) NWallCount++;
                    if (bBottom) NWallCount++;

                    if (NWallCount == 4) {
                        MarkerName = FGridFlowBuilderMarkers::MARKER_WALL_X;
                        Angle = 0;
                    }
                    else if (NWallCount == 3) {
                        MarkerName = FGridFlowBuilderMarkers::MARKER_WALL_T;
                        if (!bTop) Angle = 0;
                        else if (!bRight) Angle = 90;
                        else if (!bBottom) Angle = 180;
                        else if (!bLeft) Angle = 270;
                    }
                    else if (NWallCount == 2) {
                        if (bRight && bBottom) {
                            MarkerName = FGridFlowBuilderMarkers::MARKER_WALL_L;
                            Angle = 0;
                        }
                        else if (bBottom && bLeft) {
                            MarkerName = FGridFlowBuilderMarkers::MARKER_WALL_L;
                            Angle = 90;
                        }
                        else if (bLeft && bTop) {
                            MarkerName = FGridFlowBuilderMarkers::MARKER_WALL_L;
                            Angle = 180;
                        }
                        else if (bTop && bRight) {
                            MarkerName = FGridFlowBuilderMarkers::MARKER_WALL_L;
                            Angle = 270;
                        }
                        else if (bTop && bBottom) {
                            Angle = 90;
                        }
                    }
                }

                int32 TileX = x - OffsetIdxX;
                int32 TileY = y - OffsetIdxY;
                int32 TileZ = Cell.Height;
                FVector TileCenter = FVector(TileX + 0.5f, TileY + 0.5f, TileZ) * GridSize;
                if (MarkerName.Len() > 0) {
                    EmitMarkerAt(TileCenter, MarkerName, Angle);
                }

                if (bEmitFloor) {
                    EmitMarkerAt(TileCenter, FGridFlowBuilderMarkers::MARKER_GROUND, Angle);
                }

                if (Cell.CellType != EFlowTilemapCellType::Empty && Cell.bHasOverlay) {
                    EmitMarkerAt(TileCenter, Cell.Overlay.MarkerName, Angle);
                }

                if (Cell.bHasItem) {
                    const UFlowGraphItem** SearchResult = Items.Find(Cell.ItemId);
                    if (SearchResult) {
                        const UFlowGraphItem* Item = *SearchResult;
                        TSharedPtr<FGridFlowBuilderMarkerUserData> ItemMetaData = MakeShareable(
                            new FGridFlowBuilderMarkerUserData);
                        ItemMetaData->TileCoord = FFlowTilemapCoord(Cell.TileCoord);
                        ItemMetaData->bIsItem = true;
                        ItemMetaData->Item = Item;
                        EmitMarkerAt(TileCenter, Item->MarkerName, Angle, ItemMetaData);
                    }
                }
            }
        }
    }

    TSet<FIntPoint> WallSeparators;
    TSet<FIntPoint> FenceSeparators;
    for (int32 y = 0; y <= Height; y++) {
        for (int32 x = 0; x <= Width; x++) {
            const int32 TileX = x - OffsetIdxX;
            const int32 TileY = y - OffsetIdxY;
            const FFlowTilemapEdge* EdgeH = Tilemap->GetEdgeHSafe(x, y);
            if (EdgeH) {
                EmitEdgeMarker(*EdgeH, FVector(TileX, TileY, EdgeH->HeightCoord), GridSize, Tilemap, Items);
                if (EdgeH->EdgeType == EFlowTilemapEdgeType::Wall) {
                    WallSeparators.Add(FIntPoint(x, y));
                    WallSeparators.Add(FIntPoint(x + 1, y));
                }
                if (EdgeH->EdgeType == EFlowTilemapEdgeType::Fence) {
                    FenceSeparators.Add(FIntPoint(x, y));
                    FenceSeparators.Add(FIntPoint(x + 1, y));
                }
            }

            const FFlowTilemapEdge* EdgeV = Tilemap->GetEdgeVSafe(x, y);
            if (EdgeV) {
                EmitEdgeMarker(*EdgeV, FVector(TileX, TileY, EdgeV->HeightCoord), GridSize, Tilemap, Items);
                if (EdgeV->EdgeType == EFlowTilemapEdgeType::Wall) {
                    WallSeparators.Add(FIntPoint(x, y));
                    WallSeparators.Add(FIntPoint(x, y + 1));
                }
                if (EdgeV->EdgeType == EFlowTilemapEdgeType::Fence) {
                    FenceSeparators.Add(FIntPoint(x, y));
                    FenceSeparators.Add(FIntPoint(x, y + 1));
                }
            }
        }
    }

    FIntPoint BaseOffset = FIntPoint(-OffsetIdxX, -OffsetIdxY);
    const float TileZ = 0;
    for (const FIntPoint& FenceSeparator : FenceSeparators) {
        FIntPoint Coord = FenceSeparator + BaseOffset;
        FVector Location = FVector(Coord.X, Coord.Y, TileZ) * GridSize;
        EmitMarkerAt(Location, FGridFlowBuilderMarkers::MARKER_FENCE_SEPARATOR, 0);

        // Adding this for legacy purpose.  This will be removed in the future
        EmitMarkerAt(Location, FGridFlowBuilderMarkers::MARKER_CAVE_FENCE_SEPARATOR, 0);
    }
    
    for (const FIntPoint& WallSeparator : WallSeparators) {
        FIntPoint Coord = WallSeparator + BaseOffset;
        FVector Location = FVector(Coord.X, Coord.Y, TileZ) * GridSize;
        EmitMarkerAt(Location, FGridFlowBuilderMarkers::MARKER_WALL_SEPARATOR, 0);
    }
}

void UGridFlowBuilder::EmitMarkerAt(const FVector& WorldLocation, const FString& MarkerName, float Angle,
                                    TSharedPtr<class IDungeonMarkerUserData> InUserData) {
    FQuat Rotation = FQuat::MakeFromEuler(FVector(0, 0, Angle));
    EmitMarkerAt(WorldLocation, MarkerName, Rotation, InUserData);
}

void UGridFlowBuilder::EmitEdgeMarker(const FFlowTilemapEdge& Edge, const FVector& TileCoord, const FVector& GridSize,
                                      UGridFlowTilemap* Tilemap, const TMap<FGuid, const UFlowGraphItem*>& Items) {
    check(Edge.EdgeCoord.bIsEdgeCoord);
    
    if (Edge.EdgeType == EFlowTilemapEdgeType::Empty) {
        return;
    }

    FVector Location = FVector(TileCoord.X, TileCoord.Y, TileCoord.Z);
    Location += Edge.EdgeCoord.bHorizontalEdge ? FVector(0.5f, 0, 0) : FVector(0, 0.5f, 0);
    Location *= GridSize;
    
    if (Edge.EdgeType == EFlowTilemapEdgeType::Wall) {
        EmitMarkerAt(Location, FGridFlowBuilderMarkers::MARKER_WALL, Edge.MarkerAngle);
    }
    else if (Edge.EdgeType == EFlowTilemapEdgeType::Door) {
        FFlowTilemapCellDoorInfo DoorMeta;
        if (Tilemap->GetDoorMeta(Edge.EdgeCoord, DoorMeta)) {
            if (!DoorMeta.bLocked) {
                const FString MarkerName = DoorMeta.bOneWay ? FGridFlowBuilderMarkers::MARKER_DOOR_ONEWAY : FGridFlowBuilderMarkers::MARKER_DOOR; 
                EmitMarkerAt(Location, MarkerName, DoorMeta.Angle);
            }
            if (Edge.bHasItem) {
                const UFlowGraphItem* const* SearchResult = Items.Find(Edge.ItemId);
                if (SearchResult) {
                    const UFlowGraphItem* Item = *SearchResult;
                    TSharedPtr<FGridFlowBuilderMarkerUserData> ItemMetaData = MakeShareable(
                        new FGridFlowBuilderMarkerUserData);
                    ItemMetaData->TileCoord = Edge.EdgeCoord;
                    ItemMetaData->bIsItem = true;
                    ItemMetaData->Item = Item;
                    EmitMarkerAt(Location, Item->MarkerName, DoorMeta.Angle, ItemMetaData);
                }
            }
        }
    }
    else if (Edge.EdgeType == EFlowTilemapEdgeType::Fence) {
        EmitMarkerAt(Location, FGridFlowBuilderMarkers::MARKER_CAVE_FENCE, Edge.MarkerAngle);
        EmitMarkerAt(Location, FGridFlowBuilderMarkers::MARKER_FENCE, Edge.MarkerAngle);
    }
}

bool UGridFlowBuilder::IdentifyBuildSucceeded() const {
    return GridFlowModel && GridFlowModel->AbstractGraph && GridFlowModel->Tilemap;
}

void UGridFlowBuilder::EmitMarkerAt(const FVector& WorldLocation, const FString& MarkerName, const FQuat& Rotation,
                                    TSharedPtr<class IDungeonMarkerUserData> InUserData) {
    FTransform Transform = FTransform::Identity;
    Transform.SetLocation(WorldLocation);
    Transform.SetRotation(Rotation);

    if (Dungeon) {
        // Transform the marker relative to the dungeon
        Transform = Transform * Dungeon->GetActorTransform();
    }

    AddMarker(MarkerName, Transform, InUserData);
}

void UGridFlowBuilder::DrawDebugData(UWorld* InWorld, bool bPersistant /*= false*/, float lifeTime /*= 0*/) {
    if (!GridFlowModel || !GridFlowConfig) {
        return;
    }
    
    const FVector GridSize = GridFlowConfig->GridSize;
    UGridFlowTilemap* Tilemap = GridFlowModel->Tilemap;
    if (!Tilemap) {
        return;
    }

    int32 Width = Tilemap->GetWidth();
    int32 Height = Tilemap->GetHeight();

    FVector TileExtent = GridSize * 0.5f;
    TileExtent.Z = 0;
    for (int32 y = 0; y < Height; y++) {
        for (int32 x = 0; x < Width; x++) {
            FFlowTilemapCell& Cell = Tilemap->Get(x, y);
            if (Cell.CellType != EFlowTilemapCellType::Empty) {
                FColor TileColor = FColor(0, 0, 0);
                if (Cell.CellType == EFlowTilemapCellType::Floor) {
                    TileColor = FColor(255, 0, 0);
                }
                else if (Cell.CellType == EFlowTilemapCellType::Wall) {
                    TileColor = FColor(0, 255, 0);
                }
                else if (Cell.CellType == EFlowTilemapCellType::Door) {
                    TileColor = FColor(0, 0, 255);
                }
                TileColor.A = 64;

                FVector TileCenter = FVector(x + 0.5f, y + 0.5f, 0) * GridSize;
                DrawDebugSolidBox(InWorld, TileCenter, TileExtent, FQuat::Identity, TileColor, bPersistant, lifeTime);
            }
        }
    }
}

TSubclassOf<UDungeonModel> UGridFlowBuilder::GetModelClass() {
    return UGridFlowModel::StaticClass();
}

TSubclassOf<UDungeonConfig> UGridFlowBuilder::GetConfigClass() {
    return UGridFlowConfig::StaticClass();
}

TSubclassOf<UDungeonToolData> UGridFlowBuilder::GetToolDataClass() {
    return UGridFlowToolData::StaticClass();
}

TSubclassOf<UDungeonQuery> UGridFlowBuilder::GetQueryClass() {
    return UGridFlowQuery::StaticClass();
}

bool UGridFlowBuilder::ProcessSpatialConstraint(UDungeonSpatialConstraint* SpatialConstraint,
                                                const FTransform& Transform, FQuat& OutRotationOffset) {
    return false;
}

void UGridFlowBuilder::GetDefaultMarkerNames(TArray<FString>& OutMarkerNames) {
    OutMarkerNames.Reset();
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_GROUND);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_WALL);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_WALL_L);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_WALL_T);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_WALL_X);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_DOOR);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_DOOR_ONEWAY);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_CAVE_FENCE);
    OutMarkerNames.Add(FGridFlowBuilderMarkers::MARKER_CAVE_FENCE_SEPARATOR);
}

bool UGridFlowBuilder::PerformSelectionLogic(const TArray<UDungeonSelectorLogic*>& SelectionLogics,
                                             const FDAMarkerInfo& socket) {
    for (UDungeonSelectorLogic* SelectionLogic : SelectionLogics) {
        UGridFlowSelectorLogic* GridFlowSelectionLogic = Cast<UGridFlowSelectorLogic>(SelectionLogic);
        if (!GridFlowSelectionLogic) {
            UE_LOG(GridFlowBuilderLog, Warning,
                   TEXT("Invalid selection logic specified.  GridFlowSelectorLogic expected"));
            return false;
        }

        // Perform blueprint based selection logic
        FIntPoint TileOffset = GridFlowModel ? GridFlowModel->BuildTileOffset : FIntPoint::ZeroValue;
        FVector Location = socket.Transform.GetLocation();
        FVector GridSize = GridFlowConfig->GridSize;
        int32 TileX = FMath::FloorToInt(Location.X / GridSize.X) + TileOffset.X;
        int32 TileY = FMath::FloorToInt(Location.Y / GridSize.Y) + TileOffset.Y;

        bool bSelected = GridFlowSelectionLogic->SelectNode(GridFlowModel, GridFlowConfig, this, GridFlowQuery, Random,
                                                            TileX, TileY, socket.Transform);
        if (!bSelected) {
            return false;
        }
    }
    return true;
}

FTransform UGridFlowBuilder::PerformTransformLogic(const TArray<UDungeonTransformLogic*>& TransformLogics,
                                                   const FDAMarkerInfo& socket) {
    FTransform result = FTransform::Identity;

    for (UDungeonTransformLogic* TransformLogic : TransformLogics) {
        UGridFlowTransformLogic* GridFlowTransformLogic = Cast<UGridFlowTransformLogic>(TransformLogic);
        if (!GridFlowTransformLogic) {
            UE_LOG(GridFlowBuilderLog, Warning,
                   TEXT("Invalid transform logic specified.  GridFlowTransformLogic expected"));
            continue;
        }

        FVector Location = socket.Transform.GetLocation();
        FVector GridSize = GridFlowConfig->GridSize;
        int32 GridX = FMath::FloorToInt(Location.X / GridSize.X);
        int32 GridY = FMath::FloorToInt(Location.Y / GridSize.Y);
        FTransform LogicOffset;
        if (TransformLogic) {
            GridFlowTransformLogic->GetNodeOffset(GridFlowModel, GridFlowConfig, GridFlowQuery, Random, GridX, GridY,
                                                  LogicOffset);
        }
        else {
            LogicOffset = FTransform::Identity;
        }

        FTransform out;
        FTransform::Multiply(&out, &LogicOffset, &result);
        result = out;
    }
    return result;

}

void UGridFlowBuilder::ProcessThemeItemUserData(TSharedPtr<IDungeonMarkerUserData> InUserData, AActor* SpawnedActor) {
    if (InUserData.IsValid() && SpawnedActor) {
        TSharedPtr<FGridFlowBuilderMarkerUserData> MarkerData = StaticCastSharedPtr<FGridFlowBuilderMarkerUserData
        >(InUserData);
        if (MarkerData->bIsItem) {
            UDungeonFlowItemMetadataComponent* ItemComponent = SpawnedActor->FindComponentByClass<UDungeonFlowItemMetadataComponent>();
            if (ItemComponent) {
                ItemComponent->SetFlowItem(MarkerData->Item);
            }
        }
    }
}

///////////////////////////////// FGridFlowProcessDomainExtender /////////////////////////////////
void FGridFlowProcessDomainExtender::ExtendDomains(FFlowProcessor& InProcessor) {
    const TSharedPtr<FGridFlowAbstractGraphDomain> AbstractGraphDomain = MakeShareable(new FGridFlowAbstractGraphDomain);
    InProcessor.RegisterDomain(AbstractGraphDomain);

    const TSharedPtr<FGridFlowTilemapDomain> TilemapDomain = MakeShareable(new FGridFlowTilemapDomain);
    InProcessor.RegisterDomain(TilemapDomain);
}

