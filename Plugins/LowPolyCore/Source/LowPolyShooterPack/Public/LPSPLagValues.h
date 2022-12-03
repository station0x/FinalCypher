//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPDirectionTransform.h"
#include "LPSPLagValues.generated.h"

/**Location and rotation direction vectors.*/
USTRUCT(BlueprintType)
struct LOWPOLYSHOOTERPACK_API FLPSPLagValues
{
	GENERATED_BODY()

public:
	/**Returns the value of Movement.*/
	FLPSPDirectionTransform GetMovement() const { return Movement; }

	/**Returns the value of Look.*/
	FLPSPDirectionTransform GetLook() const { return Look; }

	/**Returns the value of Stiffness.*/
	float GetStiffness() const { return Stiffness; }

	/**Returns the value of Damping.*/
	float GetDamping() const { return Damping; }

private:
	/**Movement direction transform.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	FLPSPDirectionTransform Movement;

	/**Look direction transform.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	FLPSPDirectionTransform Look;

	/**Determines the stiffness value used when interpolating the values.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	float Stiffness = 0.4f;

	/**Determines the damping value used when interpolating the values.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	float Damping = 0.5f;
};