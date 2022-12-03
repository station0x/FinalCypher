//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LPSPRepresentation.generated.h"

/**A Representation is an actor that can easily have it's properties changed at runtime.
 *The class itself mostly contains helper functions.
 */
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPRepresentation : public AActor
{
	GENERATED_BODY()

public:
	//Constructor.
	ALPSPRepresentation();

	/**On Construction.*/
	virtual void OnConstruction(const FTransform& Transform) override;

	/**Tries to change the value of bEnabled.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Representation")
	void TrySetEnabled(const bool bValue);

	/**Returns the value of bEnabled.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Representation")
	bool IsEnabled() const { return bEnabled; }

	/**Changes the value of bEnabled.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Representation")
	void SetEnabled(const bool bValue);

protected:
	/**Start Implementations.*/
	virtual void OnEnable_Implementation();
	virtual void OnDisable_Implementation();
	/**End Implementations.*/
	
	/**Called when this representation gets enabled.*/
	UFUNCTION(BlueprintNativeEvent)
	void OnEnable();

	/**Called when this representation gets disabled.*/
	UFUNCTION(BlueprintNativeEvent)
	void OnDisable();
	
private:
	/**
	 * When enabled, the representation will use its settings to determine the different properties to set for its components.
	 * When disabled, the representation becomes completely inactive (Invisible, No Collision, No Physics, No Tick).
	 * Beware that representations that are not enabled will not have recursive (or otherwise) changes applied to their properties.
	 */
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Representation")
	bool bEnabled = true;

	/**Controls whether this representation is allowed to tick or not.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Representation", meta = (EditCondition = "bEnabled"))
	bool bTicks;
	
	/**Allows us to only see this representation in the editor, and not in the actual game.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Representation", meta = (EditCondition = "bEnabled"))
	bool bHiddenInGame;
};