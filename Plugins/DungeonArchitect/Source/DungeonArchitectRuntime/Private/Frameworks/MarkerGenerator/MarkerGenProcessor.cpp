//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/MarkerGenProcessor.h"

#include "Builders/FloorPlan/FloorPlanConfig.h"
#include "Builders/Grid/GridDungeonConfig.h"
#include "Builders/GridFlow/GridFlowConfig.h"
#include "Builders/SimpleCity/SimpleCityConfig.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridProcessor.h"

TSharedPtr<IMarkerGenProcessor> FMarkerGenProcessorFactory::Create(const UDungeonConfig* InConfig, const FTransform& InDungeonTransform) {
	if (InConfig) {
		if (const UGridDungeonConfig* GridConfig = Cast<UGridDungeonConfig>(InConfig)) {
			const FVector GridSize = GridConfig->GridCellSize;
			const FTransform GridDungeonTransform = FTransform(InDungeonTransform.GetTranslation()); 
			return MakeShareable(new FMarkerGenGridProcessor(GridDungeonTransform, GridSize));
		}
		else if (const UGridFlowConfig* GridFlowConfig = Cast<UGridFlowConfig>(InConfig)) {
			const FVector GridSize = GridFlowConfig->GridSize;
			return MakeShareable(new FMarkerGenGridProcessor(InDungeonTransform, GridSize));
		}
		else if (const USimpleCityConfig* SimpleCityConfig = Cast<USimpleCityConfig>(InConfig)) {
			const FVector2D GridSize = SimpleCityConfig->CellSize;
			return MakeShareable(new FMarkerGenGridProcessor(InDungeonTransform, FVector(GridSize, 1)));
		}
		else if (const UFloorPlanConfig* FloorPlanConfig = Cast<UFloorPlanConfig>(InConfig)) {
			const FVector GridSize = FloorPlanConfig->GridSize;
			return MakeShareable(new FMarkerGenGridProcessor(InDungeonTransform, GridSize));
		}
	}

	return nullptr;
}

