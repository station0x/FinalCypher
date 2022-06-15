//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/DungeonProp.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"

enum class EMarkerGenGridPatternRuleType : uint8;
struct FDAMarkerInfo;

class FMGGridSceneCell {
public:
	void Add(const FDAMarkerInfo& InMarkerInfo, EMarkerGenGridPatternRuleType InType);
	void Remove(const FString& InMarkerName, EMarkerGenGridPatternRuleType InType);
	bool Contains(const FString& InMarkerName, EMarkerGenGridPatternRuleType InType) const;
	void Clear();

	const TArray<FDAMarkerInfo>& GetTileMarkers() const { return TileMarkers; }
	const TArray<FDAMarkerInfo>& GetCornerMarkers() const { return CornerMarkers; }
	const TArray<FDAMarkerInfo>& GetEdgeXMarkers() const { return EdgeXMarkers; }
	const TArray<FDAMarkerInfo>& GetEdgeYMarkers() const { return EdgeYMarkers; }
	const TArray<FDAMarkerInfo>* GetMarkerList(EMarkerGenGridPatternRuleType InType) const;

private:
	TArray<FDAMarkerInfo>* GetMarkerList(EMarkerGenGridPatternRuleType InType);
	
private:
	TArray<FDAMarkerInfo> TileMarkers;
	TArray<FDAMarkerInfo> CornerMarkers;
	TArray<FDAMarkerInfo> EdgeXMarkers;
	TArray<FDAMarkerInfo> EdgeYMarkers;
};

class FMGGridSceneMarkerList {
public:
	FMGGridSceneMarkerList(const FVector& InCellSize, const TArray<FDAMarkerInfo>& InMarkers, int32 InBoundsExpansion = 0);
	FMGGridSceneMarkerList(const FMGGridSceneMarkerList&) = delete;		// Disallow copy constructor
	
	FORCEINLINE bool IsCoordValid(const FIntPoint& InCellCoord) const {
		return InCellCoord.X >= WorldOffset.X && InCellCoord.X < WorldOffset.X + WorldSize.X
			&& InCellCoord.Y >= WorldOffset.Y && InCellCoord.Y < WorldOffset.Y + WorldSize.Y;
	}
	
	FORCEINLINE FMGGridSceneCell* GetCell(const FIntPoint& InCellCoord) {
		if (!IsCoordValid(InCellCoord)) return nullptr;
		return &Cells[GetIndex(InCellCoord)];
	}

	FORCEINLINE const FMGGridSceneCell* GetCell(const FIntPoint& InCellCoord) const {
		if (!IsCoordValid(InCellCoord)) return nullptr;
		return &Cells[GetIndex(InCellCoord)];
	}

	void GetWorldToCellCoords(const FVector& InWorldLocation, FIntPoint& OutCoord, EMarkerGenGridPatternRuleType& OutCoordType) const;
	void GetCellToWorldCoords(const FIntPoint& InCoord, int32 InCoordZ, EMarkerGenGridPatternRuleType InCoordType, FVector& OutWorldLocation) const;
	void GenerateMarkerList(TArray<FDAMarkerInfo>& OutMarkers) const;

	FVector2D GetCellSize() const { return CellSize; }
	float GetCellHeight() const { return CellHeight; }
	FIntPoint GetWorldSize() const { return WorldSize; }
	FIntPoint GetWorldOffset() const { return WorldOffset; }
	int32 GenerateNextMarkerId();

private:
	FORCEINLINE int32 GetIndex(const FIntPoint& InCellCoord) const {
		const FIntPoint LocalCoord = InCellCoord - WorldOffset;
		return WorldSize.X * LocalCoord.Y + LocalCoord.X;
	}
	
private:
	FVector2D CellSize;
	float CellHeight;
	FIntPoint WorldSize;
	FIntPoint WorldOffset;
	TArray<FMGGridSceneCell> Cells;
	int32 NextMarkerId = 0;
};


template<typename T>
class TMGGridSceneCells {
public:
	static T DefaultValue;
	
	TMGGridSceneCells(const FIntPoint& InWorldOffset, const FIntPoint& InWorldSize)
			: WorldOffset(InWorldOffset)
			, WorldSize(InWorldSize)
	{
		const int32 NumItems = WorldSize.X * WorldSize.Y;
		Cells.AddDefaulted(NumItems);
	}
	
	FORCEINLINE const T* GetValue(const FIntPoint& InCellCoord) const {
		if (!IsCoordValid(InCellCoord)) {
			return nullptr;
		}

		return &Cells[GetIndex(InCellCoord)];
	}

	FORCEINLINE T* GetValue(const FIntPoint& InCellCoord) {
		if (!IsCoordValid(InCellCoord)) {
			return nullptr;
		}

		return &Cells[GetIndex(InCellCoord)];
	}


	FORCEINLINE void SetValue(const FIntPoint& InCellCoord, const T& InValue) {
		if (IsCoordValid(InCellCoord)) {
			Cells[GetIndex(InCellCoord)] = InValue;
		}
	}
	
	void Clear(const T& bInValue) {
		for (T& CellOccupied : Cells) {
			CellOccupied = bInValue;
		} 
	}
	
	FORCEINLINE bool IsCoordValid(const FIntPoint& InCellCoord) const {
		return InCellCoord.X >= WorldOffset.X && InCellCoord.X < WorldOffset.X + WorldSize.X
			&& InCellCoord.Y >= WorldOffset.Y && InCellCoord.Y < WorldOffset.Y + WorldSize.Y;
	}
	
	
protected:
	FORCEINLINE int32 GetIndex(const FIntPoint& InCellCoord) const {
		const FIntPoint LocalCoord = InCellCoord - WorldOffset;
		return WorldSize.X * LocalCoord.Y + LocalCoord.X;
	}
	
protected:
	FIntPoint WorldOffset;
	FIntPoint WorldSize;
	TArray<T> Cells;
};

struct FMGCellOccupancyInfo {
	bool bGroundOccupied = false;
	bool bEdgeXOccupied = false;
	bool bEdgeYOccupied = false;
	bool bCornerOccupied = false;
	void SetOccupied(EMarkerGenGridPatternRuleType InRuleType, bool InValue) {
		if (InRuleType == EMarkerGenGridPatternRuleType::Ground) {
			bGroundOccupied = InValue;
		}
		else if (InRuleType == EMarkerGenGridPatternRuleType::EdgeX) {
			bEdgeXOccupied = InValue;
		}
		else if (InRuleType == EMarkerGenGridPatternRuleType::EdgeY) {
			bEdgeYOccupied = InValue;
		} 
		else if (InRuleType == EMarkerGenGridPatternRuleType::Corner) {
			bCornerOccupied = InValue;
		} 
	}
	bool IsOccupied(EMarkerGenGridPatternRuleType InRuleType) const {
		if (InRuleType == EMarkerGenGridPatternRuleType::Ground) return bGroundOccupied;
		if (InRuleType == EMarkerGenGridPatternRuleType::EdgeX) return bEdgeXOccupied;
		if (InRuleType == EMarkerGenGridPatternRuleType::EdgeY) return bEdgeYOccupied;
		if (InRuleType == EMarkerGenGridPatternRuleType::Corner) return bCornerOccupied;
		return false;
	}
};

class FMGGridSceneCellBoolean : public TMGGridSceneCells<FMGCellOccupancyInfo> {
public:
	FMGGridSceneCellBoolean(const FIntPoint& InWorldOffset, const FIntPoint& InWorldSize)
		: TMGGridSceneCells(InWorldOffset, InWorldSize)
	{
	}
}; 

