//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/FlowDomain.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleDatabase.h"

typedef TSharedPtr<class IFlowAGNodeGroupGenerator> IFlowAGNodeGroupGeneratorPtr;
typedef TSharedPtr<class FFlowAbstractGraphConstraints> FFlowAbstractGraphConstraintsPtr;

class DUNGEONARCHITECTRUNTIME_API FSnapGridFlowAbstractGraphDomain : public IFlowDomain {
public:
	virtual FName GetDomainID() const override;
	virtual FText GetDomainDisplayName() const override;
	virtual void GetDomainTasks(TArray<UClass*>& OutTaskClasses) const override;
    
#if WITH_EDITOR
	virtual UFlowExecTask* TryCreateCompatibleTask(UFlowExecTask* InTaskObject) override;
#endif // WITH_EDITOR

	void SetModuleDatabase(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase);
	USnapGridFlowModuleDatabase* GetModuleDatabase() const { return ModuleDatabase.Get(); }
	bool SupportsDoorCategory() const { return bSupportDoorCategory; }
	void SetSupportsDoorCategory(bool bInSupportDoorCategory) { bSupportDoorCategory = bInSupportDoorCategory; }
	
public:
	static const FName DomainID;

private:
	TWeakObjectPtr<USnapGridFlowModuleDatabase> ModuleDatabase;
	bool bSupportDoorCategory = false;
};

