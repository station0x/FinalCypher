//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "EdMode.h"

class FMGPatternEditMode : public FEdMode {
public:
	FMGPatternEditMode();
	virtual bool GetOverrideCursorVisibility(bool& bWantsOverride, bool& bHardwareCursorVisible, bool bSoftwareCursorVisible) const override;
	
public:
	static const FEditorModeID EM_PatternEditor;
	
};

