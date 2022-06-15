//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreatePath.h"

#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraphQuery.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/SnapGridFlowAbstractGraphDomain.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Utils/Grid3DFlowAbstractGraphPathUtils.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleDatabase.h"

DEFINE_LOG_CATEGORY_STATIC(LogSGFCreatePath, Log, All);

IFlowAGNodeGroupGeneratorPtr USnapGridFlowLayoutTaskCreatePath::CreateNodeGroupGenerator(TWeakPtr<const IFlowDomain> InDomainPtr) const {
	const TSharedPtr<const IFlowDomain> Domain = InDomainPtr.Pin();
	if (!Domain.IsValid() || Domain->GetDomainID() != FSnapGridFlowAbstractGraphDomain::DomainID) {
		return MakeShareable(new FNullFlowAGNodeGroupGenerator);
	}
    
	const TSharedPtr<const FSnapGridFlowAbstractGraphDomain> SGFLayoutGraphDomain = StaticCastSharedPtr<const FSnapGridFlowAbstractGraphDomain>(Domain);
	USnapGridFlowModuleDatabase* ModuleDatabase = SGFLayoutGraphDomain->GetModuleDatabase();
	if (!ModuleDatabase) {
		return MakeShareable(new FNullFlowAGNodeGroupGenerator); 
	}
	
	const bool bSupportsDoorCategory = SGFLayoutGraphDomain->SupportsDoorCategory();
	const TSharedPtr<FSGFNodeCategorySelector> NodeCategorySelector = MakeShareable(new FSGFNodeCategorySelector(
				ModuleCategories, ModuleCategoryOverrideMethod, StartNodeCategoryOverride, EndNodeCategoryOverride, CategoryOverrideLogic));
	
	if (bSupportsDoorCategory) {
		return MakeShareable(new FSnapFlowAGNodeGroupGenerator(ModuleDatabase, NodeCategorySelector));
	}
	else {
		return MakeShareable(new FSnapFlowAGIgnoreDoorCategoryNodeGroupGenerator(ModuleDatabase, NodeCategorySelector));
	}
}

FFlowAbstractGraphConstraintsPtr USnapGridFlowLayoutTaskCreatePath::CreateGraphConstraints(TWeakPtr<const IFlowDomain> InDomainPtr) const {
	const TSharedPtr<const IFlowDomain> Domain = InDomainPtr.Pin();
	if (!Domain.IsValid() || Domain->GetDomainID() != FSnapGridFlowAbstractGraphDomain::DomainID) {
		return MakeShareable(new FNullFlowAbstractGraphConstraints);
	}
    
	const TSharedPtr<const FSnapGridFlowAbstractGraphDomain> SGFLayoutGraphDomain = StaticCastSharedPtr<const FSnapGridFlowAbstractGraphDomain>(Domain);
	USnapGridFlowModuleDatabase* ModuleDatabase = SGFLayoutGraphDomain->GetModuleDatabase();
	if (!ModuleDatabase) {
	    return MakeShareable(new FNullFlowAbstractGraphConstraints);
    }

	const bool bSupportsDoorCategory = SGFLayoutGraphDomain->SupportsDoorCategory();
	const TSharedPtr<FSGFNodeCategorySelector> NodeCategorySelector = MakeShareable(new FSGFNodeCategorySelector(
					ModuleCategories, ModuleCategoryOverrideMethod, StartNodeCategoryOverride, EndNodeCategoryOverride, CategoryOverrideLogic));
	return MakeShareable(new FSnapGridFlowAbstractGraphConstraints(ModuleDatabase, NodeCategorySelector, bSupportsDoorCategory));
}

UFlowLayoutNodeCreationConstraint* USnapGridFlowLayoutTaskCreatePath::GetNodeCreationConstraintLogic() const {
	return bUseNodeCreationConstraint ? NodeCreationConstraint : nullptr;
}

void USnapGridFlowLayoutTaskCreatePath::FinalizePath(const FFlowAGStaticGrowthState& StaticState, FFlowAGGrowthState& State) const {
	Super::FinalizePath(StaticState, State);

	const FFlowAbstractGraphQuery GraphQuery(StaticState.Graph);
	const int32 PathLength = State.Path.Num();
	FSGFNodeCategorySelector NodeCategorySelector(ModuleCategories, ModuleCategoryOverrideMethod, StartNodeCategoryOverride, EndNodeCategoryOverride, CategoryOverrideLogic);

	for (int PathIdx = 0; PathIdx < PathLength; PathIdx++) {
		const FFlowAGGrowthState_PathItem& Path = State.Path[PathIdx];
		UFlowAbstractNode* Node = GraphQuery.GetNode(Path.NodeId);
        
		UFANodeSnapDomainData* NodeDomainData = Node->FindOrAddDomainData<UFANodeSnapDomainData>();
		NodeDomainData->ModuleCategories = NodeCategorySelector.GetCategoriesAtNode(Node->PathIndex, Node->PathLength);

		if (Path.UserData.IsValid()) {
			TSharedPtr<FSGFNodeGroupUserData> GroupUserData = StaticCastSharedPtr<FSGFNodeGroupUserData>(Path.UserData);
			USGFNodeGroupUserData* GroupDomainData = Node->FindOrAddDomainData<USGFNodeGroupUserData>();
			GroupDomainData->CopyFrom(*GroupUserData);
		}
	}
}

