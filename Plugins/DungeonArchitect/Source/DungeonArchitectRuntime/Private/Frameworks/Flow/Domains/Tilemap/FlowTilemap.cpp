//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Flow/Domains/Tilemap/FlowTilemap.h"

#include "Containers/Queue.h"

/////////////////////////////////// UFlowTilemap ///////////////////////////////////

const FName UFlowTilemap::StateTypeID = TEXT("TilemapObject");
void UFlowTilemap::Initialize(int32 InWidth, int32 InHeight) {
    Width = InWidth;
    Height = InHeight;
    Cells.SetNum(Width * Height);

    for (int y = 0; y < InHeight; y++) {
        for (int x = 0; x < InWidth; x++) {
            Cells[CELL_INDEX(x, y)].TileCoord = FIntPoint(x, y);
        }
    }

    {
        const int32 EdgesWidth = Width + 1;
        const int32 EdgesHeight = Height + 1;
        EdgesHorizontal.SetNum(EdgesWidth * EdgesHeight);
        EdgesVertical.SetNum(EdgesWidth * EdgesHeight);
        for (int y = 0; y < EdgesHeight; y++) {
            for (int x = 0; x < EdgesWidth; x++) {
                // Update the horizontal edges
                const FIntPoint Coord(x, y);
                EdgesHorizontal[EDGE_INDEX(x, y)].EdgeCoord = FFlowTilemapCoord(Coord, true);
                EdgesVertical[EDGE_INDEX(x, y)].EdgeCoord = FFlowTilemapCoord(Coord, false);
            }
        }
    }

    WallMetadataMap.Reset();
    DoorMetadataMap.Reset();
}

void UFlowTilemap::SetWallMetadata(const FFlowTilemapCoord& Coord, const FFlowTilemapCellWallInfo& InWallMeta) {
    FFlowTilemapCellWallInfo& WallMeta = WallMetadataMap.FindOrAdd(Coord);
    WallMeta = InWallMeta;
}

void UFlowTilemap::SetDoorMetadata(const FFlowTilemapCoord& Coord, const FFlowTilemapCellDoorInfo& InDoorMeta) {
    FFlowTilemapCellDoorInfo& DoorMeta = DoorMetadataMap.FindOrAdd(Coord);
    DoorMeta = InDoorMeta;
}

bool UFlowTilemap::GetWallMeta(const FFlowTilemapCoord& Coord, FFlowTilemapCellWallInfo& OutData) const {
    const FFlowTilemapCellWallInfo* SearchResult = WallMetadataMap.Find(Coord);
    if (!SearchResult) return false;
    OutData = *SearchResult;
    return true;
}

bool UFlowTilemap::GetDoorMeta(const FFlowTilemapCoord& Coord, FFlowTilemapCellDoorInfo& OutData) const {
    const FFlowTilemapCellDoorInfo* SearchResult = DoorMetadataMap.Find(Coord);
    if (!SearchResult) return false;
    OutData = *SearchResult;
    return true;

}

/////////////////////////////// FFlowTilemapDistanceField /////////////////////////////// 

FFlowTilemapDistanceField::FFlowTilemapDistanceField(UFlowTilemap* Tilemap) {
    if (Tilemap) {
        InitializeArray2D(Tilemap->GetWidth(), Tilemap->GetHeight());
    }

    FindDistanceFromEdge(Tilemap);
    FindDistanceFromDoor(Tilemap);
}

namespace {
    const int32 TileChildOffsets[] = {
        -1, 0,
        1, 0,
        0, -1,
        0, 1
    };
}

void FFlowTilemapDistanceField::FindDistanceFromEdge(UFlowTilemap* Tilemap) {
    TQueue<FIntPoint> Queue;
    for (int y = 0; y < Tilemap->GetHeight(); y++) {
        for (int x = 0; x < Tilemap->GetWidth(); x++) {
            FFlowTilemapCell& Cell = Tilemap->Get(x, y);
            if (Cell.CellType == EFlowTilemapCellType::Floor) {
                bool bAllNeighborsWalkable = true;
                for (int i = 0; i < 4; i++) {
                    int32 cx = x + TileChildOffsets[i * 2 + 0];
                    int32 cy = y + TileChildOffsets[i * 2 + 1];
                    FFlowTilemapCell* ncell = Tilemap->GetSafe(cx, cy);
                    if (!ncell) continue;

                    if (ncell->CellType != EFlowTilemapCellType::Floor) {
                        bAllNeighborsWalkable = false;
                        break;
                    }

                    // Check if there's a blocking overlay
                    if (Cell.Overlay.bEnabled && Cell.Overlay.bTileBlockingOverlay) {
                        bAllNeighborsWalkable = false;
                        break;
                    }
                }

                if (!bAllNeighborsWalkable) {
                    Queue.Enqueue(Cell.TileCoord);
                    Array[INDEX(x, y)].DistanceFromEdge = 0;
                }
            }
        }
    }

    FIntPoint CellCoord;
    while (Queue.Dequeue(CellCoord)) {
        FFlowTilemapCell& Cell = Tilemap->Get(CellCoord.X, CellCoord.Y);

        int32 x = Cell.TileCoord.X;
        int32 y = Cell.TileCoord.Y;
        int32 NDist = Get(x, y).DistanceFromEdge + 1;

        for (int i = 0; i < 4; i++) {
            int32 nx = x + TileChildOffsets[i * 2 + 0];
            int32 ny = y + TileChildOffsets[i * 2 + 1];
            FFlowTilemapCell* NCell = Tilemap->GetSafe(nx, ny);
            if (!NCell) continue;
            bool bWalkableTile = (NCell->CellType == EFlowTilemapCellType::Floor);
            if (bWalkableTile && Cell.Overlay.bEnabled && Cell.Overlay.bTileBlockingOverlay) {
                bWalkableTile = false;
            }

            if (bWalkableTile && NDist < Array[INDEX(nx, ny)].DistanceFromEdge) {
                Array[INDEX(nx, ny)].DistanceFromEdge = NDist;
                Queue.Enqueue(NCell->TileCoord);
            }
        }
    }
}

void FFlowTilemapDistanceField::FindDistanceFromDoor(UFlowTilemap* Tilemap) {
    TQueue<FIntPoint> Queue;
    for (int y = 0; y < Tilemap->GetHeight(); y++) {
        for (int x = 0; x < Tilemap->GetWidth(); x++) {
            FFlowTilemapCell& Cell = Tilemap->Get(x, y);
            if (Cell.CellType == EFlowTilemapCellType::Door) {
                Queue.Enqueue(Cell.TileCoord);
                Array[INDEX(x, y)].DistanceFromDoor = 0;
            }
        }
    }

    FIntPoint CellCoord;
    while (Queue.Dequeue(CellCoord)) {
        FFlowTilemapCell& Cell = Tilemap->Get(CellCoord.X, CellCoord.Y);

        int32 x = CellCoord.X;
        int32 y = CellCoord.Y;
        int32 ndist = Array[INDEX(x, y)].DistanceFromDoor + 1;

        for (int i = 0; i < 4; i++) {
            int32 nx = x + TileChildOffsets[i * 2 + 0];
            int32 ny = y + TileChildOffsets[i * 2 + 1];
            FFlowTilemapCell* ncell = Tilemap->GetSafe(nx, ny);
            if (!ncell) continue;
            bool bWalkableTile = (ncell->CellType == EFlowTilemapCellType::Floor);
            if (bWalkableTile && Cell.Overlay.bEnabled && Cell.Overlay.bTileBlockingOverlay) {
                bWalkableTile = false;
            }

            if (bWalkableTile && ndist < Array[INDEX(nx, ny)].DistanceFromDoor) {
                Array[INDEX(nx, ny)].DistanceFromDoor = ndist;
                Queue.Enqueue(ncell->TileCoord);
            }
        }
    }
}



