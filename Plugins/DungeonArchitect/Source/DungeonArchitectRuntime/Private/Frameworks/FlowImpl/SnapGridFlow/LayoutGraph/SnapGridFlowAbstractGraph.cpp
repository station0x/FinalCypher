//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/SnapGridFlowAbstractGraph.h"


#define LOCTEXT_NAMESPACE "USnapGridFlowAbstractGraph"

///////////////////////////////// UGridFlowAbstractGraph3D ///////////////////////////////////////
void USnapGridFlowAbstractGraph::CloneFromStateObject(UObject* SourceObject) {
    USnapGridFlowAbstractGraph* SourceGraph = Cast<USnapGridFlowAbstractGraph>(SourceObject);
    if (!SourceGraph) return;
    CopyStateFrom(SourceGraph);
    GridSize = SourceGraph->GridSize;
}

#undef LOCTEXT_NAMESPACE

