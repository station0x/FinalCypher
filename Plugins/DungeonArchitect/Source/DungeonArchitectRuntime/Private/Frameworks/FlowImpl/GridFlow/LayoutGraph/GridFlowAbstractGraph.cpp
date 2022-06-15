//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/GridFlowAbstractGraph.h"


#define LOCTEXT_NAMESPACE "FlowDomainAbstractGraph"

///////////////////////////////// UGridFlowAbstractGraph ///////////////////////////////////////
void UGridFlowAbstractGraph::CloneFromStateObject(UObject* SourceObject) {
    UGridFlowAbstractGraph* SourceGraph = Cast<UGridFlowAbstractGraph>(SourceObject);
    if (!SourceGraph) return;
    CopyStateFrom(SourceGraph);
    GridSize = SourceGraph->GridSize;
}

#undef LOCTEXT_NAMESPACE

