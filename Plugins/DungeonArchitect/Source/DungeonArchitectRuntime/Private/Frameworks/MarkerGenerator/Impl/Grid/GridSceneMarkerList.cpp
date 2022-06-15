//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/Impl/Grid/GridSceneMarkerList.h"


template<>
bool TMGGridSceneCells<bool>::DefaultValue = false;

////////////////////////////// FMGGridSceneCell //////////////////////////////
void FMGGridSceneCell::Add(const FDAMarkerInfo& InMarkerInfo, EMarkerGenGridPatternRuleType InType) {
	TArray<FDAMarkerInfo>* MarkerList = GetMarkerList(InType);
	// Make sure we don't have duplicates
	if (MarkerList) {
		bool bFoundDuplicate = false;
		for (const FDAMarkerInfo& MarkerInfo : *MarkerList) {
			if (MarkerInfo.MarkerName == InMarkerInfo.MarkerName && MarkerInfo.Transform.GetLocation().Equals(InMarkerInfo.Transform.GetLocation())) {
				bFoundDuplicate = true;
				break;
			}
		}
		if (!bFoundDuplicate) {
			MarkerList->Add(InMarkerInfo);
		}
	}
}

void FMGGridSceneCell::Remove(const FString& InMarkerName, EMarkerGenGridPatternRuleType InType) {
	TArray<FDAMarkerInfo>* MarkerList = GetMarkerList(InType);
	if (MarkerList) {
		*MarkerList = MarkerList->FilterByPredicate([InMarkerName](const FDAMarkerInfo& MarkerInfo) -> bool {
			return MarkerInfo.MarkerName != InMarkerName;
		}); 
	}
}

bool FMGGridSceneCell::Contains(const FString& InMarkerName, EMarkerGenGridPatternRuleType InType) const {
	const TArray<FDAMarkerInfo>* MarkerList = GetMarkerList(InType);
	if (MarkerList) {
		for (const FDAMarkerInfo& MarkerInfo : *MarkerList) {
			if (MarkerInfo.MarkerName == InMarkerName) {
				return true;
			}
		}
	}
	return false;
}

void FMGGridSceneCell::Clear() {
	TileMarkers.Reset();
	CornerMarkers.Reset();
	EdgeXMarkers.Reset();
	EdgeYMarkers.Reset();
}


TArray<FDAMarkerInfo>* FMGGridSceneCell::GetMarkerList(EMarkerGenGridPatternRuleType InType) {
	TArray<FDAMarkerInfo>* MarkerList = nullptr;
	if (InType == EMarkerGenGridPatternRuleType::Ground) {
		MarkerList = &TileMarkers;
	}
	else if (InType == EMarkerGenGridPatternRuleType::Corner) {
		MarkerList = &CornerMarkers;
	}
	else if (InType == EMarkerGenGridPatternRuleType::EdgeX) {
		MarkerList = &EdgeXMarkers;
	}
	else if (InType == EMarkerGenGridPatternRuleType::EdgeY) {
		MarkerList = &EdgeYMarkers;
	}
	else {
		check(0);
	}
	return MarkerList;
}

const TArray<FDAMarkerInfo>* FMGGridSceneCell::GetMarkerList(EMarkerGenGridPatternRuleType InType) const {
	const TArray<FDAMarkerInfo>* MarkerList = nullptr;
	if (InType == EMarkerGenGridPatternRuleType::Ground) {
		MarkerList = &TileMarkers;
	}
	else if (InType == EMarkerGenGridPatternRuleType::Corner) {
		MarkerList = &CornerMarkers;
	}
	else if (InType == EMarkerGenGridPatternRuleType::EdgeX) {
		MarkerList = &EdgeXMarkers;
	}
	else if (InType == EMarkerGenGridPatternRuleType::EdgeY) {
		MarkerList = &EdgeYMarkers;
	}
	else {
		check(0);
	}
	return MarkerList;
}


////////////////////////////// FMGGridSceneMarkerList //////////////////////////////
FMGGridSceneMarkerList::FMGGridSceneMarkerList(const FVector& InCellSize, const TArray<FDAMarkerInfo>& InMarkers, int32 InBoundsExpansion)
	: CellSize(FVector2D(InCellSize))
	, CellHeight(InCellSize.Z)
{
	NextMarkerId = InMarkers.Num();

	// Find the bounds and allocate default cells
	FVector2D BoundsMin, BoundsMax;
	if (InMarkers.Num() > 0) {
		BoundsMin = BoundsMax = FVector2D(InMarkers[0].Transform.GetLocation());
		for (const FDAMarkerInfo& Marker : InMarkers) {
			const FVector Location = Marker.Transform.GetLocation();
			BoundsMin.X = FMath::Min(BoundsMin.X, Location.X);
			BoundsMin.Y = FMath::Min(BoundsMin.Y, Location.Y);
			BoundsMax.X = FMath::Max(BoundsMax.X, Location.X);
			BoundsMax.Y = FMath::Max(BoundsMax.Y, Location.Y);
		}
	}
	else {
		BoundsMin = BoundsMax = FVector2D::ZeroVector;
	}

	if (CellSize.X > 0 && CellSize.Y > 0) {
		BoundsMin /= CellSize;
		BoundsMax /= CellSize;
	}
	else {
		BoundsMin = BoundsMax = FVector2D::ZeroVector;
	}

	const int32 StartX = FMath::FloorToInt(BoundsMin.X);
	const int32 StartY = FMath::FloorToInt(BoundsMin.Y);
	const int32 EndX = FMath::FloorToInt(BoundsMax.X);
	const int32 EndY = FMath::FloorToInt(BoundsMax.Y);
	
	WorldOffset.X = StartX;
	WorldOffset.Y = StartY;
	WorldSize.X = EndX - StartX + 1;
	WorldSize.Y = EndY - StartY + 1;

	// Expand the bounds
	WorldOffset -= FIntPoint(InBoundsExpansion, InBoundsExpansion);
	WorldSize += FIntPoint(InBoundsExpansion, InBoundsExpansion) * 2;

	const int32 NumCells = WorldSize.X * WorldSize.Y;
	Cells.AddDefaulted(NumCells);

	// Register the markers
	for (const FDAMarkerInfo& Marker : InMarkers) {
		FIntPoint Coord;
		EMarkerGenGridPatternRuleType CoordType;
		GetWorldToCellCoords(Marker.Transform.GetLocation(), Coord, CoordType);
		FMGGridSceneCell* CellPtr = GetCell(Coord);
		if (CellPtr) {
			CellPtr->Add(Marker, CoordType);
		}
	} 
}

void FMGGridSceneMarkerList::GetWorldToCellCoords(const FVector& InWorldLocation, FIntPoint& OutCoord, EMarkerGenGridPatternRuleType& OutCoordType) const {
	const FVector2D CoordF = FVector2D(InWorldLocation) / CellSize;

	OutCoord = FIntPoint(
		FMath::FloorToInt(CoordF.X),
		FMath::FloorToInt(CoordF.Y));

	const float DX = CoordF.X - OutCoord.X;
	const float DY = CoordF.Y - OutCoord.Y;

#define MG_EQUALS(A, B) FMath::IsNearlyEqual(A, B, 1e-02f) 
	if (MG_EQUALS(DX, 0.0f) && MG_EQUALS(DY, 0.0f)) {
		OutCoordType = EMarkerGenGridPatternRuleType::Corner;
	}
	else if (MG_EQUALS(DX, 0.5f) && MG_EQUALS(DY, 0.0f)) {
		OutCoordType = EMarkerGenGridPatternRuleType::EdgeX;
	}
	else if (MG_EQUALS(DX, 0.0f) && MG_EQUALS(DY, 0.5f)) {
		OutCoordType = EMarkerGenGridPatternRuleType::EdgeY;
	}
	else {
		OutCoordType = EMarkerGenGridPatternRuleType::Ground;
	}
#undef MG_EQUALS
}

void FMGGridSceneMarkerList::GetCellToWorldCoords(const FIntPoint& InCoord, int32 InCoordZ, EMarkerGenGridPatternRuleType InCoordType, FVector& OutWorldLocation) const {
	FVector2D CoordF = FVector2D(InCoord.X, InCoord.Y);
	if (InCoordType == EMarkerGenGridPatternRuleType::Ground) {
		CoordF += FVector2D(0.5f, 0.5f);
	}
	else if (InCoordType == EMarkerGenGridPatternRuleType::EdgeX) {
		CoordF += FVector2D(0.5f, 0.0f);
	}
	else if (InCoordType == EMarkerGenGridPatternRuleType::EdgeY) {
		CoordF += FVector2D(0.0f, 0.5f);
	}

	OutWorldLocation = FVector(CoordF * CellSize, InCoordZ * CellHeight);
}

void FMGGridSceneMarkerList::GenerateMarkerList(TArray<FDAMarkerInfo>& OutMarkers) const {
	OutMarkers.Reset();
	
	for (const FMGGridSceneCell& Cell : Cells) {
		OutMarkers.Append(Cell.GetTileMarkers());
		OutMarkers.Append(Cell.GetCornerMarkers());
		OutMarkers.Append(Cell.GetEdgeXMarkers());
		OutMarkers.Append(Cell.GetEdgeYMarkers());
	}

	// Reset their Ids
	int32 IndexCounter = 0;
	for (FDAMarkerInfo& Marker : OutMarkers) {
		Marker.Id = IndexCounter++;
	}
}

int32 FMGGridSceneMarkerList::GenerateNextMarkerId() {
	return NextMarkerId++;
}

