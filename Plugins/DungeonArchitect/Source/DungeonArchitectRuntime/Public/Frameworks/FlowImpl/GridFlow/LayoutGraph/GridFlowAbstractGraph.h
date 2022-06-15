//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "GridFlowAbstractGraph.generated.h"

///////////////////////////// Grid Flow Abstract Graph /////////////////////////////
UCLASS()
class DUNGEONARCHITECTRUNTIME_API UGridFlowAbstractGraph : public UFlowAbstractGraphBase, public IFlowExecCloneableState {
    GENERATED_BODY()
    
public:
    virtual void CloneFromStateObject(UObject* SourceObject) override;

public:
    UPROPERTY()
    FIntPoint GridSize;
};

