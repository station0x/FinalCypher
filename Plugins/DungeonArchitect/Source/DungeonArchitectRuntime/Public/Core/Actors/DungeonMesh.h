//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Materials/MaterialInterface.h"
#include "PhysicsEngine/BodyInstance.h"
#include "DungeonMesh.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FMaterialOverride {
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    int32 index = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    UMaterialInterface* Material = nullptr;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UDungeonMesh : public UObject {
    GENERATED_UCLASS_BODY()

public:
    UPROPERTY()
    UStaticMesh* StaticMesh;

    UPROPERTY()
    TArray<FMaterialOverride> MaterialOverrides;

    UPROPERTY()
    uint32 HashCode;

    UPROPERTY()
    FBodyInstance BodyInstance;

    UPROPERTY()
    bool bUseCustomCollision = false;

    UPROPERTY()
    UStaticMeshComponent* Template;

    void CalculateHashCode();
};

