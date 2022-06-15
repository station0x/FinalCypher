//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/Preview2D/Preview2DViewportWidget.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/Preview2D/Preview2DViewport.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/Preview2D/Preview2DViewportClient.h"

#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"

void SMGPreview2DViewport::Construct(const FArguments& InArgs) {
	PatternEditorPtr = InArgs._PatternEditor;

	ChildSlot
	[
		SAssignNew(ViewportWidget, SViewport)
		.EnableGammaCorrection(false)
		.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
		.ShowEffectWhenDisabled(false)
	];
	
	ViewportClient = MakeShareable(new FMGPreview2DViewportClient(SharedThis(this)));
	Viewport = MakeShareable(new FMGPreview2DViewport(ViewportClient.Get(), ViewportWidget));
	ViewportWidget->SetViewportInterface( Viewport.ToSharedRef() );
}

void SMGPreview2DViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
	ViewportClient->SetZoomPos(Viewport->GetViewOffset(), Viewport->GetZoomAmount());

	Viewport->Invalidate();
	Viewport->InvalidateDisplay();
}

void SMGPreview2DViewport::RefreshViewport() const {
	if (Viewport.IsValid()) {
		Viewport->Invalidate();
	}
}

