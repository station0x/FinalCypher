//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditor.h"

class FDungeonArchitectThemeEditor;
class SPatternEditorViewport;

class SMGPatternEditorWidget : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SMGPatternEditorWidget) {}
		SLATE_ARGUMENT(TWeakPtr<IDetailsView>, PropertyEditor)
	SLATE_END_ARGS()
	
public:
	/** SCompoundWidget functions */
	void Construct(const FArguments& InArgs, TWeakPtr<FDungeonArchitectThemeEditor> InThemeEditor);
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;

	void ActivateEdMode(TSharedRef<IToolkitHost> InToolkitHost) const;
	void DeactivateEdMode() const;
	void Load(UMarkerGenPattern* InPattern, bool bFocusCameraOnScene = true) const;
	TSharedPtr<IMGPatternEditor> GetEditorImpl() const { return PatternEditor; }
	void OnPatternRuleModified(UMarkerGenPatternRule* InRule);
	void SetEditorImpl(TSharedPtr<IMGPatternEditor> InEditorImpl);

private:
    TWeakPtr<IDetailsView> PropertyEditor;
	TSharedPtr<SPatternEditorViewport> PatternViewport;
	TSharedPtr<IMGPatternEditor> PatternEditor;
	TSharedPtr<FUICommandList> UICommands;
};

