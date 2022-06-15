//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Utils/Grid3DFlowAbstractGraphPathUtils.h"

#include "Core/Utils/MathUtils.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/SnapGridFlowAbstractGraph.h"

bool UGrid3DLayoutNodeCreationConstraint::CanCreateLayoutNode_Implementation(const FIntVector& NodeCoord, const FIntVector& GridSize, int32 TotalPathLength, int32 CurrentPathPosition) {
    return true;
}

bool UGrid3DLayoutNodeCreationConstraint::CanCreateNodeAt(const UFlowAbstractNode* Node, int32 TotalPathLength, int32 CurrentPathPosition) {
    const FIntVector Coord = FMathUtils::ToIntVector(Node->Coord, true);
    USnapGridFlowAbstractGraph* Graph3D = Cast<USnapGridFlowAbstractGraph>(Node->GetOuter());
    if (Graph3D) {
        const FIntVector GridSize = Graph3D->GridSize;
        return CanCreateLayoutNode(Coord, GridSize, TotalPathLength, CurrentPathPosition);
    }
    return Super::CanCreateNodeAt(Node, TotalPathLength, CurrentPathPosition);
}

