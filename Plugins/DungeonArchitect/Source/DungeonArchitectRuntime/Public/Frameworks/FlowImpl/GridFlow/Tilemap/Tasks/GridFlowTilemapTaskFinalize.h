//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/Tilemap/Tasks/FlowTilemapTask.h"
#include "GridFlowTilemapTaskFinalize.generated.h"

struct FFlowTilemapCell;
class UGridFlowTilemap;
class UGridFlowAbstractGraph;

UCLASS(Meta = (TilemapTask, Title = "Finalize Tilemap", Tooltip = "Finalize Tilemap", MenuPriority = 2600))
class DUNGEONARCHITECTRUNTIME_API UGridFlowTilemapTaskFinalize : public UFlowTilemapTask {
    GENERATED_BODY()
public:
    /**
        Debug: Show tiles that are not reachable due to overlays blocking them

        Variable Name: [N/A]
    */
    UPROPERTY(EditAnywhere, Category = "Debug")
    bool bDebugUnwalkableCells = false;

public:
    virtual void Execute(const FFlowExecutionInput& Input, const FFlowTaskExecutionSettings& InExecSettings, FFlowExecutionOutput& Output) override;

private:
    TArray<FFlowTilemapCell*> FilterWalkablePath(const TArray<FFlowTilemapCell*>& Cells) const;
    void UpdateLayoutTilemapMetadata(UGridFlowAbstractGraph* InGraph, UGridFlowTilemap* InTilemap) const;
};

