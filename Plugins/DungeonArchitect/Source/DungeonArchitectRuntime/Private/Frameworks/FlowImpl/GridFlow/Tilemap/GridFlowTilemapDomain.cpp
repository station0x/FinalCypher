//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/GridFlow/Tilemap/GridFlowTilemapDomain.h"

#include "Frameworks/FlowImpl/GridFlow/Tilemap/Tasks/GridFlowTilemapTaskCreateElevations.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/Tasks/GridFlowTilemapTaskCreateOverlay.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/Tasks/GridFlowTilemapTaskFinalize.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/Tasks/GridFlowTilemapTaskInitialize.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/Tasks/GridFlowTilemapTaskMerge.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/Tasks/GridFlowTilemapTaskOptimize.h"

#define LOCTEXT_NAMESPACE "GridFlowDomainTilemap"

const FName FGridFlowTilemapDomain::DomainID = TEXT("GridFlowTilemap");

FName FGridFlowTilemapDomain::GetDomainID() const {
    return DomainID;
}

FText FGridFlowTilemapDomain::GetDomainDisplayName() const {
    return LOCTEXT("DTMDisplayName", "Tilemap");
}

void FGridFlowTilemapDomain::GetDomainTasks(TArray<UClass*>& OutTaskClasses) const {
    static const TArray<UClass*> DomainTasks = {
        UGridFlowTilemapTaskInitialize::StaticClass(),
        UGridFlowTilemapTaskCreateElevations::StaticClass(),
        UGridFlowTilemapTaskCreateOverlay::StaticClass(),
        UGridFlowTilemapTaskMerge::StaticClass(),
        UGridFlowTilemapTaskOptimize::StaticClass(),
        UGridFlowTilemapTaskFinalize::StaticClass()
    };
    OutTaskClasses = DomainTasks;
}


#undef LOCTEXT_NAMESPACE

