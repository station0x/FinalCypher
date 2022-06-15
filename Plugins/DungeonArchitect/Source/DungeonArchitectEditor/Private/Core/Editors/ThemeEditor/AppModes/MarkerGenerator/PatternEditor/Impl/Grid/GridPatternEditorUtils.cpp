//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Impl/Grid/GridPatternEditorUtils.h"

#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"

void FGridPatternEditorUtils::DeprojectGround(const FVector& InWorldIntersection, float InTileSize, float InEdgeSize, FIntPoint& OutCoord, EMarkerGenGridPatternRuleType& OutItemType) {
	const float Offset = InTileSize + InEdgeSize;
	FVector2D Loc(InWorldIntersection);
	Loc += FVector2D(InTileSize * 0.5f + InEdgeSize);
		
	FIntPoint Key;
	Key.X = FMath::FloorToInt(Loc.X / Offset);
	Key.Y = FMath::FloorToInt(Loc.Y / Offset);

	const float FX = Loc.X - Key.X * Offset;
	const float FY = Loc.Y - Key.Y * Offset;
	if (FX < InEdgeSize && FY < InEdgeSize) {
		OutItemType = EMarkerGenGridPatternRuleType::Corner;
	}
	else if (FX < InEdgeSize) {
		OutItemType = EMarkerGenGridPatternRuleType::EdgeY;
	}
	else if (FY < InEdgeSize) {
		OutItemType = EMarkerGenGridPatternRuleType::EdgeX;
	}
	else {
		OutItemType = EMarkerGenGridPatternRuleType::Ground;
	}
	OutCoord = Key;
}

void FGridPatternEditorUtils::GetItemLocationScale(FIntPoint InCoord, float TileSize, float EdgeSize, EMarkerGenGridPatternRuleType InItemType, bool bVisuallyDominant, FVector& OutLocation, FVector& OutScale) {
	constexpr float MeshBaseSize = 100.0f;
	const float Offset = TileSize + EdgeSize;

	float WallHeight = TileSize;
	if (!bVisuallyDominant) {
		WallHeight *= 0.25f;
	}
	
	if (InItemType == EMarkerGenGridPatternRuleType::Corner) {
		OutLocation = FVector(InCoord.X - 0.5f, InCoord.Y - 0.5f, 0) * Offset;
		OutScale = FVector(EdgeSize, EdgeSize, WallHeight) / MeshBaseSize;
	}
	else if (InItemType == EMarkerGenGridPatternRuleType::EdgeX) {
		OutLocation = FVector(InCoord.X, InCoord.Y - 0.5f, 0) * Offset;
		OutScale = FVector(TileSize, EdgeSize, WallHeight) / MeshBaseSize;
	}
	else if (InItemType == EMarkerGenGridPatternRuleType::EdgeY) {
		OutLocation = FVector(InCoord.X - 0.5f, InCoord.Y, 0) * Offset;
		OutScale = FVector(EdgeSize, TileSize, WallHeight) / MeshBaseSize;
	}
	else {	// Ground
		OutLocation = FVector(InCoord.X, InCoord.Y, 0) * Offset;
		OutScale = FVector(TileSize, TileSize, TileSize * 0.1f) / MeshBaseSize;
	}
}

