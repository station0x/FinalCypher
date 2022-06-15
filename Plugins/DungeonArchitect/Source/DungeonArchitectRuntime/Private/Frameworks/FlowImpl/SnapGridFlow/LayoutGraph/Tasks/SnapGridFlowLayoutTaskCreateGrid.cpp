//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/Tasks/SnapGridFlowLayoutTaskCreateGrid.h"

#include "Frameworks/Flow/ExecGraph/FlowExecTaskAttributeMacros.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/SnapGridFlowAbstractGraph.h"

////////////////////////////////////// UGridFlowTaskAbstract_CreateGrid3D //////////////////////////////////////

void USnapGridFlowLayoutTaskCreateGrid::Execute(const FFlowExecutionInput& Input, const FFlowTaskExecutionSettings& InExecSettings, FFlowExecutionOutput& Output) {
    if (Input.IncomingNodeOutputs.Num() != 0) {
        Output.ExecutionResult = EFlowTaskExecutionResult::FailHalt;
        Output.ErrorMessage = "Input not allowed";
        return;
    }

    // Build the graph object
    USnapGridFlowAbstractGraph* Graph = NewObject<USnapGridFlowAbstractGraph>();
    Graph->GridSize = GridSize;
    CreateGraph(Graph, GridSize);

    // Create a new state, since this will our first node
    Output.State = MakeShareable(new FFlowExecNodeState);
    Output.State->SetStateObject(UFlowAbstractGraphBase::StateTypeID, Graph);
    
    Output.ExecutionResult = EFlowTaskExecutionResult::Success;
}

void USnapGridFlowLayoutTaskCreateGrid::CreateGraph(UFlowAbstractGraphBase* InGraph, const FIntVector& InGridSize) const {
    TMap<FIntVector, FGuid> Nodes;
    for (int z = 0; z < InGridSize.Z; z++) {
        for (int y = 0; y < InGridSize.Y; y++) {
            for (int x = 0; x < InGridSize.X; x++) {
                FIntVector Coord(x, y, z);
                UFlowAbstractNode* Node = InGraph->CreateNode();
                Node->PreviewLocation = FVector(x, y, z) * SeparationDistance;
                Node->Coord = FVector(x, y, z);
                Nodes.Add(Coord, Node->NodeId);

                if (x > 0) {
                    FGuid PrevNodeX = Nodes[FIntVector(x - 1, y, z)];
                    InGraph->CreateLink(PrevNodeX, Node->NodeId);
                }

                if (y > 0) {
                    FGuid PrevNodeY = Nodes[FIntVector(x, y - 1, z)];
                    InGraph->CreateLink(PrevNodeY, Node->NodeId);
                }
                if (z > 0) {
                    FGuid PrevNodeZ = Nodes[FIntVector(x, y, z - 1)];
                    InGraph->CreateLink(PrevNodeZ, Node->NodeId);
                }
            }
        }
    }
}

bool USnapGridFlowLayoutTaskCreateGrid::GetParameter(const FString& InParameterName, FDAAttribute& OutValue) {
    FLOWTASKATTR_GET_VECTOR(GridSize);

    return false;
    
}

bool USnapGridFlowLayoutTaskCreateGrid::SetParameter(const FString& InParameterName, const FDAAttribute& InValue) {
    FLOWTASKATTR_SET_VECTORI(GridSize)

    return false;
    
}

bool USnapGridFlowLayoutTaskCreateGrid::SetParameterSerialized(const FString& InParameterName, const FString& InSerializedText) {
    FLOWTASKATTR_SET_PARSE_VECTORI(GridSize)

    return false;
}

