//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Tasks/BaseFlowLayoutTask.h"
#include "SnapGridFlowLayoutTaskCreateGrid.generated.h"

class UFlowAbstractGraphBase;

UCLASS(Meta = (AbstractTask, Title="Create Grid 3D", Tooltip="Create a grid to work with", MenuPriority = 1000))
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowLayoutTaskCreateGrid : public UBaseFlowLayoutTask {
    GENERATED_BODY()
public:
    
    /** 
    The size of the initial layout grid 

    Variable Name: GridSize
    */
    UPROPERTY(EditAnywhere, Category = "Create Grid 3D")
    FIntVector GridSize = FIntVector(5, 4, 3);

    /**
    Controls how far the nodes are. Only for preview purpose 

    Variable Name: [N/A]
    */
    UPROPERTY(EditAnywhere, Category = "Preview")
    int SeparationDistance = 400;
    
public:
    virtual void Execute(const FFlowExecutionInput& Input, const FFlowTaskExecutionSettings& InExecSettings, FFlowExecutionOutput& Output) override;
    virtual bool GetParameter(const FString& InParameterName, FDAAttribute& OutValue) override;
    virtual bool SetParameter(const FString& InParameterName, const FDAAttribute& InValue) override;
    virtual bool SetParameterSerialized(const FString& InParameterName, const FString& InSerializedText) override;
    
private:
    void CreateGraph(UFlowAbstractGraphBase* InGraph, const FIntVector& InGridSize) const;
};

