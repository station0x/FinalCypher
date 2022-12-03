//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "Runtime/Core/Public/CoreMinimal.h"
#include "Runtime/Engine/Classes/GameFramework/Actor.h"
#include "Runtime/Engine/Public/Engine.h"
#include "LPSPCasing.generated.h"

/**Base Casing class. All Casings inherit from this.*/
UCLASS(Abstract, BlueprintType)
class LOWPOLYSHOOTERPACK_API ALPSPCasing final : public AActor
{
	GENERATED_BODY()

public:
	ALPSPCasing();
	
	/**Let the casing know that it has been spawned.
	 * If this function isn't called when spawning a casing, it will not get deleted properly.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Casing")
	void OnSpawn();

	/**Adds an impulse to this casing.
	 * This function is usually called to launch the casing in a specific direction.
	 */
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Casing")
	void ApplyImpulse(FVector Direction, const FVector Strength);

	/**Returns the value of bUpdateScale.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Low Poly Shooter Pack | Casing")
	bool ShouldUpdateScale() const { return bUpdateScale; }

	/**Returns the value of DestroyDelay.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Low Poly Shooter Pack | Casing")
	float GetDestroyDelay() const { return DestroyDelay; }

protected:
	/*Pivot.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	USceneComponent* Pivot;

	/**Casing mesh.*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	UStaticMeshComponent* Mesh;
	
private:
	/**Should we scale the casing down as time goes on?
	* This is very useful for casings that don't need to be visible after the first few moments, but do need all
	* other behavior.
	*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Casing")
	bool bUpdateScale = true;
	
	/**Speed of rotation for this casing.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Casing")
	float RotationSpeed = 2500.0f;

	/**Range of possible delay values used to destroy the casing.
	 * In short, a random value between these two will be the time it takes for the casing to be destroyed.
	 */
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Casing")
	FVector2D DestroyDelayRange = FVector2D(0.5f, 1.0f);

	/**Sound played when the casing is destroyed!
	 * We could have used a surface data for this, but we don't have enough sounds yet.
	 */
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Casing")
	USoundBase* Sound;
	
	/**Amount of time required to pass before destroying this object. Works like object life time.*/
	float DestroyDelay = 1.0f;

	/**Start Implementation.*/
	void OnSpawn_Implementation();
	void ApplyImpulse_Implementation(FVector Direction, const FVector Strength) const;
	/**End Implementation.*/

	/**Tick.*/
	virtual void Tick(float DeltaTime) override;
};