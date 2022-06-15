//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Builders/SnapGridFlow/SnapGridFlowAsset.h"

#include "Frameworks/Flow/Domains/Snap/SnapFlowAbstractGraphSupport.h"
#include "Frameworks/Flow/ExecGraph/FlowExecGraphScript.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreateMainPath.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreatePath.h"

void FSnapGridFlowAssetRuntimeUpgrader::Upgrade(USnapGridFlowAsset* InAsset) {
	if (!InAsset || !InAsset->ExecScript || !InAsset->ExecScript->ScriptGraph) return;
	if (InAsset->Version == static_cast<int>(ESnapGridFlowAssetVersion::LatestVersion)) {
		return;
	}

	if (InAsset->Version + 1 == static_cast<int>(ESnapGridFlowAssetVersion::DeprecateTaskExtensions)) {
		for (UGridFlowExecScriptGraphNode* ScriptNode : InAsset->ExecScript->ScriptGraph->Nodes) {
			if (const UGridFlowExecScriptTaskNode* TaskNode = Cast<UGridFlowExecScriptTaskNode>(ScriptNode)) {
				FSnapGridFlowAssetUpgradeLib::DeprecateTaskExtensions(TaskNode->Task);
			}
		}
		
		// Bring this to the next version
		InAsset->Version++;
	}

	InAsset->Modify();
	check(InAsset->Version == static_cast<int>(ESnapGridFlowAssetVersion::DeprecateTaskExtensions));
}

//////////////////////////////////////// FSnapGridFlowAssetUpgradeLib ////////////////////////////////////////
void FSnapGridFlowAssetUpgradeLib::DeprecateTaskExtensions(UFlowExecTask* Task) {
	if (!Task) return;
	for (UFlowExecTaskExtender* TaskExtender : Task->Extenders) {
		if (const USnapFlowAGTaskExtender* SGFTaskExtender = Cast<USnapFlowAGTaskExtender>(TaskExtender)) {
			if (USnapGridFlowLayoutTaskCreateMainPath* SGFCreateMainPathTask = Cast<USnapGridFlowLayoutTaskCreateMainPath>(Task)) {
				SGFCreateMainPathTask->ModuleCategories = SGFTaskExtender->ModuleCategories;
				SGFCreateMainPathTask->ModuleCategoryOverrideMethod = SGFTaskExtender->ModuleCategoryOverrideMethod;
				SGFCreateMainPathTask->StartNodeCategoryOverride = SGFTaskExtender->StartNodeCategoryOverride;
				SGFCreateMainPathTask->EndNodeCategoryOverride = SGFTaskExtender->EndNodeCategoryOverride;
				SGFCreateMainPathTask->CategoryOverrideLogic = SGFTaskExtender->CategoryOverrideLogic;
			}
			else if (USnapGridFlowLayoutTaskCreatePath* SGFCreatePathTask = Cast<USnapGridFlowLayoutTaskCreatePath>(Task)) {
				SGFCreatePathTask->ModuleCategories = SGFTaskExtender->ModuleCategories;
				SGFCreatePathTask->ModuleCategoryOverrideMethod = SGFTaskExtender->ModuleCategoryOverrideMethod;
				SGFCreatePathTask->StartNodeCategoryOverride = SGFTaskExtender->StartNodeCategoryOverride;
				SGFCreatePathTask->EndNodeCategoryOverride = SGFTaskExtender->EndNodeCategoryOverride;
				SGFCreatePathTask->CategoryOverrideLogic = SGFTaskExtender->CategoryOverrideLogic;
			}
		}
	}
}

