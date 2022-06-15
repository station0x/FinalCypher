//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Flow/Domains/AbstractGraph/Nodes/GridFlowAbstractEdGraphNodes.h"

#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/GridFlowAbstractGraph.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/GridFlowTilemap.h"

#define LOCTEXT_NAMESPACE "GridFlowAbstractEdGraphNode"


FText UGridFlowAbstractEdGraphNode::GetNodeTitle(ENodeTitleType::Type TitleType) const {
    if (!ScriptNode.IsValid() || !ScriptNode->bActive) {
        return FText();
    }

    switch (ScriptNode->FindOrAddDomainData<UFANodeTilemapDomainData>()->RoomType) {
    case EGridFlowAbstractNodeRoomType::Room:
        return LOCTEXT("RoomTypeText_Room", "R");

    case EGridFlowAbstractNodeRoomType::Corridor:
        return LOCTEXT("RoomTypeText_Room", "Co");

    case EGridFlowAbstractNodeRoomType::Cave:
        return LOCTEXT("RoomTypeText_Room", "Ca");

    case EGridFlowAbstractNodeRoomType::Unknown:
    default:
        return FText();
    }
}


#undef LOCTEXT_NAMESPACE

