//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class DUNGEONARCHITECTEDITOR_API FDATestRunnerCommands : public TCommands<FDATestRunnerCommands> {
public:
	FDATestRunnerCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> StartStopPerfRunner;
};

