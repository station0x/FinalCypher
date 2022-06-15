//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/EditorImpl/GridAppModeEditor.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Impl/Grid/GridPatternEditor.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridLayer.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"

TSharedPtr<IMGPatternEditor> FMGGridAppModeEditor::CreatePatternEditorImpl() {
	return MakeShareable(new FMGGridPatternEditor);
}

UMarkerGenLayer* FMGGridAppModeEditor::CreateNewLayer(UObject* InOuter) {
	UMarkerGenGridLayer* NewLayer = NewObject<UMarkerGenGridLayer>(InOuter);
	UMarkerGenGridPattern* GridPattern = NewObject<UMarkerGenGridPattern>(NewLayer);
	NewLayer->Pattern = GridPattern;

	return NewLayer;
}

bool FMGGridAppModeEditor::IsLayerCompatible(UMarkerGenLayer* InLayer) {
	return InLayer && InLayer->IsA<UMarkerGenGridLayer>();
}

