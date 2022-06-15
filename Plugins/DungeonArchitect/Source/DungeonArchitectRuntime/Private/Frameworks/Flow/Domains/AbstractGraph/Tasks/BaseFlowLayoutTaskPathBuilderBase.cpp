//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Flow/Domains/AbstractGraph/Tasks/BaseFlowLayoutTaskPathBuilderBase.h"

#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraphConstraints.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Tasks/Lib/FlowAbstractGraphPathUtils.h"
#include "Frameworks/Flow/Domains/FlowDomain.h"

void UBaseFlowLayoutTaskPathBuilderBase::FinalizePath(const FFlowAGStaticGrowthState& StaticState, FFlowAGGrowthState& State) const {
    FFlowAbstractGraphPathUtils::FinalizePath(StaticState, State);
}

IFlowAGNodeGroupGeneratorPtr UBaseFlowLayoutTaskPathBuilderBase::CreateNodeGroupGenerator(TWeakPtr<const IFlowDomain> InDomainPtr) const {
    return MakeShareable(new FNullFlowAGNodeGroupGenerator);
}

FFlowAbstractGraphConstraintsPtr UBaseFlowLayoutTaskPathBuilderBase::CreateGraphConstraints(TWeakPtr<const IFlowDomain> InDomainPtr) const {
    return MakeShareable(new FNullFlowAbstractGraphConstraints);
}

UFlowLayoutNodeCreationConstraint* UBaseFlowLayoutTaskPathBuilderBase::GetNodeCreationConstraintLogic() const {
    return nullptr;
}

