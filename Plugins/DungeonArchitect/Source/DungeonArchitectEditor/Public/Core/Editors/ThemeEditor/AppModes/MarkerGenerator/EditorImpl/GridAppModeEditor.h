//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/EditorImpl/AppModeEditorInterface.h"

class FMGGridAppModeEditor : public IMGAppModeEditorInterface {
public:
	virtual TSharedPtr<IMGPatternEditor> CreatePatternEditorImpl() override;
	virtual UMarkerGenLayer* CreateNewLayer(UObject* InOuter) override;
	virtual bool IsLayerCompatible(UMarkerGenLayer* InLayer) override;
};

