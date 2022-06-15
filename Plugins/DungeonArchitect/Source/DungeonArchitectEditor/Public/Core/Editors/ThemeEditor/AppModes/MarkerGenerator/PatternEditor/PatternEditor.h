//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatterEditorInputInterface.h"

class SPatternEditorViewport;
class UMarkerGenPattern;
class FPatternEditorViewportClient;
class UMarkerGenPatternRule;

DECLARE_DELEGATE_OneParam(FMGPatternRuleDelegate, UMarkerGenPatternRule*);

class IMGPatternEditor : public IMGPatternEditorInputInterface, public TSharedFromThis<IMGPatternEditor> {
public:
	void Initialize(TSharedPtr<SPatternEditorViewport> InEditorWidget, const TSharedRef<FUICommandList> InCommandList);
	virtual ~IMGPatternEditor() {}

	virtual void Tick(FPatternEditorViewportClient* InViewportClient, float DeltaSeconds) {}
	virtual void InvalidateRuleVisuals(const UMarkerGenPatternRule* InRule) {}
	virtual void FocusCameraOnScene() const {}
	virtual void RefreshRuleActor(UMarkerGenPatternRule* InRule) {}
	virtual void Load(UMarkerGenPattern* InPattern) = 0;

	virtual void TrackingStarted(FPatternEditorViewportClient* InViewportClient, const FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge) override;
	virtual void TrackingStopped(FPatternEditorViewportClient* InViewportClient) override;

	FMGPatternRuleDelegate& GetOnRuleSelectionChanged() { return OnRuleSelectionChanged; }
	FMGPatternRuleDelegate& GetOnRulePositionChanged() { return OnRulePositionChanged; }

protected:
	virtual void SetupScene() = 0;
	virtual void BindCommands();

	virtual void HandleAddNewRule() {}
	virtual bool CanAddNewRule() const { return false; }
	virtual void HandleDeleteRule() {}
	virtual bool CanDeleteRule() const { return false; }
	virtual void HandleCopyRule() {}
	virtual bool CanCopyRule() const { return false; }
	virtual void HandlePasteRule() {}
	virtual bool CanPasteRule() const { return false; }

	void OpenViewportMenu(TFunction<void(FMenuBuilder& InMenuBuilder)> InBuildMenu) const;
	static FRay GetRayFromMousePos(FEditorViewportClient* ViewportClient, int MouseX, int MouseY);

protected:
	bool bTracking = false;
	TWeakPtr<SPatternEditorViewport> EditorWidgetPtr;
	TSharedPtr<FUICommandList> CommandList;
	TWeakPtr<FPreviewScene> PreviewScenePtr;
	FMGPatternRuleDelegate OnRuleSelectionChanged;
	FMGPatternRuleDelegate OnRulePositionChanged;
};

typedef TSharedPtr<IMGPatternEditor> IMGPatternEditorPtr;

