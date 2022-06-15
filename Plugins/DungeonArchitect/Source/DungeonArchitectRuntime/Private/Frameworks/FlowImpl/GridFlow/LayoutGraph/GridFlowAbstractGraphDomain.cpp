//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/GridFlowAbstractGraphDomain.h"

#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/Tasks/GridFlowLayoutTaskCreateGrid.h"
#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/Tasks/GridFlowLayoutTaskCreateKeyLock.h"
#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/Tasks/GridFlowLayoutTaskCreateMainPath.h"
#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/Tasks/GridFlowLayoutTaskCreatePath.h"
#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/Tasks/GridFlowLayoutTaskFinalize.h"
#include "Frameworks/FlowImpl/GridFlow/LayoutGraph/Tasks/GridFlowLayoutTaskSpawnItems.h"

#define LOCTEXT_NAMESPACE "GridFlowAbstractGraphDomain"

const FName FGridFlowAbstractGraphDomain::DomainID = TEXT("AbstractGraph");

FName FGridFlowAbstractGraphDomain::GetDomainID() const {
	return DomainID;
}

FText FGridFlowAbstractGraphDomain::GetDomainDisplayName() const {
	return LOCTEXT("DTMDisplayName", "Layout Graph");
}

void FGridFlowAbstractGraphDomain::GetDomainTasks(TArray<UClass*>& OutTaskClasses) const {
	static const TArray<UClass*> DomainTasks = {
		UGridFlowLayoutTaskCreateGrid::StaticClass(),
		UGridFlowLayoutTaskCreateMainPath::StaticClass(),
		UGridFlowLayoutTaskCreatePath::StaticClass(),
		UGridFlowLayoutTaskSpawnItems::StaticClass(),
		UGridFlowLayoutTaskCreateKeyLock::StaticClass(),
		UGridFlowLayoutTaskFinalize::StaticClass()
	};
	OutTaskClasses = DomainTasks;
}


#undef LOCTEXT_NAMESPACE

