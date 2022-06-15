//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditorMode.h"


#define LOCTEXT_NAMESPACE "DAPatternEditMode"

////////////////////////////// FPatternEditMode //////////////////////////////
const FEditorModeID FMGPatternEditMode::EM_PatternEditor = TEXT("MGPatternEditor");
FMGPatternEditMode::FMGPatternEditMode() {
	bDrawPivot = false;
	bDrawGrid = false;
}

bool FMGPatternEditMode::GetOverrideCursorVisibility(bool& bWantsOverride, bool& bHardwareCursorVisible, bool bSoftwareCursorVisible) const {
	bWantsOverride = true;
	bHardwareCursorVisible = true;
	return true;
}


#undef LOCTEXT_NAMESPACE

