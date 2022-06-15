//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Tasks/BaseFlowLayoutTaskFinalize.h"
#include "SnapGridFlowLayoutTaskFinalize.generated.h"

UCLASS(Meta = (AbstractTask, Title = "Finalize Graph", Tooltip = "Call this to finalize the layout graph", MenuPriority = 1500))
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowLayoutTaskFinalize : public UBaseFlowLayoutTaskFinalize {
    GENERATED_BODY()
public:

};

