//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "ConnectionSerialization.generated.h"

USTRUCT()
struct DUNGEONARCHITECTRUNTIME_API FSnapConnectionInstance {
    GENERATED_USTRUCT_BODY()

    UPROPERTY()
    FGuid ModuleA;

    UPROPERTY()
    FGuid DoorA;

    UPROPERTY()
    FGuid ModuleB;

    UPROPERTY()
    FGuid DoorB;

    UPROPERTY()
    FTransform WorldTransform;

    UPROPERTY()
    bool bHasSpawnedDoorActor = false;

    UPROPERTY()
    TArray<TWeakObjectPtr<AActor>> SpawnedDoorActors;
};

