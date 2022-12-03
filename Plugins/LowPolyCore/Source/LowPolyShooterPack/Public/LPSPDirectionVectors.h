//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPDirectionVectors.generated.h"

/**Horizontal and vertical vector values.*/
USTRUCT(BlueprintType)
struct LOWPOLYSHOOTERPACK_API FLPSPDirectionVectors
{
	GENERATED_BODY()

public:
	/**Horizontal vector.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector Horizontal;

	/**Vertical vector.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector Vertical;
};