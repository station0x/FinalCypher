// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Framework/Commands/Commands.h"
#include "CompilerBoosterStyle.h"

class FCompilerBoosterCommands : public TCommands<FCompilerBoosterCommands>
{
public:

	FCompilerBoosterCommands()
		: TCommands<FCompilerBoosterCommands>(TEXT("CompilerBooster"), NSLOCTEXT("Contexts", "CompilerBooster", "CompilerBooster Plugin"), NAME_None, FCompilerBoosterStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > PluginAction;
};
