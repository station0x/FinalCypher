//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/GridSceneMarkerList.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"
#include "Frameworks/MarkerGenerator/MarkerGenProcessor.h"

class UMarkerGenGridLayer;
class FMGPatternGridRuleExecutor;

class FMarkerGenGridProcessor : public IMarkerGenProcessor {
public:
	FMarkerGenGridProcessor(const FTransform& InDungeonTransform, const FVector& InGridSize);
	virtual bool Process(const UMarkerGenLayer* InLayer, const TArray<FDAMarkerInfo>& InMarkers, const FRandomStream& InRandom, TArray<FDAMarkerInfo>& OutMarkers) override;

private:
	struct FMGPatternMatchCoord {
		int32 X;
		int32 Y;
		int32 AsmIdx = 0;
	};

	static void GeneratePatternMatchCommands(const FMGGridPatternAssembly& InPattern, int32 InPatternAsmIdx, const FIntPoint& InWorldOffset, const FIntPoint& InWorldSize, TArray<FMGPatternMatchCoord>& OutCommands);
	void ExecutePattern(int X, int Y, const FMGGridPatternAssembly& InPattern, const UMarkerGenGridLayer* InGridLayer, FMGGridSceneCellBoolean& InOccupancyList, TSharedPtr<FMGPatternGridRuleExecutor> InExecutor) const;
	
private:
	FVector GridSize;
};

