//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class IMGPatternEditor;
class FMGPreview2DViewport;
class FMGPreview2DViewportClient;

class SMGPreview2DViewport : public SCompoundWidget {
public:
	SLATE_BEGIN_ARGS(SMGPreview2DViewport) {}
		SLATE_ARGUMENT(TWeakPtr<IMGPatternEditor>, PatternEditor)
	SLATE_END_ARGS()
	
	void Construct(const FArguments& InArgs);
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;
	void RefreshViewport() const;

private:
	// Pointer back to the Font editor tool that owns us
	TWeakPtr<IMGPatternEditor> PatternEditorPtr;

	TSharedPtr<SViewport> ViewportWidget;
	TSharedPtr<FMGPreview2DViewportClient> ViewportClient;
	
	// Slate viewport for rendering and I/O
	TSharedPtr<FMGPreview2DViewport> Viewport;
};


