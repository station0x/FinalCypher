//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class SMGPreview2DViewport;

class FMGPreview2DViewportClient : public FCommonViewportClient {
public:
	/** Constructor */
	FMGPreview2DViewportClient(TWeakPtr<SMGPreview2DViewport> InPatternEditorViewport);
	virtual ~FMGPreview2DViewportClient() override;

	/** FLevelEditorViewportClient interface */
	virtual void Draw(FViewport* Viewport, FCanvas* Canvas) override;
	void SetZoomPos(FVector2D InNewPos, float InNewZoom);
	void SetTranslucent(bool bInUseTranslucentBlend);

private:
	TWeakPtr<SMGPreview2DViewport> PatternEditorViewportPtr;
	
	float ZoomAmount;
	FVector2D ZoomPos;
	float ZoomSensitivity;
	bool bUseTranslucentBlend = true;
};

