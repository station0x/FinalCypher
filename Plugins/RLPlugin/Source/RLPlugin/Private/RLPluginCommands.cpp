// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "RLPluginCommands.h"

#define LOCTEXT_NAMESPACE "FRLPluginModule"

void FRLPluginCommands::RegisterCommands()
{
    UI_COMMAND( PluginAction, "CC Setup", "Execute CC Setup action", EUserInterfaceActionType::Button, FInputGesture() );
}

#undef LOCTEXT_NAMESPACE
