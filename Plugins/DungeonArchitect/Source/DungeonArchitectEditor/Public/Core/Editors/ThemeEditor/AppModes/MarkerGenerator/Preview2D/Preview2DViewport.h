//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Slate/SceneViewport.h"

/** Viewport Client 2D for the preview viewport */
class DUNGEONARCHITECTEDITOR_API FMGPreview2DViewport : public FSceneViewport {
public:
	FMGPreview2DViewport(FViewportClient* InViewportClient, TSharedPtr<SViewport> InViewportWidget);

	virtual FReply OnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseButtonUp(const FGeometry& InGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseMove(const FGeometry& InGeometry, const FPointerEvent& MouseEvent) override;
	virtual FReply OnMouseWheel(const FGeometry& InGeometry, const FPointerEvent& MouseEvent) override;

	float GetZoomAmount() const { return ZoomAmount; }
	FVector2D GetViewOffset() const { return ViewOffset; }
	FVector2D GraphCoordToPanelCoord(const FVector2D& GraphSpaceCoordinate) const;
	FVector2D PanelCoordToGraphCoord(const FVector2D& PanelSpaceCoordinate) const;

private:
	/** The position within the graph at which the user is looking */
	FVector2D ViewOffset;

	/** How zoomed in/out we are. e.g. 0.25f results in quarter-sized nodes. */
	float ZoomAmount;

	float ZoomSensitivity = 0.1f;

	/** Are we panning the view at the moment? */
	bool bIsPanning;

	/**	The current position of the software cursor */
	FVector2D SoftwareCursorPosition;

	/**	Whether the software cursor should be drawn */
	bool bShowSoftwareCursor;

	bool bRequestRedraw;

	/** The total distance that the mouse has been dragged while down */
	float TotalMouseDelta;
};

