//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPAttachment.h"
#include "LPSPLagValues.h"

#include "LPSPScope.generated.h"

/**
 * Base class used for all scopes.
 * Contains different modifiers that each scope can have, that change gameplay.
 */
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPScope final : public ALPSPAttachment
{
	GENERATED_BODY()

public:
	/**Returns the value of AimOffset.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Scope")
	FLPSPOffset GetAimOffset() const { return AimOffset; }

	/**Returns the value of FieldOfViewMultiplier.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Scope")
	float GetFieldOfViewMultiplier() const { return FieldOfViewMultiplier; }
	
	/**Returns the value of YawSensitivityMultiplier.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Scope")
	float GetYawSensitivityMultiplier() const { return YawSensitivityMultiplier; }

	/**Returns the value of PitchSensitivityMultiplier.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Scope")
	float GetPitchSensitivityMultiplier() const { return PitchSensitivityMultiplier; }
	
	/**Returns the value of SpreadMultiplier.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Scope")
	float GetSpreadMultiplier() const { return SpreadMultiplier; }

	/**Returns the value of LagMultiplier.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Scope")
	FLPSPLagValues GetLagMultiplier() const { return LagMultiplier; }

	/**Returns the value of AimSpeed.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Scope")
	float GetAimSpeedMultiplier() const { return AimSpeedMultiplier; }

protected:
	/**Begin Play.*/
	virtual void BeginPlay() override;

private:
	/**Offset applied to the weapon using this scope while aiming.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Adjustments", meta = (DisplayName = "Offset Aim"))
	FLPSPOffset AimOffset;
	
	/**Value that we multiply the field of view by when the player aims through this scope.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Adjustments", meta = (DisplayName = "Multiplier Field Of View"))
	float FieldOfViewMultiplier = 0.9f;

	/**Yaw sensitivity multiplier. Changes how fast the camera rotates left and right while aiming with this scope.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Adjustments", meta = (DisplayName = "Multiplier Sensitivity Yaw"))
	float YawSensitivityMultiplier = 0.8f;
	
	/**Pitch sensitivity multiplier. Changes how fast the camera rotates up and down while aiming with this scope.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Adjustments", meta = (DisplayName = "Multiplier Sensitivity Pitch"))
	float PitchSensitivityMultiplier = 0.8f;
	
	/**Value multiplied by the weapon's default spread while aiming. Generally used to reduce/remove spread.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Adjustments", meta = (DisplayName = "Multiplier Spread"))
	float SpreadMultiplier = 0.0f;

	/**Speed at which this scope is aimed. (Default is One)*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Adjustments", meta = (DisplayName = "Multiplier Aiming Speed"))
	float AimSpeedMultiplier = 1.0f;

	/**Value multiplied by the weapon's default aiming lag values. (Only applied while aiming)*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Adjustments", meta = (DisplayName = "Multiplier Aiming Lag"))
	FLPSPLagValues LagMultiplier;

	/**
	 *Should we override the sight material with custom settings?
	 *Useful for updating parameters while in-game!
	 */
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Sight", meta = (DisplayName = "Sight Material Override"))
	bool bOverrideSightMaterial = false;

	/**Texture used for the scope's sight.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Sight", meta = (DisplayName = "Sight Texture", EditCondition = "bOverrideSightMaterial"))
	UTexture* SightTexture;
	
	/**Color to tint the scope's sight with.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Sight", meta = (DisplayName = "Sight Tint", EditCondition = "bOverrideSightMaterial"))
	FLinearColor SightTint = FLinearColor::Red;

	/**Size of the sight texture.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Sight", meta = (DisplayName = "Sight Scale", EditCondition = "bOverrideSightMaterial"))
	float SightScale = 10.0f;
	
	/**Depth value used on the scope's parallax effect.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Sight", meta = (DisplayName = "Sight Depth", EditCondition = "bOverrideSightMaterial"))
	float SightDepth = 1500.0f;
	
	/**Emission multiplier for the sight material.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Scope | Sight", meta = (DisplayName = "Sight Emission", EditCondition = "bOverrideSightMaterial"))
	float SightEmission = 3.0f;
};