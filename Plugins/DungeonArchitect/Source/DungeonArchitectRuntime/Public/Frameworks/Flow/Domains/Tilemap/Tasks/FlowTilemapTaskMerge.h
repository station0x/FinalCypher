//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/Tilemap/Tasks/FlowTilemapTask.h"
#include "FlowTilemapTaskMerge.generated.h"

UCLASS(Abstract)
class DUNGEONARCHITECTRUNTIME_API UFlowTilemapTaskMerge : public UFlowTilemapTask {
	GENERATED_BODY()
public:
	virtual void Execute(const FFlowExecutionInput& Input, const FFlowTaskExecutionSettings& InExecSettings, FFlowExecutionOutput& Output) override;
};    

