//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPDirectionVectors.h"
#include "LPSPDirectionTransform.generated.h"

/**Location and rotation direction vectors.*/
USTRUCT(BlueprintType)
struct LOWPOLYSHOOTERPACK_API FLPSPDirectionTransform
{
	GENERATED_BODY()

public:
	/**Horizontal vector.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FLPSPDirectionVectors Location;

	/**Vertical vector.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FLPSPDirectionVectors Rotation;
};