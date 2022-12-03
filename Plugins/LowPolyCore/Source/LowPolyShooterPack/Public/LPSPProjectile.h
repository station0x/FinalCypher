//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LPSPProjectile.generated.h"

/**
 *Base class used by all projectiles in the asset.
 *Very useful for things shot from a weapon!
 */
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPProjectile final : public AActor
{
	GENERATED_BODY()

public:
	/**Constructor.*/
	ALPSPProjectile();
};