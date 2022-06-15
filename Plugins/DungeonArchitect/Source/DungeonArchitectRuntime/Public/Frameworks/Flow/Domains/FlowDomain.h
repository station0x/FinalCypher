//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class UFlowExecTask;

class DUNGEONARCHITECTRUNTIME_API IFlowDomain {
public:
    virtual ~IFlowDomain() {}
    virtual FName GetDomainID() const = 0;
    virtual FText GetDomainDisplayName() const = 0;
    virtual void GetDomainTasks(TArray<UClass*>& OutTaskClasses) const = 0;

#if WITH_EDITOR
    virtual UFlowExecTask* TryCreateCompatibleTask(UFlowExecTask* InTaskObject) { return nullptr; }
#endif // WITH_EDITOR
    
};
typedef TSharedPtr<class IFlowDomain> IFlowDomainPtr;
typedef TWeakPtr<class IFlowDomain> IFlowDomainWeakPtr;

