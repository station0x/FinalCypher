//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPSGSettings.h"
#include "LPSPGameInstance.generated.h"

/**
 *Base LPSP Game Instance class.
 *The actual Game Instance used is likely to be a child blueprint of this class.
 */
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ULPSPGameInstance final : public UGameInstance
{
	GENERATED_BODY()
	
public:
	/**Init.*/
	// virtual void Init() override;

	/**Shutdown.*/
	// virtual void Shutdown() override;

	/**Saves the game settings.*/
	// UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Game Instance", meta = (DisplayName = "Settings Object Save"))
	// void SaveSettings() const;

	/**Loads the game settings.*/
	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Low Poly Shooter Pack | Game Instance", meta = (DisplayName = "Settings Object Load"))
	// ULPSPSGSettings* LoadSettings();

	/**Resets the game settings.*/
	// UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Game Instance", meta = (DisplayName = "Settings Object Reset"))
	// void ResetSettings();

	/**Returns true if the settings are valid.*/
	// UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Low Poly Shooter Pack | Game Instance")
	// bool HasSettings() const { return Settings.IsValid(); }

	/**Called when the new settings are loaded.*/
	// UFUNCTION(BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Game Instance")
	// void OnLoadSettings();

private:
	/**Settings object.*/
	// UPROPERTY()
	// TWeakObjectPtr<ULPSPSGSettings> Settings;

	/**Class of the save game object. Mostly exposed so we can assign the blueprint version of the class.*/
	// UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Game Instance", meta = (DisplayName = "Settings Object Class"))
	// TSubclassOf<ULPSPSGSettings> SettingsClass;
};
