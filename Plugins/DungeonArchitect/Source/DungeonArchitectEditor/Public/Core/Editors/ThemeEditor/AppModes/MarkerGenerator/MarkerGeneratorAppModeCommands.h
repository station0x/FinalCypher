//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class DUNGEONARCHITECTEDITOR_API FMarkerGeneratorAppModeCommands : public TCommands<FMarkerGeneratorAppModeCommands> {
public:
	FMarkerGeneratorAppModeCommands();
	virtual void RegisterCommands() override;

public:
	TSharedPtr<FUICommandInfo> Build;
	TSharedPtr<FUICommandInfo> AddRule;
	TSharedPtr<FUICommandInfo> CopyRule;
	TSharedPtr<FUICommandInfo> PasteRule;
	TSharedPtr<FUICommandInfo> DeleteRule;
};


