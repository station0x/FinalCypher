//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/DungeonBuilder.h"

class UMarkerGenLayer;
class IMGPatternEditor;

class IMGAppModeEditorInterface {
public:
	virtual ~IMGAppModeEditorInterface() {}
	virtual TSharedPtr<IMGPatternEditor> CreatePatternEditorImpl() = 0;
	virtual UMarkerGenLayer* CreateNewLayer(UObject* InOuter) = 0;
	virtual bool IsLayerCompatible(UMarkerGenLayer* InLayer) = 0;
};

