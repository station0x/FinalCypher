//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditor.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppModeCommands.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Viewport/PatternEditorViewport.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Viewport/PatternEditorViewportClient.h"

void IMGPatternEditor::Initialize(TSharedPtr<SPatternEditorViewport> InEditorWidget, const TSharedRef<FUICommandList> InCommandList) {
	check(InEditorWidget.IsValid());
	InEditorWidget->GetPatternViewportClient()->SetInputInterface(SharedThis(this));
	EditorWidgetPtr = InEditorWidget;
	PreviewScenePtr = InEditorWidget->GetPreviewScene();
	CommandList = InCommandList; 
	SetupScene();
	BindCommands();
}

void IMGPatternEditor::TrackingStarted(FPatternEditorViewportClient* InViewportClient, const FInputEventState& InInputState, bool bIsDraggingWidget, bool bNudge) {
	bTracking = true;
}

void IMGPatternEditor::TrackingStopped(FPatternEditorViewportClient* InViewportClient) {
	bTracking = false;
}

void IMGPatternEditor::BindCommands() {
	const FMarkerGeneratorAppModeCommands& Commands = FMarkerGeneratorAppModeCommands::Get();

	CommandList->MapAction(
		Commands.AddRule,
		FExecuteAction::CreateRaw(this, &IMGPatternEditor::HandleAddNewRule),
		FCanExecuteAction::CreateRaw(this, &IMGPatternEditor::CanAddNewRule));

	CommandList->MapAction(
		Commands.DeleteRule,
		FExecuteAction::CreateRaw(this, &IMGPatternEditor::HandleDeleteRule),
		FCanExecuteAction::CreateRaw(this, &IMGPatternEditor::CanDeleteRule));

	CommandList->MapAction(
		Commands.CopyRule,
		FExecuteAction::CreateRaw(this, &IMGPatternEditor::HandleCopyRule),
		FCanExecuteAction::CreateRaw(this, &IMGPatternEditor::CanCopyRule));

	CommandList->MapAction(
		Commands.PasteRule,
		FExecuteAction::CreateRaw(this, &IMGPatternEditor::HandlePasteRule),
		FCanExecuteAction::CreateRaw(this, &IMGPatternEditor::CanPasteRule));


}

void IMGPatternEditor::OpenViewportMenu(TFunction<void(FMenuBuilder& InMenuBuilder)> InBuildMenu) const {
	FMenuBuilder MenuBuilder(true, CommandList);

	InBuildMenu(MenuBuilder);

	const TSharedPtr<SWidget> MenuWidget = MenuBuilder.MakeWidget();
	const TSharedPtr<SWidget> ParentWidget = EditorWidgetPtr.Pin();

	if (MenuWidget.IsValid() && ParentWidget.IsValid())
	{
		const FVector2D MouseCursorLocation = FSlateApplication::Get().GetCursorPos();

		FSlateApplication::Get().PushMenu(
			ParentWidget.ToSharedRef(),
			FWidgetPath(),
			MenuWidget.ToSharedRef(),
			MouseCursorLocation,
			FPopupTransitionEffect(FPopupTransitionEffect::ContextMenu)
		);
	}
}

FRay IMGPatternEditor::GetRayFromMousePos(FEditorViewportClient* ViewportClient, int MouseX, int MouseY) {
	FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(
		ViewportClient->Viewport,
		ViewportClient->GetScene(),
		ViewportClient->EngineShowFlags).SetRealtimeUpdate(ViewportClient->IsRealtime()));		// why SetRealtimeUpdate here??
	// this View is deleted by the FSceneViewFamilyContext destructor
	const FSceneView* View = ViewportClient->CalcSceneView(&ViewFamily);
	const FViewportCursorLocation MouseViewportRay(View, ViewportClient, MouseX, MouseY);

	FVector RayOrigin = MouseViewportRay.GetOrigin();
	const FVector RayDirection = MouseViewportRay.GetDirection();

	// in Ortho views, the RayOrigin appears to be completely arbitrary, in some views it is on the view plane,
	// others it moves back/forth with the OrthoZoom. Translate by a large amount here in hopes of getting
	// ray origin "outside" the scene (which is a disaster for numerical precision !! ... )
	if (ViewportClient->IsOrtho())
	{
		RayOrigin -= 0.1 * HALF_WORLD_MAX * RayDirection;
	}

	return FRay(RayOrigin, RayDirection, true);
}

