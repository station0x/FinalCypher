//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPAttachment.h"
#include "LPSPLaser.generated.h"

/**Weapon Laser Attachment.*/
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPLaser final : public ALPSPAttachment
{
	GENERATED_BODY()

public:
	/**Should this laser get turned off while the player sprints?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Laser")
	bool OffWhileSprinting = true;

	/**Should this laser be turned off while the player aims?*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Laser")
	bool OffWhileAiming = true;
};