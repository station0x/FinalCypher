//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LPSPGameAbility.h"

#include "LPSPGameAbilityLibrary.generated.h"

/**Class containing static helper functions for game abilities.*/
UCLASS()
class LOWPOLYSHOOTERPACK_API ULPSPGameAbilityLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
public:
	/**Tries to start the ability with the container. Returns false if it was not possible.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Game Ability Library", meta = (DisplayName = "Game Ability Try Start", BlueprintThreadSafe))
	static bool TryStartGameAbility(const FLPSPGameAbility Ability, UPARAM(ref) FGameplayTagContainer& Container)
	{
		//Start.
		return Ability.TryStart(Container);
	}

	/**Returns true if the ability can be started.*/
	UFUNCTION(BlueprintPure, Category = "Low Poly Shooter Pack | Game Ability Library", meta = (DisplayName = "Game Ability Can Start", BlueprintThreadSafe))
	static bool CanStartGameAbility(const FLPSPGameAbility Ability, UPARAM(ref) FGameplayTagContainer& Container)
	{
		//Start.
		return Ability.CanStart(Container);
	}

	/**Starts a game ability.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Game Ability Library", meta = (DisplayName = "Game Ability Start", BlueprintThreadSafe))
	static void StartGameAbility(const FLPSPGameAbility Ability, UPARAM(ref) FGameplayTagContainer& Container)
	{
		//Start.
		Ability.Start(Container);
	}
	
	/**Stops a game ability.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Game Ability Library", meta = (DisplayName = "Game Ability Stop", BlueprintThreadSafe))
	static void StopGameAbility(const FLPSPGameAbility Ability, UPARAM(ref) FGameplayTagContainer& Container, const bool AddRemoved = true)
	{
		//Stop.
		Ability.Stop(Container, AddRemoved);
	}

	/**Debugs all tags in a container.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Game Ability Library", meta = (DisplayName = "Game Container Debug", BlueprintThreadSafe))
	static void DebugContainer(const FGameplayTagContainer Container)
	{
		//Get tags.
		TArray<FGameplayTag> Tags;
		Container.GetGameplayTagArray(Tags);

		//Debug.
		for (FGameplayTag Tag : Tags)
			GEngine->AddOnScreenDebugMessage(INDEX_NONE, 1.0f, FColor::Red, Tag.GetTagName().ToString());
	}
};