//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "FlowTilemapTask.generated.h"

UCLASS(Abstract)
class DUNGEONARCHITECTRUNTIME_API UFlowTilemapTask : public UFlowExecTask {
	GENERATED_BODY()
public:
#if WITH_EDITOR
	virtual FLinearColor GetNodeColor() const override;
#endif // WITH_EDITOR
};

