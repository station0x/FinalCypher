//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LPSPGameAbility.generated.h"

/**Ability used in-gameplay. Basically a custom implementation of gameplay abilities for simplicity.*/
USTRUCT(BlueprintType)
struct LOWPOLYSHOOTERPACK_API FLPSPGameAbility
{
	GENERATED_BODY()

public:
	/**Return true if this ability can start.*/
	bool CanStart(FGameplayTagContainer Container) const;
	
	/**Tries to start the ability. Returns false if it was not possible.*/
	bool TryStart(FGameplayTagContainer& Container) const;

	/**Starts the ability.*/
	void Start(FGameplayTagContainer& Container) const;

	/**Stops the ability.*/
	void Stop(FGameplayTagContainer& Container, bool AddRemoved = true) const;

private:
	/**Tags required for the ability to be allowed to start.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	FGameplayTagContainer RequiredTags;

	/**Tags that can stop this ability from starting. Any of these will stop the ability from starting.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	FGameplayTagContainer BlockedTags;

	/**Tags to add when the ability starts. These tags are removed when the ability is done.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	FGameplayTagContainer AddedTags;

	/**Tags to remove when the ability starts. These tags are re-added when the ability is done.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	FGameplayTagContainer RemovedTags;
};