//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "SnapGridFlowAbstractGraph.generated.h"

///////////////////////////// Grid Flow Abstract Graph 3D ///////////////////////////// 
UCLASS()
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowAbstractGraph : public UFlowAbstractGraphBase, public IFlowExecCloneableState {
    GENERATED_BODY()
public:
    virtual void CloneFromStateObject(UObject* SourceObject) override;

public:
    UPROPERTY()
    FIntVector GridSize;
};

