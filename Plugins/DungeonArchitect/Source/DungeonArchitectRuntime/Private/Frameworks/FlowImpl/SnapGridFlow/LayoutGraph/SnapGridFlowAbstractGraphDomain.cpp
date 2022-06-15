//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/SnapGridFlowAbstractGraphDomain.h"

#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreateGrid.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreateKeyLock.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreateMainPath.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreatePath.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskFinalize.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskSpawnItems.h"

#define LOCTEXT_NAMESPACE "SnapGridFlowAbstractGraphDomain"

////////////////////////////// FFlowDomainAbstractGraph3D //////////////////////////////
const FName FSnapGridFlowAbstractGraphDomain::DomainID = TEXT("AbstractGraph3D");
FName FSnapGridFlowAbstractGraphDomain::GetDomainID() const {
    return DomainID;
}

FText FSnapGridFlowAbstractGraphDomain::GetDomainDisplayName() const {
    return LOCTEXT("DTM3DDisplayName", "Layout Graph 3D");
}

void FSnapGridFlowAbstractGraphDomain::GetDomainTasks(TArray<UClass*>& OutTaskClasses) const {
    static const TArray<UClass*> DomainTasks = {
        USnapGridFlowLayoutTaskCreateGrid::StaticClass(),
        USnapGridFlowLayoutTaskCreateMainPath::StaticClass(),
        USnapGridFlowLayoutTaskCreatePath::StaticClass(),
        USnapGridFlowLayoutTaskSpawnItems::StaticClass(),
        USnapGridFlowLayoutTaskCreateKeyLock::StaticClass(),
        USnapGridFlowLayoutTaskFinalize::StaticClass()
    };
    OutTaskClasses = DomainTasks;
}

#if WITH_EDITOR
UFlowExecTask* FSnapGridFlowAbstractGraphDomain::TryCreateCompatibleTask(UFlowExecTask* InTaskObject) {
    if (!InTaskObject) {
        return nullptr;
    }

    TSubclassOf<UFlowExecTask> TargetTaskClass = nullptr;
    if (InTaskObject->IsA<UBaseFlowLayoutTaskCreateMainPath>()) {
        TargetTaskClass = USnapGridFlowLayoutTaskCreateMainPath::StaticClass();
    }
    else if (InTaskObject->IsA<UBaseFlowLayoutTaskCreatePath>()) {
        TargetTaskClass = USnapGridFlowLayoutTaskCreatePath::StaticClass();
    }
    else if (InTaskObject->IsA<UBaseFlowLayoutTaskSpawnItems>()) {
        TargetTaskClass = USnapGridFlowLayoutTaskSpawnItems::StaticClass();
    }
    else if (InTaskObject->IsA<UBaseFlowLayoutTaskCreateKeyLock>()) {
        TargetTaskClass = USnapGridFlowLayoutTaskCreateKeyLock::StaticClass();
    }
    else if (InTaskObject->IsA<UBaseFlowLayoutTaskFinalize>()) {
        TargetTaskClass = USnapGridFlowLayoutTaskFinalize::StaticClass();
    }

    if (!TargetTaskClass) {
        return nullptr;
    }

    UFlowExecTask* CompatibleTask = NewObject<UFlowExecTask>(InTaskObject->GetOuter(), TargetTaskClass);
    UEngine::CopyPropertiesForUnrelatedObjects(InTaskObject, CompatibleTask);
    return CompatibleTask;
}
#endif // WITH_EDITOR


void FSnapGridFlowAbstractGraphDomain::SetModuleDatabase(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase) {
    ModuleDatabase = InModuleDatabase;
}

#undef LOCTEXT_NAMESPACE

