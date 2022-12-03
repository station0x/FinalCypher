//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "LPSPSGSettings.generated.h"

UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ULPSPSGSettings final : public USaveGame
{
	GENERATED_BODY()

public:
	/**Returns the value of FieldOfView.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Settings Save Game")
	int GetFieldOfView() const { return FieldOfView; }

	/**Updates the value of FieldOfView.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Settings Save Game")
	void SetFieldOfView(const int Value) { FieldOfView = Value; }

	/**Returns the value of Sensitivity.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Settings Save Game")
	float GetSensitivity() const { return Sensitivity; }

	/**Updates the value of Sensitivity.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Settings Save Game")
	void SetSensitivity(const float Value) { Sensitivity = Value; }

private:
	/**Default Character field of view. We store this in settings so we can modify and save it via the options menu.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Settings Save Game | Values")
	int FieldOfView = 97.0f;

	/**Settings Sensitivity Multiplier.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Settings Save Game | Values")
	float Sensitivity = 0.75f;
};