//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/DungeonArchitectThemeEditor.h"

#include "Widgets/SNullWidget.h"
#include "WorkflowOrientedApp/WorkflowTabFactory.h"

#define DEFINE_THEME_EDITOR_TAB_FACTORY_BASE(NAME, CAPTION, ICON, TOOLTIP_MENU, TOOLTIP_TAB)			\
class FThemeEditorTabFactory_##NAME : public FWorkflowTabFactory {	\
public:	\
	FThemeEditorTabFactory_##NAME(TSharedPtr<class FDungeonArchitectThemeEditor> InThemeEditor, TSharedPtr<SWidget> InWidget)		\
		: FWorkflowTabFactory(TabID_##NAME, InThemeEditor)		\
		, WidgetPtr(InWidget)		\
	{		\
		bIsSingleton = true;		\
		TabLabel = CAPTION;		\
		TabIcon = FSlateIcon(FEditorStyle::GetStyleSetName(), ICON);		\
		ViewMenuDescription = CAPTION;		\
		ViewMenuTooltip = TOOLTIP_MENU;		\
	}		\
	FThemeEditorTabFactory_##NAME(TSharedPtr<class FDungeonArchitectThemeEditor> InThemeEditor)	: FThemeEditorTabFactory_##NAME(InThemeEditor, SNullWidget::NullWidget)	{}	\
	virtual TSharedRef<SWidget> CreateTabBody(const FWorkflowTabSpawnInfo& Info) const override		\
	{		\
		check (WidgetPtr.IsValid());	\
		return WidgetPtr.Pin().ToSharedRef();		\
	}		\
	virtual FText GetTabToolTipText(const FWorkflowTabSpawnInfo& Info) const override { return TOOLTIP_TAB; }	\
	TSharedRef<SDockTab> SpawnTab(const FWorkflowTabSpawnInfo& Info) const; \
protected:	\
	TWeakPtr<SWidget> WidgetPtr;	\
};


#define DEFINE_THEME_EDITOR_TAB_FACTORY(NAME, CAPTION, ICON, TOOLTIP_MENU, TOOLTIP_TAB)	\
	DEFINE_THEME_EDITOR_TAB_FACTORY_BASE(NAME, CAPTION, ICON, TOOLTIP_MENU, TOOLTIP_TAB)	\
	TSharedRef<SDockTab> FThemeEditorTabFactory_##NAME::SpawnTab(const FWorkflowTabSpawnInfo& Info) const { return FWorkflowTabFactory::SpawnTab(Info); }


#define DEFINE_THEME_EDITOR_TAB_FACTORY_CUSTOM_SPAWNER(NAME, CAPTION, ICON, TOOLTIP_MENU, TOOLTIP_TAB)	\
	DEFINE_THEME_EDITOR_TAB_FACTORY_BASE(NAME, CAPTION, ICON, TOOLTIP_MENU, TOOLTIP_TAB)

