//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/FlowDomain.h"

class DUNGEONARCHITECTRUNTIME_API FGridFlowTilemapDomain : public IFlowDomain {
    public:
    virtual FName GetDomainID() const override;
    virtual FText GetDomainDisplayName() const override;
    virtual void GetDomainTasks(TArray<UClass*>& OutTaskClasses) const override;
    
public:
    static const FName DomainID;
};

