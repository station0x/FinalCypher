//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPGameInstance.h"
#include "Kismet/GameplayStatics.h"

// void ULPSPGameInstance::Init()
// {
	//Load settings at startup. This makes sure that we can apply them.
	// LoadSettings();
	
	//Base.
	// Super::Init();
// }

// void ULPSPGameInstance::Shutdown()
// {
	//Base.
	// Super::Shutdown();
	
	//Just as a precaution, we save when the game stops.
	// SaveSettings();
// }

// void ULPSPGameInstance::SaveSettings() const
// {	
	//Save.
	// UGameplayStatics::SaveGameToSlot(Settings.Get(), "Settings", 0);
// }

// ULPSPSGSettings* ULPSPGameInstance::LoadSettings()
// {
	//Return if we already have a valid settings object.
	// if(Settings.IsValid())
		// return Settings.Get();
	
	// Load from the settings slot.
	// if(UGameplayStatics::DoesSaveGameExist("Settings", 0))
		// Settings = Cast<ULPSPSGSettings>(UGameplayStatics::LoadGameFromSlot("Settings", 0));
	// else
		// Settings = Cast<ULPSPSGSettings>(UGameplayStatics::CreateSaveGameObject(SettingsClass));

	// Event.
	// OnLoadSettings();
	
	// Return.
	// return Settings.Get();
// }

// void ULPSPGameInstance::ResetSettings()
// {
	//Remove settings and load new ones. This will basically create a new file with default settings.
	// if(UGameplayStatics::DeleteGameInSlot("Settings", 0))
		// LoadSettings();
// }