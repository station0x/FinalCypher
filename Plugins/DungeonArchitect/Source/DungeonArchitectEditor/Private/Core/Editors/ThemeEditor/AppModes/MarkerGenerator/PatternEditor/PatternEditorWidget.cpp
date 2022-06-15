//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditorWidget.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditorMode.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Viewport/PatternEditorViewport.h"
#include "Core/Editors/ThemeEditor/DungeonArchitectThemeEditor.h"

#include "EditorModeManager.h"

#define LOCTEXT_NAMESPACE "SMGPatternEditor"

void SMGPatternEditorWidget::Construct(const FArguments& InArgs, TWeakPtr<FDungeonArchitectThemeEditor> InThemeEditor) {
	PropertyEditor = InArgs._PropertyEditor;
	PatternViewport = SNew(SPatternEditorViewport);
	
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = InThemeEditor.Pin();
	UICommands = ThemeEditor->GetToolkitCommands();

	ChildSlot
	[
		PatternViewport.ToSharedRef()
	];
}

void SMGPatternEditorWidget::SetEditorImpl(TSharedPtr<IMGPatternEditor> InEditorImpl) {
	PatternEditor = InEditorImpl;
	if (PatternEditor.IsValid()) {
		PatternEditor->Initialize(PatternViewport, UICommands.ToSharedRef());
	}
}

void SMGPatternEditorWidget::Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) {
	SCompoundWidget::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);

	if (PatternEditor.IsValid()) {
		PatternEditor->Tick(PatternViewport->GetPatternViewportClient().Get(), InDeltaTime);
	}
}

void SMGPatternEditorWidget::ActivateEdMode(TSharedRef<IToolkitHost> InToolkitHost) const {
	FEditorModeTools* ModeTools = PatternViewport->GetViewportClient()->GetModeTools();
	ModeTools->SetToolkitHost(InToolkitHost);
	ModeTools->SetDefaultMode(FMGPatternEditMode::EM_PatternEditor);
	ModeTools->ActivateDefaultMode();
}

void SMGPatternEditorWidget::DeactivateEdMode() const {
	FEditorModeTools* ModeTools = PatternViewport->GetViewportClient()->GetModeTools();
	ModeTools->DeactivateAllModes();
}

void SMGPatternEditorWidget::Load(UMarkerGenPattern* InPattern, bool bFocusCameraOnScene) const {
	if (PatternEditor.IsValid()) {
		PatternEditor->Load(InPattern);

		if (bFocusCameraOnScene) {
			PatternEditor->FocusCameraOnScene();
		}
	}
}

void SMGPatternEditorWidget::OnPatternRuleModified(UMarkerGenPatternRule* InRule) {
	if (PatternEditor.IsValid()) {
		PatternEditor->RefreshRuleActor(InRule);
	}
}

#undef LOCTEXT_NAMESPACE

