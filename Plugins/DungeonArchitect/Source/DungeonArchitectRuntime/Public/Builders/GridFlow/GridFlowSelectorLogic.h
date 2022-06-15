//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/ThemeEngine/Rules/DungeonSelectorLogic.h"
#include "GridFlowSelectorLogic.generated.h"

class UGridFlowModel;
class UGridFlowConfig;
class UGridFlowBuilder;
class UGridFlowQuery;

/**
*
*/
UCLASS(Blueprintable, HideDropDown)
class DUNGEONARCHITECTRUNTIME_API UGridFlowSelectorLogic : public UDungeonSelectorLogic {
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintNativeEvent, Category = "Dungeon")
    bool SelectNode(UGridFlowModel* Model, UGridFlowConfig* Config, UGridFlowBuilder* Builder, UGridFlowQuery* Query,
                    const FRandomStream& RandomStream, int32 TileX, int32 TileY, const FTransform& MarkerTransform);
    virtual bool SelectNode_Implementation(UGridFlowModel* Model, UGridFlowConfig* Config, UGridFlowBuilder* Builder,
                                           UGridFlowQuery* Query, const FRandomStream& RandomStream, int32 TileX,
                                           int32 TileY, const FTransform& MarkerTransform);


};

