//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/FlowAssetBase.h"
#include "SnapGridFlowAsset.generated.h"

enum class ESnapGridFlowAssetVersion {
    InitialVersion = 0,
    DeprecateTaskExtensions,

    //----------- Versions should be placed above this line -----------------
    LastVersionPlusOne,
    LatestVersion = LastVersionPlusOne - 1
};

UCLASS()
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowAsset : public UFlowAssetBase {
    GENERATED_BODY()

public:
    UPROPERTY()
    int32 Version = 0;

};


class FSnapGridFlowAssetRuntimeUpgrader {
public:
    static void Upgrade(USnapGridFlowAsset* InAsset);

};

class DUNGEONARCHITECTRUNTIME_API FSnapGridFlowAssetUpgradeLib {
public:
    static void DeprecateTaskExtensions(class UFlowExecTask* Task);
};

