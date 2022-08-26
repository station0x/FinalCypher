// Copyright Sam Bonifacio. All Rights Reserved.

#pragma once

#include "PlayerInputMappings.h"
#include "Misc/AutoSettingsInputConfig.h"
#include "Subsystems/EngineSubsystem.h"
#include "InputMappingManager.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMappingsChanged, APlayerController*, Player);

/**
 * Manages input mapping for players
 */
UCLASS(Config = Input)
class AUTOSETTINGSINPUT_API UInputMappingManager : public UEngineSubsystem
{
	GENERATED_BODY()
	
public:

	UInputMappingManager();

	// Fired when any local player's input mappings are updated
	UPROPERTY(BlueprintAssignable)
	FOnMappingsChanged OnMappingsChanged;

	// Get subsystem instance
	static UInputMappingManager* Get();

	// Get input presets defined in project settings (AutoSettings page)
	UFUNCTION(BlueprintPure, Category = "Input Mapping")
	static TArray<FInputMappingPreset> GetDefaultInputPresets();

	// Get input mappings for player
	// Static version
	UFUNCTION(BlueprintPure, Category = "Input Mapping", DisplayName = "Get Player Input Mappings")
	static FPlayerInputMappings GetPlayerInputMappingsStatic(APlayerController* Player);

	// Set a player's input mapping preset
	// Static version
	UFUNCTION(BlueprintCallable, Category = "Input Mapping", DisplayName = "Set Player Input Preset")
	static void SetPlayerInputPresetStatic(APlayerController* Player, FInputMappingPreset Preset);

	// Set a player's input mapping preset by tag
	// Presets and tags are defined in project settings (AutoSettings page)
	// Static version
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	static void SetPlayerInputPresetByTag(APlayerController* Player, FGameplayTag PresetTag);

	// Set key group used to retrieve active mappings for a player
	// Useful for globally switching prompts between key groups when for example input device is changed
	UFUNCTION(BlueprintCallable, Category = "Input Mapping", DisplayName = "Set Player Input Key Group")
	static void SetPlayerKeyGroupStatic(APlayerController* Player, FGameplayTag KeyGroup);

	// Override a player's action mapping on the given mapping group and save to config
	// @param bAnyKeyGroup If true will replace existing mappings on any key group instead of just the key group of the mapping specified.
	UFUNCTION(BlueprintCallable, Category = "Input Mapping", DisplayName = "Add Player Input Action Override")
	static void AddPlayerActionOverrideStatic(APlayerController* Player, const FInputActionKeyMapping& NewMapping, int32 MappingGroup, bool bAnyKeyGroup = false);

	// Override a player's axis mapping on the given mapping group
	// @param bAnyKeyGroup If true will replace existing mappings on any key group instead of just the key group of the mapping specified.
	UFUNCTION(BlueprintCallable, Category = "Input Mapping", DisplayName = "Add Player Input Axis Override")
	static void AddPlayerAxisOverrideStatic(APlayerController* Player, const FInputAxisKeyMapping& NewMapping, int32 MappingGroup, bool bAnyKeyGroup = false);

	// Initialize a player's input overrides manually
	// This is what sets the saved input mappings on the Player Controller itself
	// If Auto Initialize Player Input Overrides is true in project settings, this is called automatically
	UFUNCTION(BlueprintCallable, Category = "Input Mapping", DisplayName = "Initialize Player Input Overrides")
	static bool InitializePlayerInputOverridesStatic(APlayerController* Player);

	// Returns the mapping used for the given action
	// @param MappingGroup Mapping group index - if an input has multiple mappings, this will determine which one to use. A value of -1 will use the first mapping group available.
	UFUNCTION(BlueprintPure, Category = "Input Mapping", Displayname = "Get Player Action Mapping")
	static FInputActionKeyMapping GetPlayerActionMappingStatic(APlayerController* Player, FName ActionName, int32 MappingGroup = -1);

	// Returns the mapping used for the given axis and scale
	// @param MappingGroup Mapping group index - if an input has multiple mappings, this will determine which one to use. A value of -1 will use the first mapping group available.
	UFUNCTION(BlueprintPure, Category = "Input Mapping", Displayname = "Get Player Axis Mapping")
	static FInputAxisKeyMapping GetPlayerAxisMappingStatic(APlayerController* Player, FName AxisName, float Scale, int32 MappingGroup = -1);

	static const UAutoSettingsInputConfig* GetInputConfigStatic();

	// Dumps registered player controllers to log with their input mappings
	static void DumpPlayers();
	static void TestLayoutMerge();

	void SetPlayerKeyGroup(APlayerController* Player, FGameplayTag KeyGroup);

	// Override a player's action mapping on the given mapping group and save to config
	// @param	bAnyKeyGroup	If true will replace existing mappings on any key group instead of just the key group of the mapping specified.
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	void AddPlayerActionOverride(APlayerController* Player, const FInputActionKeyMapping& NewMapping, int32 MappingGroup, bool bAnyKeyGroup = false);

	// Override a player's axis mapping on the given mapping group
	// @param	bAnyKeyGroup	If true will replace existing mappings on any key group instead of just the key group of the mapping specified.
	UFUNCTION(BlueprintCallable, Category = "Input Mapping")
	void AddPlayerAxisOverride(APlayerController* Player, const FInputAxisKeyMapping& NewMapping, int32 MappingGroup, bool bAnyKeyGroup = false);

	// Returns the mapping used for the given action
	// @param MappingGroup Mapping group index - if an input has multiple mappings, this will determine which one to use. A value of -1 will use the first mapping group available.
	UFUNCTION(BlueprintPure, Category = "Input Mapping")
	FInputActionKeyMapping GetPlayerActionMapping(APlayerController* Player, FName ActionName, int32 MappingGroup, FGameplayTag KeyGroup, bool bUsePlayerKeyGroup = false ) const;

	// Returns the mapping used for the given axis and scale
	// @param MappingGroup Mapping group index - if an input has multiple mappings, this will determine which one to use. A value of -1 will use the first mapping group available.
	UFUNCTION(BlueprintPure, Category = "Input Mapping")
	FInputAxisKeyMapping GetPlayerAxisMapping(APlayerController* Player, FName AxisName, float Scale, int32 MappingGroup, FGameplayTag KeyGroup, bool bUsePlayerKeyGroup = false) const;

	// Returns the mappings used for the given action
	// @param MappingGroup Mapping group index - if an input has multiple mappings, this will determine which one to use. A value of -1 will search all mapping groups.
	UFUNCTION(BlueprintPure, Category = "Input Mapping")
	TArray<FInputActionKeyMapping> GetPlayerActionMappings(APlayerController* Player, FName ActionName, int32 MappingGroup, FGameplayTag KeyGroup, bool bUsePlayerKeyGroup = false ) const;

	// Returns the mappings used for the given axis and scale
	// @param MappingGroup Mapping group index - if an input has multiple mappings, this will determine which one to use. A value of -1 will search all mapping groups.
	UFUNCTION(BlueprintPure, Category = "Input Mapping")
	TArray<FInputAxisKeyMapping> GetPlayerAxisMappings(APlayerController* Player, FName AxisName, float Scale, int32 MappingGroup, FGameplayTag KeyGroup, bool bUsePlayerKeyGroup = false) const;

	// Finds any mappings that use the specified Key
	UFUNCTION(BlueprintPure, Category = "Input Mapping")
	void GetPlayerMappingsByKey(APlayerController* Player, FKey Key, TArray<FInputActionKeyMapping>& Actions, TArray<FInputAxisKeyMapping>& Axes ) const;

	// Set a player's input mapping preset
	void SetPlayerInputPreset(APlayerController* Player, FInputMappingPreset Preset);

	// Set a player's input mapping preset by tag
	// Presets and tags are defined in project settings (AutoSettings page)
	void SetPlayerInputPreset(APlayerController* Player, FGameplayTag PresetTag);

	bool IsPlayerControllerRegistered(APlayerController* PlayerController) const { return RegisteredPlayerControllers.Contains(PlayerController); }

	const UAutoSettingsInputConfig* GetInputConfig() const;

protected:

	virtual void PostInitProperties() override;

private:

	UPROPERTY(Config)
	TArray<FPlayerInputMappings> PlayerInputOverrides;

	UPROPERTY()
	TArray<APlayerController*> RegisteredPlayerControllers;

	UWorld* GetGameWorld() const;

	void RegisterPlayerController(APlayerController* Player);

	FPlayerInputMappings FindPlayerInputMappings(APlayerController* Player) const;
	FPlayerInputMappings FindPlayerInputMappingsOrDefault(APlayerController* Player) const;

	void SavePlayerInputMappings(APlayerController* Player, FPlayerInputMappings& Mappings);

	UFUNCTION()
	void OnRegisteredPlayerControllerDestroyed(AActor* DestroyedActor);	
};
