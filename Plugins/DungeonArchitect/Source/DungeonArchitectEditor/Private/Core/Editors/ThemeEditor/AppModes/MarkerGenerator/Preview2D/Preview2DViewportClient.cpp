//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/Preview2D/Preview2DViewportClient.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/Preview2D/Preview2DViewportWidget.h"

#include "CanvasTypes.h"

FMGPreview2DViewportClient::FMGPreview2DViewportClient(TWeakPtr<SMGPreview2DViewport> InPatternEditorViewport)
	: PatternEditorViewportPtr(InPatternEditorViewport)
	, ZoomAmount(1.0f)
	, ZoomPos(FVector2D::ZeroVector)
	, ZoomSensitivity(0.1f)
{
}

FMGPreview2DViewportClient::~FMGPreview2DViewportClient() {
	
}

void FMGPreview2DViewportClient::Draw(FViewport* Viewport, FCanvas* Canvas) {
	Canvas->Clear(FLinearColor(0.15f, 0.15f, 0.15f));

	struct FCamTranslator {
		FCamTranslator(float InZoomAmount, const FVector2D& InZoomPos)
			: ZoomPos(InZoomPos),
			  ZoomAmount(InZoomAmount) {}

		FVector2D operator()(const FVector2D& V) const {
			return FVector2D(-ZoomPos.X * ZoomAmount + V.X * ZoomAmount, -ZoomPos.Y * ZoomAmount + V.Y * ZoomAmount);
		}

		FVector2D ZoomPos;
		float ZoomAmount;
	};

	struct FCamScaler {
		FCamScaler(float InZoomAmount)
			: ZoomAmount(InZoomAmount) {}

		FVector2D operator()(const FVector2D& V) const {
			return FVector2D(V.X * ZoomAmount, V.Y * ZoomAmount);
		}

		float operator()(float V) const {
			return V * ZoomAmount;
		}
		
		float ZoomAmount;
	};


	FCamTranslator T(ZoomAmount, ZoomPos);
	FCamScaler S(ZoomAmount);
	
	const float BoxWidth = 100;
	const float BoxHeight = 100;

	const FVector2D Pos = T(FVector2D(0, 0));
	Canvas->DrawTile(Pos.X, Pos.Y, S(BoxWidth), S(BoxHeight), 0.0f, 0.0f, 1.0f, 1.0f, FLinearColor::Red, nullptr, bUseTranslucentBlend);
	
}

void FMGPreview2DViewportClient::SetZoomPos(FVector2D InNewPos, float InNewZoom) {
	ZoomPos = InNewPos;
	ZoomAmount = InNewZoom;
}

void FMGPreview2DViewportClient::SetTranslucent(bool bInUseTranslucentBlend) {
	bUseTranslucentBlend = bInUseTranslucentBlend;
}

