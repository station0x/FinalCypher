//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class IMGPatternEditorInputInterface;
class IMGPatternEditor;
class FDungeonArchitectThemeEditor;

class FPatternEditorViewportClient : public FEditorViewportClient , public TSharedFromThis<FPatternEditorViewportClient>
{
public:
	FPatternEditorViewportClient(FPreviewScene& InPreviewScene, TSharedPtr<SEditorViewport> InViewportWidget);
    void SetInputInterface(TWeakPtr<IMGPatternEditorInputInterface> InInputInterface);
	
	// FEditorViewportClient interface
	virtual void Tick(float DeltaSeconds) override;
	virtual void UpdateMouseDelta() override;
	virtual bool InputKey(FViewport* Viewport, int32 ControllerId, FKey Key, EInputEvent Event, float AmountDepressed, bool bGamepad) override;
	virtual void CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) override;
	virtual void TrackingStarted(const FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge) override;
	virtual void TrackingStopped() override;
	virtual void ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) override;
	virtual void CheckHoveredHitProxy(HHitProxy* HoveredHitProxy) override;
	// End of FEditorViewportClient interface

private:
	TWeakPtr<IMGPatternEditorInputInterface> InputInterface;
	bool bDragging = false;
	FRay TrackingRay;
	TWeakObjectPtr<AActor> HoveredActor;
};

