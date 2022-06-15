//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Viewport/PatternEditorViewportClient.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditor.h"
#include "Core/Editors/ThemeEditor/DungeonArchitectThemeEditor.h"

#include "EditorModeManager.h"
#include "EngineUtils.h"

DEFINE_LOG_CATEGORY_STATIC(LogPatternVPClient, Log, All);

FPatternEditorViewportClient::FPatternEditorViewportClient(FPreviewScene& InPreviewScene, TSharedPtr<SEditorViewport> InViewportWidget)
	: FEditorViewportClient(nullptr, &InPreviewScene, InViewportWidget)
{
	static constexpr float GridSize = 2048.0f;
	static constexpr int32 CellSize = 16;
	
	// Setup defaults for the common draw helper.
	DrawHelper.bDrawPivot = false;
	DrawHelper.bDrawWorldBox = false;
	DrawHelper.bDrawKillZ = false;
	DrawHelper.bDrawGrid = false;
	DrawHelper.GridColorAxis = FColor(160, 160, 160);
	DrawHelper.GridColorMajor = FColor(144, 144, 144);
	DrawHelper.GridColorMinor = FColor(128, 128, 128);
	DrawHelper.PerspectiveGridSize = GridSize;
	DrawHelper.NumCells = DrawHelper.PerspectiveGridSize / (CellSize * 2);
	
	FEditorViewportClient::SetViewMode(VMI_Lit);

	//EngineShowFlags.DisableAdvancedFeatures();
	EngineShowFlags.SetSnap(false);
	EngineShowFlags.SetGrid(false);
	EngineShowFlags.CompositeEditorPrimitives = true;
	OverrideNearClipPlane(1.0f);
	bUsingOrbitCamera = true;

	// Set the initial camera position
	FRotator OrbitRotation(-40, 200, 0);
	SetCameraSetup(
		FVector::ZeroVector,
		OrbitRotation,
		FVector(0.0f, 100, 0.0f),
		FVector::ZeroVector,
		FVector(300, 400, 500),
		FRotator(-10, 0, 0)
	);
	SetViewLocation(FVector(500, 300, 500));
	//SetLookAtLocation(FVector(0, 0, 0));

	SetRequiredCursor(true, true);
}

void FPatternEditorViewportClient::SetInputInterface(TWeakPtr<IMGPatternEditorInputInterface> InInputInterface) {
	InputInterface = InInputInterface;
}

void FPatternEditorViewportClient::Tick(float DeltaSeconds) {
	FEditorViewportClient::Tick(DeltaSeconds);

	// Tick the preview scene world.
	if (!GIntraFrameDebuggingGameThread) {
		PreviewScene->GetWorld()->Tick(LEVELTICK_All, DeltaSeconds);
	}
}

void FPatternEditorViewportClient::UpdateMouseDelta() {
	if (bDragging) {
		return;
	}
	
	FEditorViewportClient::UpdateMouseDelta();
}

bool FPatternEditorViewportClient::InputKey(FViewport* InViewport, int32 InControllerId, FKey InKey, EInputEvent InEvent, float InAmountDepressed, bool bInGamepad) {
	if (InKey == EKeys::LeftMouseButton) {
		bDragging = (InEvent == IE_Pressed);
	}
	
	const TSharedPtr<IMGPatternEditorInputInterface> Input = InputInterface.Pin();
	if (Input.IsValid()) {
		if (Input->InputKey(this, InViewport, InControllerId, InKey, InEvent, InAmountDepressed, bInGamepad)) {
			return true;
		}
	}
	
	return FEditorViewportClient::InputKey(InViewport, InControllerId, InKey, InEvent, InAmountDepressed, bInGamepad);
}

void FPatternEditorViewportClient::CapturedMouseMove(FViewport* InViewport, int32 InMouseX, int32 InMouseY) {
	FEditorViewportClient::CapturedMouseMove(InViewport, InMouseX, InMouseY);
	
	//UpdateCursor();
	
	const TSharedPtr<IMGPatternEditorInputInterface> Input = InputInterface.Pin();
	if (Input.IsValid()) {
		Input->CapturedMouseMove(this, InViewport, InMouseX, InMouseY);
	}
}

void FPatternEditorViewportClient::TrackingStarted(const FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge) {
	const TSharedPtr<IMGPatternEditorInputInterface> Input = InputInterface.Pin();
	if (Input.IsValid()) {
		Input->TrackingStarted(this, InInputState, bIsDraggingWidget, bNudge);
	}
}

void FPatternEditorViewportClient::TrackingStopped() {
	const TSharedPtr<IMGPatternEditorInputInterface> Input = InputInterface.Pin();
	if (Input.IsValid()) {
		Input->TrackingStopped(this);
	}
}

void FPatternEditorViewportClient::ProcessClick(FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) {
	FEditorViewportClient::ProcessClick(View, HitProxy, Key, Event, HitX, HitY);

	const TSharedPtr<IMGPatternEditorInputInterface> Input = InputInterface.Pin();
	if (Input.IsValid()) {
		Input->ProcessClick(this, View, HitProxy, Key, Event, HitX, HitY);
	}
}

void FPatternEditorViewportClient::CheckHoveredHitProxy(HHitProxy* HoveredHitProxy) {
	FEditorViewportClient::CheckHoveredHitProxy(HoveredHitProxy);

	const TSharedPtr<IMGPatternEditorInputInterface> Input = InputInterface.Pin();
	if (Input.IsValid()) {
		Input->CheckHoveredHitProxy(this, HoveredHitProxy);
	}
	
	HoveredActor = nullptr;
	if (HoveredHitProxy && HoveredHitProxy->IsA(HActor::StaticGetType())) {
		const HActor* ActorHit = static_cast<HActor*>(HoveredHitProxy);
		HoveredActor = ActorHit->Actor;
		if (HoveredActor.IsValid() && HoveredActor->IsChildActor())
		{
			HoveredActor = HoveredActor->GetParentActor();
		}
	}
}


