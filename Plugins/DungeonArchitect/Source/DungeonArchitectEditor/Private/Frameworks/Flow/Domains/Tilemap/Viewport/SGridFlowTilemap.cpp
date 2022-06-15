//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Flow/Domains/Tilemap/Viewport/SGridFlowTilemap.h"

#include "Frameworks/Flow/Common/Widgets/SGridFlowItemOverlay.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractItem.h"
#include "Frameworks/Flow/Domains/Tilemap/FlowTilemapRenderer.h"
#include "Frameworks/Flow/Domains/Tilemap/Viewport/GridFlowTilemapViewport.h"
#include "Frameworks/Flow/Domains/Tilemap/Viewport/GridFlowTilemapViewportClient.h"
#include "Frameworks/FlowImpl/GridFlow/Tilemap/GridFlowTilemap.h"

#include "EditorStyleSet.h"
#include "Engine/TextureRenderTarget2D.h"
#include "Framework/Application/SlateApplication.h"
#include "Slate/SceneViewport.h"
#include "Widgets/SViewport.h"
#include "Widgets/Text/STextBlock.h"

#define LOCTEXT_NAMESPACE "SGridFlowTilemap"

void SGridFlowTilemap::Construct(const FArguments& InArgs) {
    this->ChildSlot
    [
        SNew(SOverlay)
        + SOverlay::Slot()
        .Expose(TexturePanelSlot)
        [
            SAssignNew(ViewportWidget, SViewport)
			.EnableGammaCorrection(false)
			.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute())
			.ShowEffectWhenDisabled(false)
			.EnableBlending(true)
        ]

        // Bottom-right corner text indicating the type of tool
        + SOverlay::Slot()
          .Padding(10)
          .VAlign(VAlign_Bottom)
          .HAlign(HAlign_Right)
        [
            SNew(STextBlock)
			.Visibility(EVisibility::HitTestInvisible)
			.TextStyle(FEditorStyle::Get(), "Graph.CornerText")
			.Text(LOCTEXT("TilemapWidgetCornerText", "Tilemap"))
        ]
    ];

    ViewportClient = MakeShareable(new FGridFlowTilemapViewportClient(SharedThis(this)));
    Viewport = MakeShareable(new FGridFlowTilemapViewport(ViewportClient.Get(), ViewportWidget, SharedThis(this)));
    ViewportWidget->SetViewportInterface(Viewport.ToSharedRef());
}

void SGridFlowTilemap::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) {
    ViewportClient->SetZoomPos(Viewport->GetViewOffset(), Viewport->GetZoomAmount());

    Viewport->Invalidate();
    Viewport->InvalidateDisplay();
}

void SGridFlowTilemap::GeneratePreviewTexture(UGridFlowTilemap* InTilemap) {
    if (InTilemap) {
        FFlowTilemapRendererSettings Settings;
        Settings.bUseTextureTileSize = false;
        Settings.TileSize = 10;
        Settings.BackgroundColor = FLinearColor::Black;

        UTexture* PreviewTexture = FFlowTilemapRenderer::Create(InTilemap, Settings);
        ViewportClient->SetTexture(PreviewTexture);
    }
    else {
        ViewportClient->SetTexture(nullptr);
    }
}

#undef LOCTEXT_NAMESPACE

