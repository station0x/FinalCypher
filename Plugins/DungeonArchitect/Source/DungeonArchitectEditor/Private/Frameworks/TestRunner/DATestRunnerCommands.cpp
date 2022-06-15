//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/TestRunner/DATestRunnerCommands.h"

#include "Core/LevelEditor/Customizations/DungeonArchitectStyle.h"

#define LOCTEXT_NAMESPACE "FDATestRunnerCommands"

FDATestRunnerCommands::FDATestRunnerCommands() : TCommands<FDATestRunnerCommands>(
	TEXT("TestRunner"),
	NSLOCTEXT("TestRunner", "TestRunner", "Test Runner"),
	NAME_None,
	FDungeonArchitectStyle::GetStyleSetName())
{
}

void FDATestRunnerCommands::RegisterCommands() {
	UI_COMMAND(StartStopPerfRunner, "Start / Stop", "Start/Stop Performance Test", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE

