//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleResolver.h"

#include "Builders/SnapMap/SnapMapDungeonBuilder.h"
#include "Core/Utils/MathUtils.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraphQuery.h"
#include "Frameworks/Flow/Domains/Snap/SnapFlowAbstractGraphSupport.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowLibrary.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleDatabase.h"
#include "Frameworks/Snap/SnapMap/SnapMapLibrary.h"

DEFINE_LOG_CATEGORY_STATIC(LogSGFResolver, Log, All);

namespace {
    class FModuleItemFitnessCalculator {
    public:
        FModuleItemFitnessCalculator(const TMap<TSoftObjectPtr<UPlaceableMarkerAsset>, int32>& InModuleMarkers) {
            for (auto& Entry : InModuleMarkers) {
                TSoftObjectPtr<UPlaceableMarkerAsset> MarkerAsset = Entry.Key;
                UPlaceableMarkerAsset const* MarkerAssetPtr = MarkerAsset.LoadSynchronous();
                if (!MarkerAssetPtr) continue;
                
                const int32 Count = Entry.Value;
                ModuleMarkers.Add(MarkerAssetPtr, Count);
            }
        }

        int32 Calculate(const TArray<FString>& InMarkerNames) const {
            TMap<UPlaceableMarkerAsset const*, int32> AvailableMarkers = ModuleMarkers;
            return Solve(InMarkerNames, AvailableMarkers);
        }

    private:
        static int32 Solve(const TArray<FString>& InMarkerNames, TMap<UPlaceableMarkerAsset const*, int32>& AvailableMarkers) {
            int32 NumFailed;
            if (AvailableMarkers.Num() > 0) {
                NumFailed = SolveImpl(InMarkerNames, 0, AvailableMarkers);
                check(NumFailed >= 0);
            }
            else {
                NumFailed = InMarkerNames.Num();
            }

            const int32 FAIL_WEIGHT = 1000000;
            return NumFailed * FAIL_WEIGHT;
        }

        // Returns the no. of items failed in the processed sub tree
        static int32 SolveImpl(const TArray<FString>& InMarkerNames, int32 Index, TMap<UPlaceableMarkerAsset const*, int32>& AvailableMarkers) {
            if (Index == InMarkerNames.Num()) {
                return 0;
            }
            
            check (Index >= 0 || Index < InMarkerNames.Num());

            int32 BestFrameFailCount = InMarkerNames.Num();
            const FString& MarkerName = InMarkerNames[Index];
            for (auto& Entry : AvailableMarkers) {
                UPlaceableMarkerAsset const* AvailableMarkerAsset = Entry.Key;
                int32& Count = Entry.Value;
                const bool bCanAttachHere =  (Count > 0 && AvailableMarkerAsset->MarkerNames.Contains(MarkerName));
                int32 FrameFailCount = bCanAttachHere ? 0 : 1;
                if (bCanAttachHere) {
                    Count--;
                }
                FrameFailCount += SolveImpl(InMarkerNames, Index + 1, AvailableMarkers);
                if (bCanAttachHere) {
                    Count++;
                }

                if (FrameFailCount < BestFrameFailCount) {
                    BestFrameFailCount = FrameFailCount;
                }
                
                if (AvailableMarkerAsset->MarkerNames.Num() == 1 && AvailableMarkerAsset->MarkerNames[0] == MarkerName) {
                    // Faster bailout
                    break;
                }
            } 
            
            return BestFrameFailCount;
        }
        
    private:
        TMap<UPlaceableMarkerAsset const*, int32> ModuleMarkers;
    };
}

bool FSnapGridFlowModuleResolver::Resolve(UFlowAbstractGraphBase* InGraph, TArray<SnapLib::FModuleNodePtr>& OutModuleNodes) const {
    if (!InGraph || !ModuleDatabase.IsValid()) return false;
    
    FSGFModuleResolveState ResolveState(InGraph, Settings.Seed);
    for (UFlowAbstractNode* GraphNode : InGraph->GraphNodes) {
        if (GraphNode->bActive) {
            FSGFResolveNodeGroupData& NodeGroupData = ResolveState.NodeGroups.FindOrAdd(GraphNode);
            FSGFNodeGroupGenLib::BuildNodeGroup(ResolveState.GraphQuery, GraphNode, {}, NodeGroupData.Group, NodeGroupData.ConstraintLinks);
        }
    }
    if (!ResolveNodes(ResolveState)) {
        return false;
    }

    for (auto& Entry : ResolveState.ActiveModuleDoorIndices) {
        const FGuid& ModuleId = Entry.Key;
        TArray<FSGFModuleAssemblySideCell>& DoorSideCells = Entry.Value;
        for (FSGFModuleAssemblySideCell& DoorSideCell : DoorSideCells) {
            for (UFlowAbstractLink const* GraphLink : InGraph->GraphLinks) {
                if (GraphLink->Type == EFlowAbstractLinkType::Unconnected) continue;
                if ((GraphLink->Source == DoorSideCell.NodeId || GraphLink->SourceSubNode == DoorSideCell.NodeId)
                        && (GraphLink->Destination == DoorSideCell.LinkedNodeId || GraphLink->DestinationSubNode == DoorSideCell.LinkedNodeId)) {
                    // Outgoing Node
                    DoorSideCell.LinkId = GraphLink->LinkId;
                }
                else if ((GraphLink->Source == DoorSideCell.LinkedNodeId || GraphLink->SourceSubNode == DoorSideCell.LinkedNodeId)
                        && (GraphLink->Destination == DoorSideCell.NodeId || GraphLink->DestinationSubNode == DoorSideCell.NodeId)) {
                    // Incoming Node
                    DoorSideCell.LinkId = GraphLink->LinkId;
                }
            }
        }
    }
                    
    // Link the module nodes together
    for (UFlowAbstractLink const* GraphLink : InGraph->GraphLinks) {
        if (!GraphLink || GraphLink->Type == EFlowAbstractLinkType::Unconnected) continue;
        
        FGuid SourceId = GraphLink->Source;
        FGuid DestId = GraphLink->Destination;

        FSGFModuleAssemblySideCell const* SrcCell = nullptr;
        FSGFModuleAssemblySideCell const* DstCell = nullptr;
        {
            const TArray<FSGFModuleAssemblySideCell>& SourceDoorCells = ResolveState.ActiveModuleDoorIndices.FindOrAdd(SourceId);
            for (const FSGFModuleAssemblySideCell& SourceDoorCell : SourceDoorCells) {
                if (SourceDoorCell.LinkId == GraphLink->LinkId) {
                    SrcCell = &SourceDoorCell;
                    break;
                }
            } 
            const TArray<FSGFModuleAssemblySideCell>& DestDoorCells = ResolveState.ActiveModuleDoorIndices.FindOrAdd(DestId);
            for (const FSGFModuleAssemblySideCell& DestDoorCell : DestDoorCells) {
                if (DestDoorCell.LinkId == GraphLink->LinkId) {
                    DstCell = &DestDoorCell;
                    break;
                }
            } 
        }

        if (!SrcCell || !DstCell) {
            return false;
        }
        
        if (!ResolveState.ModuleNodesById.Contains(SourceId) || !ResolveState.ModuleNodesById.Contains(DestId)) {
            return false;
        }

        SnapLib::FModuleNodePtr SrcModule = ResolveState.ModuleNodesById[SourceId];
        SnapLib::FModuleNodePtr DstModule = ResolveState.ModuleNodesById[DestId];
        SnapLib::FModuleDoorPtr SrcDoor = SrcModule->Doors[SrcCell->ConnectionIdx];
        SnapLib::FModuleDoorPtr DstDoor = DstModule->Doors[DstCell->ConnectionIdx];

        SrcDoor->ConnectedDoor = DstDoor;
        DstDoor->ConnectedDoor = SrcDoor;

        SrcModule->Outgoing.Add(SrcDoor);
        DstModule->Incoming = DstDoor;
    } 
    

    OutModuleNodes.Reset();
    ResolveState.ModuleNodesById.GenerateValueArray(OutModuleNodes);
    return true;
}

bool FSnapGridFlowModuleResolver::ResolveNodes(FSGFModuleResolveState& InResolveState) const {
    return ResolveNodes_Recursive(InResolveState);
    //return ResolveNodes_Linear(InResolveState);
}

bool FSnapGridFlowModuleResolver::ResolveNodes_Recursive(FSGFModuleResolveState& InResolveState) const {
    const FFlowAbstractGraphQuery& GraphQuery = InResolveState.GraphQuery;
    UFlowAbstractGraphBase* Graph = GraphQuery.GetGraph<UFlowAbstractGraphBase>();
    UFlowAbstractNode* StartNode = nullptr;
    for (UFlowAbstractNode* Node : Graph->GraphNodes) {
        if (Node && Node->bActive) {
            for (UFlowGraphItem* NodeItem : Node->NodeItems) {
                if (!NodeItem) continue;
                if (NodeItem->ItemType == EFlowGraphItemType::Entrance) {
                    StartNode = Node;
                    break;
                }
            }
            if (StartNode) {
                break;
            }
        }
    }

    if (!StartNode) {
        return false;
    }

    const TSet<UFlowAbstractNode*> Visited;
    return ResolveNode_Recursive(InResolveState, StartNode, 0, Visited);
}

bool FSnapGridFlowModuleResolver::ResolveNode_Recursive(FSGFModuleResolveState& InResolveState, UFlowAbstractNode* InNode, int32 InDepth, TSet<UFlowAbstractNode*> InVisited) const {
    if (InResolveState.FrameIndex > Settings.MaxResolveFrames) {
        return false;
    }
    InResolveState.FrameIndex++;

    if (InVisited.Contains(InNode)) {
        // Already resolved
        return true;
    }

    FSGFResolveNodeGroupData NodeGroupData = InResolveState.NodeGroups[InNode];
    
    FSGFModuleAssembly Assembly;
    FSGFModuleAssemblyBuilder::Build(InResolveState.GraphQuery, NodeGroupData.Group, NodeGroupData.ConstraintLinks, Assembly);
    
    TArray<FModuleFitCandidate> Candidates;
    GetCandidates(InResolveState, InNode, InDepth, Assembly, Candidates);
    if (Candidates.Num() == 0) {
        return false;
    }

    InVisited.Add(InNode);
    bool bSuccess = false;
    for (const FModuleFitCandidate& Candidate : Candidates)
    {
        // Find the best fit
        RegisterNodeModule(InNode, InResolveState, Candidate);

        // Recursively resolve all outgoing nodes
        TArray<UFlowAbstractNode*> OutgoingNodes;
        {
            TArray<FGuid> OutgoingNodeIds = InResolveState.GraphQuery.GetOutgoingNodes(InNode->NodeId);
            for (const FGuid& OutgoingNodeId : OutgoingNodeIds)
            {
                UFlowAbstractNode* OutgoingNode = InResolveState.GraphQuery.GetNode(OutgoingNodeId);
                if (OutgoingNode && OutgoingNode->bActive)
                {
                    OutgoingNodes.Add(OutgoingNode);
                }
            }
        }

        // Save the depth of this module so we don't repeat them next to each other
        PushModuleLastUsedDepth(InResolveState, Candidate.ModuleItem, InDepth);
        
        bool bAllBranchesSuccessful = true;
        // Resolve the child branches
        for (UFlowAbstractNode* OutgoingNode : OutgoingNodes)
        {
            if (!ResolveNode_Recursive(InResolveState, OutgoingNode, InDepth + 1, InVisited))
            {
                bAllBranchesSuccessful = false;
                break;
            }
        }

        PopModuleLastUsedDepth(InResolveState, Candidate.ModuleItem);
        
        if (bAllBranchesSuccessful)
        {
            bSuccess = true;
            break;
        }

        DeregisterNodeModule(InNode, InResolveState);
    }
    
    // Cannot find a solution in this path. Backtrack and find another path
    InVisited.Remove(InNode);

    return bSuccess;
    
}

void FSnapGridFlowModuleResolver::GetCandidates(FSGFModuleResolveState& InResolveState, UFlowAbstractNode* InNode, int32 InDepth, const FSGFModuleAssembly& InAssembly, TArray<FModuleFitCandidate>& OutCandidates) const {
    TSet<FName> CategoryNames;
    const FRandomStream& Random = InResolveState.Random;
    
    UFANodeSnapDomainData* NodeSnapData = InNode->FindDomainData<UFANodeSnapDomainData>();
    if (NodeSnapData) {
        CategoryNames = TSet<FName>(NodeSnapData->ModuleCategories);
    }
    else {
        UE_LOG(LogSGFResolver, Error, TEXT("Snap Domain data missing in the abstract graph node"));
    }

    TArray<SnapLib::IModuleDatabaseItemPtr> ModuleItems;
    for (const FName& CategoryName : CategoryNames) {
        ModuleItems.Append(ModuleDatabase->GetCategoryModules(CategoryName));
    }

    TArray<FString> DesiredNodeMarkers;
    for (UFlowGraphItem* NodeItem : InNode->NodeItems) {
        if (!NodeItem) continue;
        FString MarkerName = NodeItem->MarkerName.TrimStartAndEnd();
        if (MarkerName.Len() > 0) {
            DesiredNodeMarkers.Add(MarkerName);
        }
    }

    bool bChooseModulesWithMinDoors = Random.FRand() < Settings.ModulesWithMinimumDoorsProbability;

    TArray<int32> ModuleIndices = FMathUtils::GetShuffledIndices(ModuleItems.Num(), Random);
    for (int32 ModuleIdx : ModuleIndices) {
        TSharedPtr<FSnapGridFlowGraphModDBItemImpl> ModuleItem = StaticCastSharedPtr<FSnapGridFlowGraphModDBItemImpl>(ModuleItems[ModuleIdx]);
        if (!ModuleItem.IsValid()) continue;

        const FSnapGridFlowModuleDatabaseItem ModuleInfo = ModuleItem->GetItem();
        FModuleItemFitnessCalculator ItemFitnessCalculator(ModuleInfo.AvailableMarkers);
        int32 ItemFitness = ItemFitnessCalculator.Calculate(DesiredNodeMarkers);
        const float ModuleEntryWeight = FMath::Clamp(ModuleInfo.SelectionWeight, 0.0f, 1.0f);
    
        const int32 NumAssemblies = ModuleInfo.RotatedAssemblies.Num();
        TArray<int32> ShuffledAsmIndices = FMathUtils::GetShuffledIndices(NumAssemblies, Random);
        for (int32 AsmIdx : ShuffledAsmIndices) {
            const FSGFModuleAssembly& ModuleAssembly = ModuleInfo.RotatedAssemblies[AsmIdx];
            TArray<FSGFModuleAssemblySideCell> DoorIndices;
            if (ModuleAssembly.CanFit(InAssembly, DoorIndices)) {
                bool bDoorCategoriesValid = true;

                for (const FSGFModuleAssemblySideCell& DoorInfo : DoorIndices) {
                    if (!ModuleInfo.Connections.IsValidIndex(DoorInfo.ConnectionIdx)) {
                        continue;
                    }
                    
                    const FSnapGridFlowModuleDatabaseConnectionInfo& NodeConnection = ModuleInfo.Connections[DoorInfo.ConnectionIdx];
                    FGuid OtherNodeId;
                    if (!InResolveState.GraphQuery.GetParentNode(DoorInfo.LinkedNodeId, OtherNodeId)) {
                        OtherNodeId = DoorInfo.LinkedNodeId;
                    }

                    if (InResolveState.ActiveModuleDoorIndices.Contains(OtherNodeId)) {
                        const TArray<FSGFModuleAssemblySideCell>& OtherNodeDoors = InResolveState.ActiveModuleDoorIndices[OtherNodeId];
                        for (const FSGFModuleAssemblySideCell& OtherNodeDoor : OtherNodeDoors) {
                            FGuid OtherDoorLinkedNodeId;
                            if (!InResolveState.GraphQuery.GetParentNode(OtherNodeDoor.LinkedNodeId, OtherDoorLinkedNodeId)) {
                                OtherDoorLinkedNodeId = OtherNodeDoor.LinkedNodeId;
                            }

                            if (OtherDoorLinkedNodeId == InNode->NodeId) {
                                TSharedPtr<SnapLib::FModuleNode> OtherNodeModule = InResolveState.ModuleNodesById[OtherNodeId];
                                if (!OtherNodeModule->Doors.IsValidIndex(OtherNodeDoor.ConnectionIdx) || !OtherNodeModule->Doors[OtherNodeDoor.ConnectionIdx].IsValid()) {
                                    bDoorCategoriesValid = false;
                                    break;
                                }

                                USnapConnectionInfo* OtherNodeConnectionInfo = OtherNodeModule->Doors[OtherNodeDoor.ConnectionIdx]->ConnectionInfo;
                                if (!NodeConnection.ConnectionInfo || !OtherNodeConnectionInfo
                                        || NodeConnection.ConnectionInfo->ConnectionCategory != OtherNodeConnectionInfo->ConnectionCategory) {
                                    bDoorCategoriesValid = false;
                                    break;
                                }
                            }
                        } 
                    }

                    if (!bDoorCategoriesValid) {
                        break;
                    }
                } 
                
                if (bDoorCategoriesValid) {
                    FModuleFitCandidate& Candidate = OutCandidates.AddDefaulted_GetRef();
                    Candidate.ModuleItem = ModuleItem;
                    Candidate.ModuleRotation = FQuat(FVector::UpVector, AsmIdx * 90);
                    Candidate.AssemblyIndex = AsmIdx;
                    Candidate.DoorIndices = DoorIndices;

                    Candidate.ItemFitness = ItemFitness;
                    Candidate.ConnectionWeight = bChooseModulesWithMinDoors ? ModuleInfo.Connections.Num() : 0;
                    Candidate.ModuleWeight = ModuleItem->GetItem().SelectionWeight;
                    Candidate.ModuleLastUsedDepth = GetModuleLastUsedDepth(InResolveState, Candidate.ModuleItem, InDepth, Settings.NonRepeatingRooms);
                }
            }
        }
    }

    OutCandidates.Sort([&Random](const FModuleFitCandidate& A, const FModuleFitCandidate& B) -> bool {
        if (A.ItemFitness == B.ItemFitness) {
            if (FMath::IsNearlyEqual(A.ModuleWeight, B.ModuleWeight)) {
                if (A.ConnectionWeight == B.ConnectionWeight) {
                    if (A.ModuleLastUsedDepth == B.ModuleLastUsedDepth) {
                        return Random.FRand() < 0.5f;
                    }
                    else {
                        return A.ModuleLastUsedDepth > B.ModuleLastUsedDepth;
                    }
                }
                else {
                    return A.ConnectionWeight < B.ConnectionWeight;
                }
            }
            else {
                return A.ModuleWeight > B.ModuleWeight;
            }
        }
        else {
            return A.ItemFitness < B.ItemFitness;
        }
    });
}

void FSnapGridFlowModuleResolver::RegisterNodeModule(UFlowAbstractNode* InNode, FSGFModuleResolveState& InResolveState, const FModuleFitCandidate& InCandidate) const {
    const SnapLib::FModuleNodePtr ModuleNode = InCandidate.ModuleItem->CreateModuleNode(InNode->NodeId);
    check(ModuleDatabase.IsValid());
    const FGuid NodeId = InNode->NodeId;
    const float Angle = PI * 0.5f * InCandidate.AssemblyIndex;
    const FQuat ModuleRotation = FQuat(FVector::UpVector, Angle);
    const FSnapGridFlowModuleDatabaseItem& ModuleDBItem = InCandidate.ModuleItem->GetItem();
    
    FVector LocalCenter = ModuleDBItem.ModuleBounds.GetCenter();
    LocalCenter = ModuleRotation.RotateVector(LocalCenter);
    const FVector DesiredCenter = InNode->Coord * Settings.ChunkSize;
    const FVector ModuleLocation = DesiredCenter - LocalCenter;
    ModuleNode->WorldTransform = FTransform(ModuleRotation, ModuleLocation) * Settings.BaseTransform;

    // Add the doors
    TArray<FSGFModuleAssemblySideCell>& DoorIndicesRef = InResolveState.ActiveModuleDoorIndices.FindOrAdd(NodeId);
    DoorIndicesRef = InCandidate.DoorIndices;
    
    FIntVector Coord = FMathUtils::ToIntVector(InNode->Coord, true);
    for (UFlowAbstractNode* SubNode : InNode->MergedCompositeNodes) {
        if (!SubNode) continue;
        const FIntVector SubCoord = FMathUtils::ToIntVector(SubNode->Coord, true);
        Coord.X = FMath::Min(Coord.X, SubCoord.X);
        Coord.Y = FMath::Min(Coord.Y, SubCoord.Y);
        Coord.Z = FMath::Min(Coord.Z, SubCoord.Z);
    }
    
    // Register in lookup
    {
        SnapLib::FModuleNodePtr& ModulePtrRef = InResolveState.ModuleNodesById.FindOrAdd(NodeId);
        ModulePtrRef = ModuleNode;
        FIntVector& CoordRef = InResolveState.ModuleCoordsById.FindOrAdd(NodeId);
        CoordRef = Coord;
    }
}

void FSnapGridFlowModuleResolver::DeregisterNodeModule(UFlowAbstractNode* InNode, FSGFModuleResolveState& InResolveState) {
    if (InNode) {
        InResolveState.ActiveModuleDoorIndices.Remove(InNode->NodeId);
        InResolveState.ModuleNodesById.Remove(InNode->NodeId);
    }
}

int32 FSnapGridFlowModuleResolver::GetModuleLastUsedDepth(FSGFModuleResolveState& InResolveState, TSharedPtr<FSnapGridFlowGraphModDBItemImpl> InModuleItem,
        int32 InCurrentDepth, int32 InMaxNonRepeatingDepth) {
    if (!InResolveState.ModuleLastUsedDepth.Contains(InModuleItem) || InResolveState.ModuleLastUsedDepth[InModuleItem].Num() == 0) {
        return MAX_int32;
    }

    const int ModuleDepthFromCurrentNode = InCurrentDepth - InResolveState.ModuleLastUsedDepth[InModuleItem].Top();
    return (ModuleDepthFromCurrentNode < InMaxNonRepeatingDepth + 1) ? ModuleDepthFromCurrentNode : MAX_int32; 
}

void FSnapGridFlowModuleResolver::PushModuleLastUsedDepth(FSGFModuleResolveState& InResolveState, TSharedPtr<FSnapGridFlowGraphModDBItemImpl> InModuleItem, int32 InDepth) {
    TArray<int>& Stack = InResolveState.ModuleLastUsedDepth.FindOrAdd(InModuleItem);
    Stack.Push(InDepth);
}

void FSnapGridFlowModuleResolver::PopModuleLastUsedDepth(FSGFModuleResolveState& InResolveState, TSharedPtr<FSnapGridFlowGraphModDBItemImpl> InModuleItem) {
    TArray<int>& Stack = InResolveState.ModuleLastUsedDepth.FindOrAdd(InModuleItem);
    Stack.Pop();
}


bool FSnapGridFlowModuleResolver::ResolveNodes_Linear(FSGFModuleResolveState& InResolveState) const {
    const FFlowAbstractGraphQuery& GraphQuery = InResolveState.GraphQuery;
    UFlowAbstractGraphBase* Graph = GraphQuery.GetGraph<UFlowAbstractGraphBase>();
    
    for (UFlowAbstractNode* const Node : Graph->GraphNodes) {
        if (!Node->bActive) continue;
        if (!ResolveNode_Linear(InResolveState, Node)) {
            return false;
        }
    }

    return true;
}

bool FSnapGridFlowModuleResolver::ResolveNode_Linear(FSGFModuleResolveState& InResolveState, UFlowAbstractNode* InNode) const {
    FFlowAGPathNodeGroup Group;
    TArray<FFAGConstraintsLink> ConstraintLinks;
    FSGFNodeGroupGenLib::BuildNodeGroup(InResolveState.GraphQuery, InNode, {}, Group, ConstraintLinks);

    const FGuid& NodeId = InNode->NodeId;
    SnapLib::FModuleNodePtr ModuleNode;
        
    // Build the input node assembly
    FSGFModuleAssembly Assembly;
    FSGFModuleAssemblyBuilder::Build(InResolveState.GraphQuery, Group, ConstraintLinks, Assembly);

    TArray<FModuleFitCandidate> Candidates;
    GetCandidates(InResolveState, InNode, 0, Assembly, Candidates);
    if (Candidates.Num() == 0) {
        return false;
    }
        
    RegisterNodeModule(InNode, InResolveState, Candidates[0]);
    return true;
}

