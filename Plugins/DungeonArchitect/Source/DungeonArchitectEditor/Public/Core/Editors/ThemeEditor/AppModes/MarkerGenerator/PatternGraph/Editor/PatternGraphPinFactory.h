//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/Widgets/GraphPanelNodeFactory_DungeonProp.h"

class FMGPatternGraphPinFactory : public FGraphPanelPinFactory {
public:
	virtual TSharedPtr<SGraphPin> CreatePin(UEdGraphPin* Pin) const override;
};

