//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

enum class EMarkerGenGridPatternRuleType : uint8;

class FGridPatternEditorUtils {
public:
	static void DeprojectGround(const FVector& InWorldIntersection, float InTileSize, float InEdgeSize, FIntPoint& OutCoord, EMarkerGenGridPatternRuleType& OutItemType);
	static void GetItemLocationScale(FIntPoint InCoord, float TileSize, float EdgeSize, EMarkerGenGridPatternRuleType InItemType, bool bVisuallyDominant, FVector& OutLocation, FVector& OutScale);
};

