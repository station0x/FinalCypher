//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Editor/PatternGraphPinFactory.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNodePin.h"

#include "KismetPins/SGraphPinBool.h"

TSharedPtr<SGraphPin> FMGPatternGraphPinFactory::CreatePin(UEdGraphPin* InPin) const {
	if (InPin->PinType.PinCategory == FMGPatternPinCategory::Bool) {
		return SNew(SGraphPinBool, InPin);
	}

	return nullptr;
}

