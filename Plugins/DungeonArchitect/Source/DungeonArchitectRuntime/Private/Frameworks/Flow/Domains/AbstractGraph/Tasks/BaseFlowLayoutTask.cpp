//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Flow/Domains/AbstractGraph/Tasks/BaseFlowLayoutTask.h"


#if WITH_EDITOR
FLinearColor UBaseFlowLayoutTask::GetNodeColor() const {
    return FLinearColor(0.08f, 0.08f, 0.08f) + FLinearColor(0.5f, 0, 0.2f) * .25f;

}
#endif // WITH_EDITOR

