//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppModeCommands.h"

#include "Core/LevelEditor/Customizations/DungeonArchitectStyle.h"

#define LOCTEXT_NAMESPACE "FMarkerGeneratorAppModeCommands"

/////////////////////////////// FMarkerGeneratorAppModeCommands ///////////////////////////////
FMarkerGeneratorAppModeCommands::FMarkerGeneratorAppModeCommands()
	: TCommands<FMarkerGeneratorAppModeCommands>(
		TEXT("DAMarkerGen"),
		LOCTEXT("ContextDesc", "Marker Generator"),
		NAME_None,
		FDungeonArchitectStyle::GetStyleSetName())
{
}

void FMarkerGeneratorAppModeCommands::RegisterCommands() {
	UI_COMMAND(Build, "Build", "Build the pattern model", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(AddRule, "Add New Rule", "Add a new Rule Block to the viewport", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(DeleteRule, "Delete Rule", "Add a new Rule Block to the viewport", EUserInterfaceActionType::Button, FInputChord());
	
	UI_COMMAND(CopyRule, "Copy Rule", "Copy the rule", EUserInterfaceActionType::Button, FInputChord());
	UI_COMMAND(PasteRule, "Paste Rule", "Paste a previously copied rule in this location", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

