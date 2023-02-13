// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "CompilerBoosterCommands.h"

#define LOCTEXT_NAMESPACE "FCompilerBoosterModule"

void FCompilerBoosterCommands::RegisterCommands()
{
	UI_COMMAND(PluginAction, "CompilerBooster", "Execute CompilerBooster action", EUserInterfaceActionType::Button, FInputGesture());
}

#undef LOCTEXT_NAMESPACE
