//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Flow/Domains/Tilemap/Tasks/FlowTilemapTaskMerge.h"

#include "Frameworks/Flow/Domains/Tilemap/FlowTilemap.h"

void UFlowTilemapTaskMerge::Execute(const FFlowExecutionInput& Input, const FFlowTaskExecutionSettings& InExecSettings, FFlowExecutionOutput& Output) {
    if (Input.IncomingNodeOutputs.Num() == 0) {
        Output.ErrorMessage = "Missing Input";
        Output.ExecutionResult = EFlowTaskExecutionResult::FailHalt;
        return;
    }

    Output.State = Input.IncomingNodeOutputs[0].State->Clone();
    UFlowTilemap* Tilemap = Output.State->GetState<UFlowTilemap>(UFlowTilemap::StateTypeID);
    
    if (!Tilemap) {
        Output.ErrorMessage = "Invalid Input Tilemap";
        Output.ExecutionResult = EFlowTaskExecutionResult::FailHalt;
        return;
    }

    TArray<UFlowTilemap*> IncomingTilemaps;
    for (const FFlowExecutionOutput& IncomingNodeOutput : Input.IncomingNodeOutputs) {
        UFlowTilemap* IncomingTilemap = IncomingNodeOutput.State->GetState<UFlowTilemap>(UFlowTilemap::StateTypeID);
        if (IncomingTilemap) {
            IncomingTilemaps.Add(IncomingTilemap);
        }
    }


    for (int y = 0; y < Tilemap->GetHeight(); y++) {
        for (int x = 0; x < Tilemap->GetWidth(); x++) {
            int32 BestWeight = 0;
            FFlowTilemapCell* BestCell = nullptr;
            TArray<FFlowTilemapCellOverlay*> IncomingOverlays;
            for (UFlowTilemap* IncomingTilemap : IncomingTilemaps) {
                int32 Weight = 0;
                FFlowTilemapCell& IncomingCell = IncomingTilemap->Get(x, y);
                if (IncomingCell.CellType == EFlowTilemapCellType::Empty) {
                    Weight = 1;
                }
                else if (IncomingCell.CellType == EFlowTilemapCellType::Custom) {
                    Weight = 2;
                }
                else {
                    Weight = 3;
                }

                if (IncomingCell.bHasOverlay) {
                    IncomingOverlays.Add(&IncomingCell.Overlay);
                }

                bool bUseResult = false;
                if (Weight > BestWeight) {
                    bUseResult = true;
                }
                else if (Weight == BestWeight) {
                    if (BestCell && IncomingCell.Height > BestCell->Height) {
                        bUseResult = true;
                    }
                }

                if (bUseResult) {
                    BestCell = &IncomingCell;
                    BestWeight = Weight;
                }
            }

            FFlowTilemapCell& Cell = Tilemap->Get(x, y);
            if (!BestCell) {
                Cell.CellType = EFlowTilemapCellType::Empty;
                continue;
            }

            Cell = *BestCell;

            FFlowTilemapCellOverlay* BestOverlay = nullptr;
            float BestOverlayWeight = 0.0f;
            for (FFlowTilemapCellOverlay* IncomingOverlay : IncomingOverlays) {
                bool bValid = Cell.Height >= IncomingOverlay->MergeConfig.MinHeight
                    && Cell.Height <= IncomingOverlay->MergeConfig.MaxHeight;

                if (!bValid) continue;

                if (!BestOverlay || IncomingOverlay->NoiseValue > BestOverlayWeight) {
                    BestOverlay = IncomingOverlay;
                    BestOverlayWeight = IncomingOverlay->NoiseValue;
                }

            }

            if (BestOverlay) {
                Cell.bHasOverlay = true;
                Cell.Overlay = *BestOverlay;
            }

            if (Cell.CellType == EFlowTilemapCellType::Wall && Cell.bHasOverlay) {
                EFlowTilemapCellOverlayMergeWallOverlayRule WallOverlayRule = Cell
                                                                                  .Overlay.MergeConfig.WallOverlayRule;
                if (WallOverlayRule == EFlowTilemapCellOverlayMergeWallOverlayRule::KeepOverlayRemoveWall) {
                    Cell.CellType = EFlowTilemapCellType::Floor;
                    Cell.bUseCustomColor = true;
                }
                else if (WallOverlayRule == EFlowTilemapCellOverlayMergeWallOverlayRule::KeepWallRemoveOverlay) {
                    Cell.bHasOverlay = false;
                }
            }
        }
    }

    
    // Merge the edges
    for (int y = 0; y <= Tilemap->GetHeight(); y++) {
        for (int x = 0; x <= Tilemap->GetWidth(); x++) {
            
            const FFlowTilemapEdge* BestEdgeH = nullptr;
            const FFlowTilemapEdge* BestEdgeV = nullptr;
            for (UFlowTilemap* IncomingTilemap : IncomingTilemaps) {
                {
                    const FFlowTilemapEdge& IncomingEdgeH = IncomingTilemap->GetEdgeH(x, y);
                    if (IncomingEdgeH.EdgeType != EFlowTilemapEdgeType::Empty) {
                        BestEdgeH = &IncomingEdgeH;
                    }
                }
                {
                    FFlowTilemapEdge& IncomingEdgeV = IncomingTilemap->GetEdgeV(x, y);
                    if (IncomingEdgeV.EdgeType != EFlowTilemapEdgeType::Empty) {
                        BestEdgeV = &IncomingEdgeV;
                    }
                }
            }
            if (BestEdgeH != nullptr) {
                Tilemap->SetEdgeH(x, y, *BestEdgeH);
            }
            if (BestEdgeV != nullptr) {
                Tilemap->SetEdgeV(x, y, *BestEdgeV);
            }
        }
    }
    
    Output.ExecutionResult = EFlowTaskExecutionResult::Success;
}

