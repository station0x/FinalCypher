//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowLibrary.h"

#include "Core/Utils/MathUtils.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraphQuery.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleBounds.h"

//////////////////////////////////////// Graph Node Impl //////////////////////////////////////////////

DEFINE_LOG_CATEGORY_STATIC(LogSnapGridFlowLib, Log, All);

FSnapGridFlowGraphNode::FSnapGridFlowGraphNode(TWeakObjectPtr<UFlowAbstractGraphBase> InGraph, const FGuid& InNodeID)
    : NodeID(InNodeID)
      , Graph(InGraph) {
}

FName FSnapGridFlowGraphNode::GetCategory() const {
    static const FName Category = "Room";
    return Category;
}

TArray<SnapLib::ISnapGraphNodePtr> FSnapGridFlowGraphNode::GetOutgoingNodes(const FGuid& IncomingNodeId) const {
    TArray<SnapLib::ISnapGraphNodePtr> OutgoingNodes;
    for (UFlowAbstractLink* Link : Graph->GraphLinks) {
        if (Link->Type == EFlowAbstractLinkType::Unconnected) {
            continue;
        }
        if (Link->Source == NodeID && Link->Destination != IncomingNodeId) {
            OutgoingNodes.Add(MakeShareable(new FSnapGridFlowGraphNode(Graph, Link->Destination)));
        }
        else if (Link->Destination == GetNodeID() && Link->Source != IncomingNodeId) {
            OutgoingNodes.Add(MakeShareable(new FSnapGridFlowGraphNode(Graph, Link->Source)));
        }
    }
    return OutgoingNodes;
}

FVector FSnapGridFlowGraphNode::GetNodeCoord() const {
    UFlowAbstractNode* Node = Graph->GetNode(NodeID);
    return Node ? Node->Coord : FVector::ZeroVector;
}


//////////////////////////////////////// Graph Generator //////////////////////////////////////////////

class FSnapGridFlowUtils {
public:
    static FBox GetLayoutNodeBounds(const FVector& LayoutCoord, const FVector& BaseOffset, const FVector& ModuleSize) {
        const FVector TargetMin = BaseOffset + LayoutCoord * ModuleSize;
        return FBox(TargetMin, TargetMin + ModuleSize);
    }
};

bool FSnapGridFlowGraphGenerator::ModuleOccludes(const SnapLib::FModuleNodePtr ModuleNode,
                                                 SnapLib::ISnapGraphNodePtr MissionNode,
                                                 const TArray<FBox>& OcclusionList) {
    const TSharedPtr<FSnapGridFlowGraphNode> LayoutMissionNode = StaticCastSharedPtr<FSnapGridFlowGraphNode
    >(MissionNode);
    const FVector LayoutCoord = LayoutMissionNode->GetNodeCoord();
    const FBox TargetBounds = FSnapGridFlowUtils::GetLayoutNodeBounds(LayoutCoord, BaseOffset, ModuleSize);
    FBox ModuleBounds = ModuleNode->GetModuleBounds();
    ModuleBounds = ModuleBounds.ExpandBy(-StaticState.BoundsContraction);
    return !TargetBounds.IsInside(ModuleBounds);
}

TArray<FTransform> FSnapGridFlowGraphGenerator::GetStartingNodeTransforms(SnapLib::FModuleNodePtr ModuleNode, SnapLib::ISnapGraphNodePtr MissionNode) {
    const TSharedPtr<FSnapGridFlowGraphNode> LayoutMissionNode = StaticCastSharedPtr<FSnapGridFlowGraphNode
    >(MissionNode);
    const FVector LayoutCoord = LayoutMissionNode->GetNodeCoord();

    TArray<FTransform> Result;
    TArray<float> Angles = {0, PI * 0.5f, PI, PI * 1.5f};
    TArray<int32> ShuffledAngleIndices = FMathUtils::GetShuffledIndices(4, StaticState.Random);
    FBox TargetBounds = FSnapGridFlowUtils::GetLayoutNodeBounds(LayoutCoord, BaseOffset, ModuleSize);
    for (int i = 0; i < Angles.Num(); i++) {
        float Angle = Angles[ShuffledAngleIndices[i]];
        FQuat Rotation(FVector::UpVector, Angle);
        FTransform RotTransform = FTransform(Rotation, FVector::ZeroVector);

        const FBox BaseModuleBounds = ModuleNode->GetModuleBounds();
        const FBox RotModuleBounds = BaseModuleBounds.TransformBy(RotTransform);

        FVector Offset = TargetBounds.GetCenter() - RotModuleBounds.GetCenter();
        FTransform TargetTransform(Rotation, Offset);
        Result.Add(TargetTransform);
    }
    return Result;
}

//////////////////////////////////////// Module Database Adapter //////////////////////////////////////////////

SnapLib::FModuleNodePtr FSnapGridFlowGraphModDBItemImpl::CreateModuleNode(const FGuid& InNodeId) {
    SnapLib::FModuleNodePtr Node = MakeShareable(new SnapLib::FModuleNode);
    Node->ModuleInstanceId = InNodeId;
    Node->ModuleDBItem = SharedThis(this);

    for (const FSnapGridFlowModuleDatabaseConnectionInfo& DoorInfo : Item.Connections) {
        SnapLib::FModuleDoorPtr Door = MakeShareable(new SnapLib::FModuleDoor);
        Door->ConnectionId = DoorInfo.ConnectionId;
        Door->ConnectionInfo = DoorInfo.ConnectionInfo;
        Door->ConnectionConstraint = DoorInfo.ConnectionConstraint;
        Door->LocalTransform = DoorInfo.Transform;
        Door->Owner = Node;
        Node->Doors.Add(Door);
    }

    return Node;
}

FSnapGridFlowModuleDatabaseImpl::FSnapGridFlowModuleDatabaseImpl(USnapGridFlowModuleDatabase* ModuleDB) {
    if (ModuleDB) {
        for (const FSnapGridFlowModuleDatabaseItem& ModuleInfo : ModuleDB->Modules) {
            TArray<SnapLib::IModuleDatabaseItemPtr>& CategoryModuleNames = ModulesByCategory.FindOrAdd(ModuleInfo.Category);
            CategoryModuleNames.Add(MakeShareable(new FSnapGridFlowGraphModDBItemImpl(ModuleInfo)));
        }
         
    }

    ChunkSize = (ModuleDB && ModuleDB->ModuleBoundsAsset) ? ModuleDB->ModuleBoundsAsset->ChunkSize : FVector::ZeroVector;
}

