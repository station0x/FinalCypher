//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "WorkflowOrientedApp/ApplicationMode.h"

class UDungeonBuilder;
class FDungeonArchitectThemeEditor;

class FThemeEditorAppModeBase : public FApplicationMode, public FNotifyHook {
public:
	explicit FThemeEditorAppModeBase(TSharedPtr<FDungeonArchitectThemeEditor> InThemeEditor, FName InModeName);
	virtual void Init(TSubclassOf<UDungeonBuilder> InBuilderClass) {}
	virtual void Tick(float DeltaTime) {}
	virtual void SetBuilderClass(TSubclassOf<class UDungeonBuilder> InBuilderClass) {}
	virtual void ShowObjectDetails(UObject* ObjectProperties, bool bForceRefresh = false) const {}

protected:
	TSharedRef<class IDetailsView> CreatePropertyEditorWidget();
	
protected:
	TWeakPtr<FDungeonArchitectThemeEditor> ThemeEditorPtr;
};

