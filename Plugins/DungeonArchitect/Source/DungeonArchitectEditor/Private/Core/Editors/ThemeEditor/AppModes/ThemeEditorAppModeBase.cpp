//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/ThemeEditorAppModeBase.h"

#include "Core/Editors/ThemeEditor/DungeonArchitectThemeEditor.h"

FThemeEditorAppModeBase::FThemeEditorAppModeBase(TSharedPtr<FDungeonArchitectThemeEditor> InThemeEditor, FName InModeName)
	: FApplicationMode(InModeName, FDungeonArchitectThemeEditor::GetLocalizedMode)
	, ThemeEditorPtr(InThemeEditor)
{	
}

TSharedRef<class IDetailsView> FThemeEditorAppModeBase::CreatePropertyEditorWidget() {
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.bUpdatesFromSelection = false;
	DetailsViewArgs.bLockable = false;
	DetailsViewArgs.bAllowSearch = true;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	DetailsViewArgs.NotifyHook = this;
	TSharedRef<IDetailsView> PropertyEditorRef = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	return PropertyEditorRef;
}

