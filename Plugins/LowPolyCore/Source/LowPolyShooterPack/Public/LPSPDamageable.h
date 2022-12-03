//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LPSPDamageable.generated.h"

/**Base class for all Damageables.*/
UCLASS()
class LOWPOLYSHOOTERPACK_API ALPSPDamageable final : public AActor
{
	GENERATED_BODY()

public:
	/**Constructor.*/
	ALPSPDamageable();

	/**Returns the value of GroundChecks.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Damageable")
	bool GetGroundChecks() const { return bGroundChecks; }

	/**Sets the value of GroundChecks.*/
	UFUNCTION(BlueprintSetter, Category = "Low Poly Shooter Pack | Damageable")
	void SetGroundChecks(const bool Value = false) { bGroundChecks = Value; }

	/**Returns the value of GroundCheckDelay.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Damageable")
	float GetGroundCheckDelay() const { return GroundCheckDelay; }

	/**Sets the value of GroundCheckDelay.*/
	UFUNCTION(BlueprintSetter, Category = "Low Poly Shooter Pack | Damageable")
	void SetGroundCheckDelay(const float Value = 0.1f) { GroundCheckDelay = Value; }
	
	/**Returns the value of GroundCheckTypes.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Damageable")
	TArray<TEnumAsByte<EObjectTypeQuery>> GetGroundCheckTypes() const { return GroundCheckTypes; }

	/**Sets the value of GroundCheckTypes.*/
	UFUNCTION(BlueprintSetter, Category = "Low Poly Shooter Pack | Damageable")
	void SetGroundCheckTypes(const TArray<TEnumAsByte<EObjectTypeQuery>> Value) { GroundCheckTypes = Value; }

	/**Apply point damage to this actor.*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Damageable",
		meta = (DisplayName = "Damage Point"))
	void PointDamage(float Damage = 0.0f, AActor* DamageCauser = nullptr);
	
	/**Apply radial damage to this actor.*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Damageable",
		meta = (DisplayName = "Damage Radial"))
	void RadialDamage(float Damage = 0.0f, AActor* DamageCauser = nullptr);

protected:
	/**Does this Damageable directly die when falling? This is very helpful when
	 *dealing with Actors that should never be in the air.*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetGroundChecks, BlueprintSetter = SetGroundChecks,
		Category = "Low Poly Shooter Pack | Damageable | Ground Checks")
	bool bGroundChecks = false;

	/**Time between each ground check. It's basically the update rate of the ground check. This basically determines how many times we check if the Damageable is standing on the ground per-frame.*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetGroundCheckDelay, BlueprintSetter = SetGroundCheckDelay,
	Category = "Low Poly Shooter Pack | Damageable | Ground Checks", meta = (EditCondition = "bGroundChecks"))
	float GroundCheckDelay = 0.1f;

	/**Type of objects to check for ground. These objects are the only ones that count as "ground" for this Damageable.*/
	UPROPERTY(EditAnywhere, BlueprintGetter = GetGroundCheckTypes, BlueprintSetter = SetGroundCheckTypes,
	Category = "Low Poly Shooter Pack | Damageable | Ground Checks", meta = (EditCondition = "bGroundChecks"))
	TArray<TEnumAsByte<EObjectTypeQuery>> GroundCheckTypes;
	
	/**Begin Play.*/
	virtual void BeginPlay() override;
};