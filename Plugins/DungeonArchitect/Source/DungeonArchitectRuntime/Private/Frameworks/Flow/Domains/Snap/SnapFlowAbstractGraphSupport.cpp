//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Flow/Domains/Snap/SnapFlowAbstractGraphSupport.h"

#include "Core/Utils/MathUtils.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraphQuery.h"
#include "Frameworks/Snap/Lib/Connection/SnapConnectionInfo.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleDatabase.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowStats.h"

//////////////////////////////////////// Snap Abstract Graph Node Group Generator //////////////////////////////////////////////

DEFINE_LOG_CATEGORY_STATIC(LogSGFSupport, Log, All);

class UFANodeSnapDomainData;

namespace SGFLib {
	class FSGFLocalCoordBuilder {
	public:
		void GetCoords(const FSGFModuleAssembly& InAssembly, TArray<FIntVector>& OutVolumeCoords, TArray<FIntVector>& OutSurfaceCoords) {
			const FIntVector NumChunks = InAssembly.NumChunks;

			// Generate the volume node coords
			{
				TArray<FIntVector>* VolumeCoordsPtr = VolumeCoordsMap.Find(NumChunks);
				if (VolumeCoordsPtr) {
					OutVolumeCoords = *VolumeCoordsPtr;
				}
				else {
					TArray<FIntVector>& VolumeCoords = VolumeCoordsMap.FindOrAdd(NumChunks);
					for (int dz = 0; dz < NumChunks.Z; dz++) {
						for (int dy = 0; dy < NumChunks.Y; dy++) {
							for (int dx = 0; dx < NumChunks.X; dx++) {
								VolumeCoords.Add(FIntVector(dx, dy, dz));
							}
						}
					}
					OutVolumeCoords = VolumeCoords;
				}
			}

			// Generate the surface node coords
			TSet<FIntVector> SurfaceCoords;
			
			// Process along the X-axis
			{
				for (int X = 0; X < NumChunks.X; X++)
				{
					for (int Z = 0; Z < NumChunks.Z; Z++)
					{
						const FSGFModuleAssemblySideCell& cellFront = InAssembly.Front.Get(X, Z);
						const FSGFModuleAssemblySideCell& cellBack = InAssembly.Back.Get(NumChunks.X - 1 - X, Z);
						
						if (cellFront.HasConnection()) SurfaceCoords.Add(FIntVector(X, 0, Z));
						if (cellBack.HasConnection()) SurfaceCoords.Add(FIntVector(X, NumChunks.Y - 1, Z));
					}
				}
			}

			// Process along the Y-axis
			{
				for (int Y = 0; Y < NumChunks.Y; Y++)
				{
					for (int Z = 0; Z < NumChunks.Z; Z++)
					{
						const FSGFModuleAssemblySideCell& cellRight = InAssembly.Right.Get(NumChunks.Y - 1 - Y, Z);
						const FSGFModuleAssemblySideCell& cellLeft = InAssembly.Left.Get(Y, Z);
						if (cellRight.HasConnection()) SurfaceCoords.Add(FIntVector(0, Y, Z));
						if (cellLeft.HasConnection()) SurfaceCoords.Add(FIntVector(NumChunks.X - 1, Y, Z));
					}
				}
			}

			// Process along the Z-axis
			{
				for (int X = 0; X < NumChunks.X; X++)
				{
					for (int Y = 0; Y < NumChunks.Y; Y++)
					{
						const FSGFModuleAssemblySideCell& cellDown = InAssembly.Down.Get(X, Y);
						const FSGFModuleAssemblySideCell& cellTop = InAssembly.Top.Get(X, Y);
						if (cellDown.HasConnection()) SurfaceCoords.Add(FIntVector(X, Y, 0));
						if (cellTop.HasConnection()) SurfaceCoords.Add(FIntVector(X, Y, NumChunks.Z - 1));
					}
				}
			}

			OutSurfaceCoords = SurfaceCoords.Array();
		}
	
	private:
		TMap<FIntVector, TArray<FIntVector>> VolumeCoordsMap;
	};

	class FSimpleLocalCoordBuilder {
	public:
		void GetCoords(const FIntVector& InGroupSize, TArray<FIntVector>& OutVolumeCoords, TArray<FIntVector>& OutSurfaceCoords) {
			TArray<FIntVector>* VolumeCoordsPtr = VolumeCoordsMap.Find(InGroupSize);
			TArray<FIntVector>* SurfaceCoordsPtr = SurfaceCoordsMap.Find(InGroupSize);

			if (VolumeCoordsPtr) {
				check(SurfaceCoordsPtr);
				OutVolumeCoords = *VolumeCoordsPtr;
				OutSurfaceCoords = *SurfaceCoordsPtr;
				return;
			}

			check (!SurfaceCoordsPtr);
			TArray<FIntVector>& VolumeCoords = VolumeCoordsMap.FindOrAdd(InGroupSize);
			TArray<FIntVector>& SurfaceCoords = SurfaceCoordsMap.FindOrAdd(InGroupSize);
			
			for (int dz = 0; dz < InGroupSize.Z; dz++) {
				for (int dy = 0; dy < InGroupSize.Y; dy++) {
					for (int dx = 0; dx < InGroupSize.X; dx++) {
						VolumeCoords.Add(FIntVector(dx, dy, dz));
						if (dx == 0 || dx == InGroupSize.X - 1 ||
								dy == 0 || dy == InGroupSize.Y - 1 ||
								dz == 0 || dz == InGroupSize.Z - 1) {
							SurfaceCoords.Add(FIntVector(dx, dy, dz));
								}
					}
				}
			}

			OutVolumeCoords = VolumeCoords;
			OutSurfaceCoords = SurfaceCoords;
		}
	
	private:
		TMap<FIntVector, TArray<FIntVector>> VolumeCoordsMap;
		TMap<FIntVector, TArray<FIntVector>> SurfaceCoordsMap;
	};

	
	class IHasher {
	public:
		virtual ~IHasher() {}
		virtual void Add(uint32 InCode) = 0;
		virtual void Clear() = 0;
		virtual uint32 Generate() const = 0;
	};

	class FCRCHasher : IHasher {
	public:
		virtual void Add(uint32 InCode) override {
			Data.Add(InCode);
		}
		virtual uint32 Generate() const override {
			return FCrc::MemCrc32(Data.GetData(), Data.Num());
		}

		virtual void Clear() override {
			Data.Reset();
		}
		
	private:
		TArray<uint32> Data;
	};
	
	class FCombineHasher : public IHasher {
	public:
		virtual void Add(uint32 InCode) override {
			HashCode = HashCombine(HashCode, InCode);
		}
		virtual uint32 Generate() const override {
			return HashCode;
		}

		virtual void Clear() override {
			HashCode = 0;
		}
		
	private:
		uint32 HashCode = 0;
	};
	

	template<typename THasher>
	uint32 GenerateHashCode(const FSGFModuleAssembly& InAssembly, const TArray<USnapConnectionInfo*>& InModuleConnections, const FName& InCategory, float InWeight) {
		THasher Hasher;
		const FIntVector NumChunks = InAssembly.NumChunks;
		
		Hasher.Add(NumChunks.X);
		Hasher.Add(NumChunks.Y);
		Hasher.Add(NumChunks.Z);

		auto FnSerializeAsmSide = [&Hasher, &InModuleConnections](const FSGFModuleAssemblySide& InSide) {
			Hasher.Add(InSide.Width);
			Hasher.Add(InSide.Height);
			for (const FSGFModuleAssemblySideCell& ConnectionInfo : InSide.ConnectionIndices) {
				USnapConnectionInfo* Connection = nullptr;
				FName ConnectionCategory = NAME_None;
				if (ConnectionInfo.HasConnection() && InModuleConnections.IsValidIndex(ConnectionInfo.ConnectionIdx)) {
					Connection = InModuleConnections[ConnectionInfo.ConnectionIdx];
					if (Connection) {
						ConnectionCategory = Connection->ConnectionCategory;
					}
				}
            
				Hasher.Add(Connection ? 1 : 0);
				Hasher.Add(GetTypeHash(ConnectionCategory));
			}
		};

		FnSerializeAsmSide(InAssembly.Front);
		FnSerializeAsmSide(InAssembly.Left);
		FnSerializeAsmSide(InAssembly.Back);
		FnSerializeAsmSide(InAssembly.Right);
		FnSerializeAsmSide(InAssembly.Top);
		FnSerializeAsmSide(InAssembly.Down);

		Hasher.Add(GetTypeHash(InCategory));
		Hasher.Add(GetTypeHash(InWeight));
    
		return Hasher.Generate();
	}

}

FSnapFlowAGNodeGroupGenerator::FSnapFlowAGNodeGroupGenerator(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase, TSharedPtr<FSGFNodeCategorySelector> InNodeCategorySelector)
	: ModuleDB(InModuleDatabase)
	, NodeCategorySelector(InNodeCategorySelector) 
{
	if (ModuleDB.IsValid()) {
		TSet<uint32> VisitedAsmHashes;
		
		for (int ModIdx = 0; ModIdx < ModuleDB->Modules.Num(); ModIdx++) {
			const FSnapGridFlowModuleDatabaseItem& Module = ModuleDB->Modules[ModIdx];
			TArray<USnapConnectionInfo*> ModuleConnections;
			for (const FSnapGridFlowModuleDatabaseConnectionInfo& Connection : Module.Connections) {
				ModuleConnections.Add(Connection.ConnectionInfo);
			}
			
			for (int AsmIdx = 0; AsmIdx <  Module.RotatedAssemblies.Num(); AsmIdx++) {
				const FSGFModuleAssembly& Assembly = Module.RotatedAssemblies[AsmIdx];
				uint32 AsmHashCode = SGFLib::GenerateHashCode<SGFLib::FCombineHasher>(Assembly, ModuleConnections, Module.Category, Module.SelectionWeight);
				
				if (!VisitedAsmHashes.Contains(AsmHashCode)) {
					VisitedAsmHashes.Add(AsmHashCode);
					
					FSnapFlowAGNodeGroupSetting& Setting = GroupSettings.AddDefaulted_GetRef();
					Setting.Weight = Module.SelectionWeight;
					Setting.GroupSize = Assembly.NumChunks;
					Setting.ModuleAssembly = Assembly;
					Setting.Category = Module.Category;
				}
			}
		}

		static SGFLib::FSGFLocalCoordBuilder CoordBuilder;
		for (FSnapFlowAGNodeGroupSetting& Setting : GroupSettings) {
			CoordBuilder.GetCoords(Setting.ModuleAssembly, Setting.LocalVolumeCoords, Setting.LocalSurfaceCoords);
		}
	}
	else {
		FSnapFlowAGNodeGroupSetting& DefaultSetting = GroupSettings.AddDefaulted_GetRef();
		DefaultSetting.Weight = 1;
		DefaultSetting.GroupSize = FIntVector(1, 1, 1);
		DefaultSetting.LocalSurfaceCoords = { FIntVector::ZeroValue };
		DefaultSetting.LocalVolumeCoords = { FIntVector::ZeroValue };
	}
 
}

void FSnapFlowAGNodeGroupGenerator::Generate(const FFlowAbstractGraphQuery& InGraphQuery, const UFlowAbstractNode* InCurrentNode,
				int32 InPathIndex, int32 InPathLength, const FRandomStream& InRandom, const TSet<FGuid>& InVisited,
				TArray<FFlowAGPathNodeGroup>& OutGroups) const
{
	
	
	const UFlowAbstractNode* Node = InCurrentNode;
	if (!Node || !NodeCategorySelector.IsValid()) {
		return;
	}

	if (GroupSettings.Num() == 0 || !ModuleDB.IsValid()) {
		const FNullFlowAGNodeGroupGenerator NullGenerator;
		NullGenerator.Generate(InGraphQuery, Node, InPathIndex, InPathLength, InRandom, InVisited, OutGroups);
		return;
	}


	const TSet<FName> AllowedCategories = TSet<FName>(NodeCategorySelector->GetCategoriesAtNode(InPathIndex, InPathLength));
	TArray<FSnapFlowAGNodeGroupSetting> FilteredGroupSettings;
	for (const FSnapFlowAGNodeGroupSetting& GroupSetting : GroupSettings) {
		if (AllowedCategories.Contains(GroupSetting.Category)) {
			FSnapFlowAGNodeGroupSetting FilteredSetting = GroupSetting;
			// Add randomness to similar weighted objects 
			FilteredSetting.Weight += InRandom.FRand() * 0.0001f;
			FilteredGroupSettings.Add(FilteredSetting);
		}
	}

	for (const FSnapFlowAGNodeGroupSetting& GroupSetting : FilteredGroupSettings) {
		for (const FIntVector& LocalSurfaceCoord : GroupSetting.LocalSurfaceCoords) {
			bool bValid = true;
			FIntVector BaseCoord = FMathUtils::ToIntVector(Node->Coord, true) - LocalSurfaceCoord;
			for (const FIntVector& LocalVolumeCoord : GroupSetting.LocalVolumeCoords) {
				FIntVector GroupNodeCoord = BaseCoord + LocalVolumeCoord;
				const UFlowAbstractNode* TestNode = InGraphQuery.GetNodeObjectAtCoord(GroupNodeCoord);
 				if (!TestNode || InVisited.Contains(TestNode->NodeId) || TestNode->bActive) {
					bValid = false;
					break;
				}
			}

			if (bValid) {
				// Add this group
				FFlowAGPathNodeGroup& NewGroup = OutGroups.AddDefaulted_GetRef();
				NewGroup.bIsGroup = true;
				NewGroup.Weight = GroupSetting.Weight;
				for (const FIntVector& LocalVolumeCoord : GroupSetting.LocalVolumeCoords) {
					FIntVector NodeCoord = BaseCoord + LocalVolumeCoord;
					const UFlowAbstractNode* GroupNode = InGraphQuery.GetNodeObjectAtCoord(NodeCoord);
					NewGroup.GroupNodes.Add(GroupNode->NodeId);
				}
				for (const FIntVector& SurfCoord : GroupSetting.LocalSurfaceCoords) {
					FIntVector NodeCoord = BaseCoord + SurfCoord;
					const UFlowAbstractNode* GroupNode = InGraphQuery.GetNodeObjectAtCoord(NodeCoord);
					NewGroup.GroupEdgeNodes.Add(GroupNode->NodeId);
				}

				// Create the group user data
				const TSharedPtr<FSGFNodeGroupUserData> UserData = MakeShareable(new FSGFNodeGroupUserData);
				UserData->ModuleAssembly = GroupSetting.ModuleAssembly;
				NewGroup.UserData = UserData;
			}
		}
	}
}

int32 FSnapFlowAGNodeGroupGenerator::GetMinNodeGroupSize() const {
	if (GroupSettings.Num() == 0) return 1;

	int32 MinGroupSize = MAX_int32;
	for (const FSnapFlowAGNodeGroupSetting& GroupSetting : GroupSettings) {
		const int32 GroupSize = GroupSetting.GroupSize.X * GroupSetting.GroupSize.Y * GroupSetting.GroupSize.Z;
		MinGroupSize = FMath::Min(MinGroupSize, GroupSize);
	}
	return MinGroupSize;
}

///////////////////////////////////////////// FSnapFlowAGFastNodeGroupGenerator /////////////////////////////////////////////

FSnapFlowAGIgnoreDoorCategoryNodeGroupGenerator::FSnapFlowAGIgnoreDoorCategoryNodeGroupGenerator(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase, TSharedPtr<FSGFNodeCategorySelector> InNodeCategorySelector)
	: ModuleDB(InModuleDatabase)
	, NodeCategorySelector(InNodeCategorySelector)
{
	if (InModuleDatabase.IsValid()) {
		TMap<FIntVector, float> GroupWeights;
		{	
			TMap<FIntVector, int32> GroupCounts;
			for (const FSnapGridFlowModuleDatabaseItem& Module : InModuleDatabase->Modules) {
				TArray<FIntVector> ChunkSizes;
				ChunkSizes.Add(Module.NumChunks);
				if (Module.bAllowRotation) {  
					ChunkSizes.Add(FIntVector(Module.NumChunks.Y, Module.NumChunks.X, Module.NumChunks.Z));
				}

				for (const FIntVector& ChunkSize : ChunkSizes) {
					float& Weight = GroupWeights.FindOrAdd(ChunkSize);
					Weight += Module.SelectionWeight;

					int32& Count = GroupCounts.FindOrAdd(ChunkSize);
					Count++;
				} 
			}

			// Average out the weights
			for (auto& Entry : GroupWeights) {
				const FIntVector& Key = Entry.Key;
				const int32 Count = GroupCounts[Key];
				Entry.Value /= Count;
			}
		}

		for (auto& Entry : GroupWeights) {
			FSnapFlowAGNodeGroupSetting& Setting = GroupSettings.AddDefaulted_GetRef();
			Setting.Weight = Entry.Value;
			Setting.GroupSize = Entry.Key;
		}
	}
	else {
		FSnapFlowAGNodeGroupSetting& DefaultSetting = GroupSettings.AddDefaulted_GetRef();
		DefaultSetting.Weight = 1;
		DefaultSetting.GroupSize = FIntVector(1, 1, 1);
	}
}


void FSnapFlowAGIgnoreDoorCategoryNodeGroupGenerator::Generate(const FFlowAbstractGraphQuery& InGraphQuery, const UFlowAbstractNode* InCurrentNode,
				int32 InPathIndex, int32 InPathLength, const FRandomStream& InRandom, const TSet<FGuid>& InVisited,
				TArray<FFlowAGPathNodeGroup>& OutGroups) const
{
	const UFlowAbstractNode* Node = InCurrentNode;
	if (!Node) {
		return;
	}

	if (GroupSettings.Num() == 0) {
		const FNullFlowAGNodeGroupGenerator NullGenerator;
		NullGenerator.Generate(InGraphQuery, Node, InPathIndex, InPathLength, InRandom, InVisited, OutGroups);
		return;
	}

	TArray<FSnapFlowAGNodeGroupSetting> WeightedGroupSettings = GroupSettings;
	for (FSnapFlowAGNodeGroupSetting& Settings : WeightedGroupSettings) {
		// Add randomness to similar weighted objects 
		Settings.Weight += InRandom.FRand() * 0.0001f;
	}

	static SGFLib::FSimpleLocalCoordBuilder CoordBuilder;
	
	for (const FSnapFlowAGNodeGroupSetting& GroupSetting : WeightedGroupSettings) {
		TArray<FIntVector> LocalSurfaceCoords;
		TArray<FIntVector> LocalVolumeCoords;
		CoordBuilder.GetCoords(GroupSetting.GroupSize, LocalVolumeCoords, LocalSurfaceCoords);
		
		for (const FIntVector& LocalSurfaceCoord : LocalSurfaceCoords) {
			bool bValid = true;
			FIntVector BaseCoord = FMathUtils::ToIntVector(Node->Coord, true) - LocalSurfaceCoord;
			for (const FIntVector& LocalVolumeCoord : LocalVolumeCoords) {
				FIntVector GroupNodeCoord = BaseCoord + LocalVolumeCoord;
				

				const UFlowAbstractNode* TestNode = InGraphQuery.GetNodeObjectAtCoord(GroupNodeCoord);
 				if (!TestNode || InVisited.Contains(TestNode->NodeId) || TestNode->bActive) {
					bValid = false;
					break;
				}
			}

			if (bValid) {
				// Add this group
				FFlowAGPathNodeGroup& NewGroup = OutGroups.AddDefaulted_GetRef();
				NewGroup.bIsGroup = true;
				NewGroup.Weight = GroupSetting.Weight;
				for (const FIntVector& LocalVolumeCoord : LocalVolumeCoords) {
					FIntVector NodeCoord = BaseCoord + LocalVolumeCoord;
					const UFlowAbstractNode* GroupNode = InGraphQuery.GetNodeObjectAtCoord(NodeCoord);
					NewGroup.GroupNodes.Add(GroupNode->NodeId);
				}
				for (const FIntVector& SurfCoord : LocalSurfaceCoords) {
					FIntVector NodeCoord = BaseCoord + SurfCoord;
					const UFlowAbstractNode* GroupNode = InGraphQuery.GetNodeObjectAtCoord(NodeCoord);
					NewGroup.GroupEdgeNodes.Add(GroupNode->NodeId);
				}
			}
		}
	}
}

int32 FSnapFlowAGIgnoreDoorCategoryNodeGroupGenerator::GetMinNodeGroupSize() const {
	if (GroupSettings.Num() == 0) return 1;

	int32 MinGroupSize = MAX_int32;
	for (const FSnapFlowAGNodeGroupSetting& GroupSetting : GroupSettings) {
		const int32 GroupSize = GroupSetting.GroupSize.X * GroupSetting.GroupSize.Y * GroupSetting.GroupSize.Z;
		MinGroupSize = FMath::Min(MinGroupSize, GroupSize);
	}
	return MinGroupSize;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TArray<FName> FSGFNodeCategorySelector::GetCategoriesAtNode(int32 PathIndex, int32 PathLength) const {
	TArray<FName> TargetCategories = ModuleCategories;

	if (ModuleCategoryOverrideMethod == ESnapFlowAGTaskModuleCategoryOverrideMethod::Blueprint) {
		// Check if a selection override bp wants to override the category list
		for (USnapFlowNodeCategorySelectionOverride* SelectionOverride : CategoryOverrideLogic) {
			TArray<FName> CategoryOverrides;
			if (SelectionOverride && SelectionOverride->TryOverrideCategories(PathIndex, PathLength, ModuleCategories, CategoryOverrides)) {
				TargetCategories = CategoryOverrides;
				break;
			}
		}
	}
	else if (ModuleCategoryOverrideMethod == ESnapFlowAGTaskModuleCategoryOverrideMethod::StartEnd) {
		if (PathIndex == 0 && StartNodeCategoryOverride.Num() > 0) {
			TargetCategories = StartNodeCategoryOverride;
		}
		else if (PathIndex == PathLength - 1 && EndNodeCategoryOverride.Num() > 0) {
			TargetCategories = EndNodeCategoryOverride;
		}
	}

	return TargetCategories;
}


void FSGFNodeGroupUserData::CopyFrom(const USGFNodeGroupUserData* Other) {
	ModuleAssembly = Other->ModuleAssembly;
}

void USGFNodeGroupUserData::CopyFrom(const FSGFNodeGroupUserData& Other) {
	ModuleAssembly = Other.ModuleAssembly;
}

//////////////////////////////////////// Snap Abstract Graph Constraint System //////////////////////////////////////////////
FSnapGridFlowAbstractGraphConstraints::FSnapGridFlowAbstractGraphConstraints(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase,
		TSharedPtr<FSGFNodeCategorySelector> InNodeCategorySelector, bool bInSupportsDoorCategories)
	: ModuleDatabase(InModuleDatabase)
	, NodeCategorySelector(InNodeCategorySelector)
	, bSupportsDoorCategories(bInSupportsDoorCategories)
{
}

bool FSnapGridFlowAbstractGraphConstraints::IsValid(const FFlowAbstractGraphQuery& InGraphQuery, const FFlowAGPathNodeGroup& Group,
                                                    const TArray<FFAGConstraintsLink>& IncomingNodes, const TArray<FName>& InAllowedCategories) const {
	if (Group.GroupEdgeNodes.Num() == 0 || Group.GroupNodes.Num() == 0 || !ModuleDatabase.IsValid()) return false;

	SCOPE_CYCLE_COUNTER(STAT_SGFIsValid);
	
	// Build the input node assembly
	FSGFModuleAssembly Assembly;
	FSGFModuleAssemblyBuilder::Build(InGraphQuery, Group, IncomingNodes, Assembly);

	if (bSupportsDoorCategories) {
		if (!Group.UserData.IsValid()) {
			UE_LOG(LogSGFSupport, Error, TEXT("Invalid SGF Group state"));
			return false;
		}

		const TSharedPtr<FSGFNodeGroupUserData> SgfUserData = StaticCastSharedPtr<FSGFNodeGroupUserData>(Group.UserData);
		const FSGFModuleAssembly& RegisteredAssembly = SgfUserData->ModuleAssembly;
	
		TArray<FSGFModuleAssemblySideCell> DoorIndices;
		return RegisteredAssembly.CanFit(Assembly, DoorIndices);
	}
	else {
		if (ModuleDatabase.IsValid()) {
			for (const FSnapGridFlowModuleDatabaseItem& Module : ModuleDatabase->Modules) {
				if (InAllowedCategories.Contains(Module.Category)) {
					const int32 NumRotatedAssemblies = Module.RotatedAssemblies.Num();
					for (int StepIdx = 0; StepIdx < NumRotatedAssemblies; StepIdx++) {
						const FSGFModuleAssembly& RegisteredAssembly = Module.RotatedAssemblies[StepIdx];
						TArray<FSGFModuleAssemblySideCell> DoorIndices;

						if (RegisteredAssembly.CanFit(Assembly, DoorIndices)) {
							return true;
						}
					}
				}
			}
		}
		return false;
	}
}

bool FSnapGridFlowAbstractGraphConstraints::IsValid(const FFlowAbstractGraphQuery& InGraphQuery, const UFlowAbstractNode* Node,
                                                    const TArray<const UFlowAbstractNode*>& IncomingNodes) {
	
	UFlowAbstractGraphBase* Graph = InGraphQuery.GetGraph<UFlowAbstractGraphBase>();
	if (!Graph) return false;
	check(Node && Node->PathIndex != INDEX_NONE);


	TSet<const UFlowAbstractNode*> AllIncomingNodes(IncomingNodes);
	{
		// TODO: Check the direction of incoming node (ignore?).
		// TODO: Ignore unconnected nodes
		// TODO: Check and REMOVE THIS
		for (const FGuid& IncomingNode : Graph->GetIncomingNodes(Node->NodeId)) {
			AllIncomingNodes.Add(InGraphQuery.GetNode(IncomingNode));
		}
	}

	FFlowAGPathNodeGroup Group;
	TArray<FFAGConstraintsLink> ConstraintLinks;
	FSGFNodeGroupGenLib::BuildNodeGroup(InGraphQuery, Node, AllIncomingNodes.Array(), Group, ConstraintLinks);

	TArray<FName> AllowedCategories;
	UFANodeSnapDomainData* NodeSnapData = Node->FindDomainData<UFANodeSnapDomainData>();
	if (!NodeSnapData) {
		return false;
	}

	return IsValid(InGraphQuery, Group, ConstraintLinks, NodeSnapData->ModuleCategories); 
}

bool FSnapGridFlowAbstractGraphConstraints::IsValid(const FFlowAbstractGraphQuery& InGraphQuery, const FFlowAGPathNodeGroup& Group, int32 PathIndex, int32 PathLength,
                                                    const TArray<FFAGConstraintsLink>& IncomingNodes) {
	const TArray<FName> AllowedCategories = NodeCategorySelector->GetCategoriesAtNode(PathIndex, PathLength);
	return IsValid(InGraphQuery, Group, IncomingNodes, AllowedCategories);
}

void FSGFNodeGroupGenLib::BuildNodeGroup(const FFlowAbstractGraphQuery& InGraphQuery,
	                   const UFlowAbstractNode* InNode, const TArray<const UFlowAbstractNode*>& InIncomingNodes,
	                   FFlowAGPathNodeGroup& OutGroup, TArray<FFAGConstraintsLink>& OutConstraintLinks) {
	UFlowAbstractGraphBase* Graph = InGraphQuery.GetGraph<UFlowAbstractGraphBase>();
	if (!Graph) return;

	FIntVector MinCoord, MaxCoord;
	OutGroup = FFlowAGPathNodeGroup();
	if (InNode->MergedCompositeNodes.Num() <= 1) {
		OutGroup.bIsGroup = false;
		OutGroup.GroupNodes.Add(InNode->NodeId);
		OutGroup.GroupEdgeNodes.Add(InNode->NodeId);
		MinCoord = MaxCoord = FMathUtils::ToIntVector(InNode->Coord, true);
	}
	else {
		OutGroup.bIsGroup = true;
		FVector MinCoordF = InNode->MergedCompositeNodes[0]->Coord;
		FVector MaxCoordF = MinCoordF;

		for (UFlowAbstractNode* const SubNode : InNode->MergedCompositeNodes) {
			MinCoordF = MinCoordF.ComponentMin(SubNode->Coord);
			MaxCoordF = MaxCoordF.ComponentMax(SubNode->Coord);
			OutGroup.GroupNodes.Add(SubNode->NodeId);
		}
		MinCoord = FMathUtils::ToIntVector(MinCoordF, true);
		MaxCoord = FMathUtils::ToIntVector(MaxCoordF, true);

		for (UFlowAbstractNode* const SubNode : InNode->MergedCompositeNodes) {
			const FIntVector Coord = FMathUtils::ToIntVector(SubNode->Coord, true);
			if (Coord.X == MinCoord.X || Coord.Y == MinCoord.Y || Coord.Z == MinCoord.Z ||
				Coord.X == MaxCoord.X || Coord.Y == MaxCoord.Y || Coord.Z == MaxCoord.Z) {
				OutGroup.GroupEdgeNodes.Add(SubNode->NodeId);
			}
		}
	}

	const USGFNodeGroupUserData* NodeUserData = InNode->FindDomainData<USGFNodeGroupUserData>();
	if (NodeUserData) {
		TSharedPtr<FSGFNodeGroupUserData> UserData = MakeShareable(new FSGFNodeGroupUserData);
		UserData->CopyFrom(NodeUserData);
		OutGroup.UserData = UserData;
	}
	else {
		//UE_LOG(LogSGFSupport, Error, TEXT("Invalid SGF Node userdata state"));
	}
	
	for (UFlowAbstractLink* Link : Graph->GraphLinks) {
		if (Link->Type == EFlowAbstractLinkType::Unconnected) continue;

		FGuid Source = Link->SourceSubNode.IsValid() ? Link->SourceSubNode : Link->Source;
		FGuid Destination = Link->DestinationSubNode.IsValid() ? Link->DestinationSubNode : Link->Destination;

		const bool bHostsSource = OutGroup.GroupNodes.Contains(Source);
		const bool bHostsDest = OutGroup.GroupNodes.Contains(Destination);
		if (!bHostsSource && !bHostsDest) continue;
		if (bHostsSource && bHostsDest) continue;

		if (bHostsSource) {
			if (OutGroup.GroupEdgeNodes.Contains(Source)) {
				UFlowAbstractNode* SourceNode = InGraphQuery.GetNode(Source);
				if (!SourceNode) SourceNode = InGraphQuery.GetSubNode(Source);
				UFlowAbstractNode* DestinationNode = InGraphQuery.GetNode(Destination);
				if (!DestinationNode) DestinationNode = InGraphQuery.GetSubNode(Destination);
				if (SourceNode && DestinationNode) {
					OutConstraintLinks.Add(FFAGConstraintsLink(SourceNode, DestinationNode));
				}
			}
		}
		else if (bHostsDest) {
			if (OutGroup.GroupEdgeNodes.Contains(Destination)) {
				UFlowAbstractNode* SourceNode = InGraphQuery.GetNode(Source);
				if (!SourceNode) SourceNode = InGraphQuery.GetSubNode(Source);
				UFlowAbstractNode* DestinationNode = InGraphQuery.GetNode(Destination);
				if (!DestinationNode) DestinationNode = InGraphQuery.GetSubNode(Destination);
				if (SourceNode && DestinationNode) {
					OutConstraintLinks.Add(FFAGConstraintsLink(DestinationNode, SourceNode));
				}
			}
		}
	}

	TMap<FIntVector, UFlowAbstractNode*> NodeByCoords;
	for (UFlowAbstractNode* GraphNode : Graph->GraphNodes) {
		if (GraphNode->MergedCompositeNodes.Num() > 0) {
			for (UFlowAbstractNode* SubNode : GraphNode->MergedCompositeNodes) {
				FIntVector Coord = FMathUtils::ToIntVector(SubNode->Coord, true);
				UFlowAbstractNode*& NodeRef = NodeByCoords.FindOrAdd(Coord);
				NodeRef = SubNode;
			}
		}
		else {
			FIntVector Coord = FMathUtils::ToIntVector(GraphNode->Coord, true);
			UFlowAbstractNode*& NodeRef = NodeByCoords.FindOrAdd(Coord);
			NodeRef = GraphNode;
		}
	}

	for (const UFlowAbstractNode* const IncomingNode : InIncomingNodes) {
		if (!IncomingNode) continue;
		FIntVector InnerCoord = FMathUtils::ToIntVector(IncomingNode->Coord, true);
		InnerCoord.X = FMath::Clamp(InnerCoord.X, MinCoord.X, MaxCoord.X);
		InnerCoord.Y = FMath::Clamp(InnerCoord.Y, MinCoord.Y, MaxCoord.Y);
		InnerCoord.Z = FMath::Clamp(InnerCoord.Z, MinCoord.Z, MaxCoord.Z);
		UFlowAbstractNode** InnerNodePtr = NodeByCoords.Find(InnerCoord);
		if (InnerNodePtr) {
			UFlowAbstractNode* InnerNode = *InnerNodePtr;
			OutConstraintLinks.Add(FFAGConstraintsLink(InnerNode, IncomingNode));
		}
	}
}

