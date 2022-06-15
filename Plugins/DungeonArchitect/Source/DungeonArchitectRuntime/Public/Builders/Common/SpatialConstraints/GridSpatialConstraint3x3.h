//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Builders/Common/SpatialConstraints/GridSpatialConstraintCellData.h"
#include "Core/DungeonSpatialConstraint.h"
#include "GridSpatialConstraint3x3.generated.h"

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FGridSpatialConstraint3x3Data {
    GENERATED_USTRUCT_BODY()
    FGridSpatialConstraint3x3Data() {
        Cells.SetNum(9);
    }

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Spatial Setup")
    TArray<FGridSpatialConstraintCellData> Cells;
};


UCLASS(DefaultToInstanced, Blueprintable)
class DUNGEONARCHITECTRUNTIME_API UGridSpatialConstraint3x3 : public UDungeonSpatialConstraint {
    GENERATED_UCLASS_BODY()

public:

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Spatial Setup")
    FGridSpatialConstraint3x3Data Configuration;
};

