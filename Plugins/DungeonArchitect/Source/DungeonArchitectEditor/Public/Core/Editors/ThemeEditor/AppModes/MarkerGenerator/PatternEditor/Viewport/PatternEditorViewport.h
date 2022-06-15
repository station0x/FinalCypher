//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "SEditorViewport.h"

class FPatternEditorViewportClient;
class IMGPatternEditor;
class FDungeonArchitectThemeEditor;

class SPatternEditorViewport : public SEditorViewport {
public:
	SLATE_BEGIN_ARGS(SPatternEditorViewport) {}
	SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);
	virtual ~SPatternEditorViewport() override;
	
	/** Set the parent tab of the viewport for determining visibility */
	void SetParentTab(TSharedRef<SDockTab> InParentTab) { ParentTab = InParentTab; }

	// SWidget Interface
	virtual void Tick(const FGeometry& AllottedGeometry, double InCurrentTime, float InDeltaTime) override;
	// End of SWidget Interface

	TSharedPtr<FPatternEditorViewportClient> GetPatternViewportClient() const { return EditorViewportClient; }
	TSharedPtr<FPreviewScene> GetPreviewScene() const { return PreviewScene; }
	
	EVisibility GetToolbarVisibility() const;
	virtual UWorld* GetWorld() const override;

protected:
	/** SEditorViewport interface */
	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;
	virtual EVisibility OnGetViewportContentVisibility() const override;
	virtual void BindCommands() override;
	virtual void OnFocusViewportToSelection() override;

	void OnToggleDebugData();

private:
	/** Determines the visibility of the viewport. */
	virtual bool IsVisible() const override;

private:
	/** The parent tab where this viewport resides */
	TWeakPtr<SDockTab> ParentTab;
	TSharedPtr<FPatternEditorViewportClient> EditorViewportClient;
	TSharedPtr<FPreviewScene> PreviewScene;
};

