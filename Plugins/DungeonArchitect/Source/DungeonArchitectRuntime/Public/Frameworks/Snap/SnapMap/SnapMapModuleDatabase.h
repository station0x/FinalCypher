//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Snap/Lib/Connection/SnapConnectionConstants.h"
#include "Frameworks/Snap/Lib/SnapLibrary.h"

#include "Engine/DataAsset.h"
#include "SnapMapModuleDatabase.generated.h"

USTRUCT()
struct DUNGEONARCHITECTRUNTIME_API FSnapMapModuleDatabaseConnectionInfo {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(VisibleAnywhere, Category = Module)
    FGuid ConnectionId;

    UPROPERTY(VisibleAnywhere, Category = Module)
    FTransform Transform;

    UPROPERTY(VisibleAnywhere, Category = Module)
    class USnapConnectionInfo* ConnectionInfo = nullptr;

    UPROPERTY(VisibleAnywhere, Category = Module)
    ESnapConnectionConstraint ConnectionConstraint = ESnapConnectionConstraint::Magnet;
};

USTRUCT()
struct DUNGEONARCHITECTRUNTIME_API FSnapMapModuleDatabaseItem {
    GENERATED_USTRUCT_BODY()

    UPROPERTY(EditAnywhere, Category = Module)
    TSoftObjectPtr<UWorld> Level;

    UPROPERTY(EditAnywhere, Category = Module)
    FName Category = "Room";

    UPROPERTY(VisibleAnywhere, Category = Module)
    FBox ModuleBounds = FBox(ForceInitToZero);

    UPROPERTY(VisibleAnywhere, Category = Module)
    TArray<FSnapMapModuleDatabaseConnectionInfo> Connections;
};

uint32 GetTypeHash(const FSnapMapModuleDatabaseItem& A);


UCLASS()
class DUNGEONARCHITECTRUNTIME_API USnapMapModuleDatabase : public UDataAsset {
    GENERATED_BODY()
public:
    UPROPERTY(EditAnywhere, Category = Module)
    TArray<FSnapMapModuleDatabaseItem> Modules;

};

