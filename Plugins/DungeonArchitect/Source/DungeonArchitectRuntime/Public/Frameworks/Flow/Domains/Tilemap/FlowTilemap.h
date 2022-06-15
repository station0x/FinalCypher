//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Utils/Array2D.h"
#include "FlowTilemap.generated.h"

UENUM(BlueprintType)
enum class EFlowTilemapCellType : uint8 {
    Empty,
    Floor,
    Wall,
    Door,
    Custom
};

UENUM(BlueprintType)
enum class EFlowTilemapEdgeType : uint8 {
    Empty,
    Wall,
    Fence,
    Door
};

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapCustomCellInfo {
    GENERATED_BODY()

    UPROPERTY()
    FString MarkerName;

    UPROPERTY()
    FLinearColor DefaultColor = FLinearColor::White;
};

UENUM(BlueprintType)
enum class EFlowTilemapCellOverlayMergeWallOverlayRule : uint8 {
    KeepWallAndOverlay,
    KeepWallRemoveOverlay,
    KeepOverlayRemoveWall
};


USTRUCT()
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapCellOverlayMergeConfig {
    GENERATED_BODY()

    /**
        Minimum height of the tile for the merge to happen

        Variable Name: MinHeight
    */
    UPROPERTY(EditAnywhere, Category = "Merge Config")
    float MinHeight = 0;

    /**
        Maximum height of the tile for the merge to happen

        Variable Name: MaxHeight
    */
    UPROPERTY(EditAnywhere, Category = "Merge Config")
    float MaxHeight = 3;

    /**
        Control how the walls are treated when overlays are placed in wall tiles

        Variable Name: [N/A]
    */
    UPROPERTY(EditAnywhere, Category = "Merge Config")
    EFlowTilemapCellOverlayMergeWallOverlayRule WallOverlayRule =
        EFlowTilemapCellOverlayMergeWallOverlayRule::KeepWallAndOverlay;

    /**
        The logical height offset on the marker for tiles inside the dungeon layout

        Variable Name: MarkerHeightOffsetForLayoutTiles
    */
    UPROPERTY(EditAnywhere, Category = "Merge Config")
    float MarkerHeightOffsetForLayoutTiles = 0;

    /**
        The logical height offset on the marker for tiles outside the dungeon layout

        Variable Name: MarkerHeightOffsetForNonLayoutTiles
    */
    UPROPERTY(EditAnywhere, Category = "Merge Config")
    float MarkerHeightOffsetForNonLayoutTiles = 0;

    /**
        Should the elevation marker be removed when this overlay is placed on an elevation tile?

        Variable Name: RemoveElevationMarker
    */
    UPROPERTY(EditAnywhere, Category = "Merge Config")
    bool RemoveElevationMarker = false;
};


USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapCoord {
    GENERATED_BODY()
    
    FFlowTilemapCoord() {}
    explicit FFlowTilemapCoord(const FIntPoint& InCoord)
        : Coord(InCoord)
        , bIsEdgeCoord(false)
    {}
    explicit FFlowTilemapCoord(const FIntPoint& InCoord, bool bInHorizontalEdge)
        : Coord(InCoord)
        , bIsEdgeCoord(true)
        , bHorizontalEdge(bInHorizontalEdge)
    {}
    
    /** The coordinate of the tile in the tilemap */
    UPROPERTY()
    FIntPoint Coord = FIntPoint::ZeroValue;

    /** Indicates the horizontal or the vertical edge on the tile. Ignored if we are referencing a tile and not an edge */
    UPROPERTY()
    bool bIsEdgeCoord = false;

    /** Indicates the horizontal or the vertical edge on the tile. Ignored if we are referencing a tile and not an edge */
    UPROPERTY()
    bool bHorizontalEdge = true;
    
};

FORCEINLINE uint32 GetTypeHash(const FFlowTilemapCoord& Coord) {
    return HashCombine(
        GetTypeHash(FIntVector(Coord.Coord.X, Coord.Coord.Y, Coord.bIsEdgeCoord ? 0 : 1)),
        Coord.bHorizontalEdge ? 0 : 1);
}
FORCEINLINE bool operator==(const FFlowTilemapCoord& A, const FFlowTilemapCoord& B) {
    return A.Coord == B.Coord
    && A.bIsEdgeCoord == B.bIsEdgeCoord
    && A.bHorizontalEdge == B.bHorizontalEdge;
}

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapEdge {
    GENERATED_BODY()
    
    UPROPERTY()
    EFlowTilemapEdgeType EdgeType = EFlowTilemapEdgeType::Empty;
    
    UPROPERTY()
    FGuid ItemId;
    
    UPROPERTY()
    float HeightCoord = 0;
    
    UPROPERTY()
    float MarkerAngle = 0;
    
    UPROPERTY()
    bool bHasItem = false;
    
    UPROPERTY()
    FFlowTilemapCoord EdgeCoord;
};

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapCellOverlay {
    GENERATED_BODY()

    UPROPERTY()
    bool bEnabled = false;

    UPROPERTY()
    FString MarkerName;

    UPROPERTY()
    FLinearColor Color = FLinearColor::White;

    UPROPERTY()
    float NoiseValue = 0;

    UPROPERTY()
    bool bTileBlockingOverlay = true;

    UPROPERTY()
    FFlowTilemapCellOverlayMergeConfig MergeConfig;
};

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapCell {
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    EFlowTilemapCellType CellType = EFlowTilemapCellType::Empty;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    FFlowTilemapCustomCellInfo CustomCellInfo;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    bool bHasItem = false;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    FGuid ItemId;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    TArray<FString> Tags;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    bool bHasOverlay = false;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    FFlowTilemapCellOverlay Overlay;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    FVector ChunkCoord = FVector::ZeroVector;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    FIntPoint TileCoord = FIntPoint::ZeroValue;

    UPROPERTY()
    bool bUseCustomColor = false;

    UPROPERTY()
    FLinearColor CustomColor = FLinearColor::White;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    bool bMainPath = false;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    bool bLayoutCell = false;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    bool bUnreachable = false;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    int DistanceFromMainPath = MAX_int32;

    UPROPERTY(BlueprintReadOnly, Category = "Dungeon")
    float Height = 0;

};

USTRUCT()
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapCellDoorInfo {
    GENERATED_BODY()

    UPROPERTY()
    bool bLocked = false;

    UPROPERTY()
    bool bOneWay = false;

    UPROPERTY()
    FIntVector ChunkA = FIntVector::ZeroValue;

    UPROPERTY()
    FIntVector ChunkB = FIntVector::ZeroValue;

    UPROPERTY()
    float Angle = 0;
};

USTRUCT()
struct DUNGEONARCHITECTRUNTIME_API FFlowTilemapCellWallInfo {
    GENERATED_BODY()

    UPROPERTY()
    TArray<FIntPoint> OwningTiles;
};

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UFlowTilemap : public UObject {
    GENERATED_BODY()

public:
    void Initialize(int32 InWidth, int32 InHeight);

    FORCEINLINE FFlowTilemapCell& Get(int32 X, int32 Y) { return Cells[CELL_INDEX(X, Y)]; }
    FORCEINLINE const FFlowTilemapCell& Get(int32 X, int32 Y) const { return Cells[CELL_INDEX(X, Y)]; }

    FORCEINLINE FFlowTilemapEdge& GetEdgeH(int32 X, int32 Y) { return EdgesHorizontal[EDGE_INDEX(X, Y)]; }
    FORCEINLINE const FFlowTilemapEdge& GetEdgeH(int32 X, int32 Y) const { return EdgesHorizontal[EDGE_INDEX(X, Y)]; }
    
    FORCEINLINE FFlowTilemapEdge& GetEdgeV(int32 X, int32 Y) { return EdgesVertical[EDGE_INDEX(X, Y)]; }
    FORCEINLINE const FFlowTilemapEdge& GetEdgeV(int32 X, int32 Y) const { return EdgesVertical[EDGE_INDEX(X, Y)]; }
    
    FORCEINLINE void Set(int32 X, int32 Y, const FFlowTilemapCell& Value) {
        check(Value.TileCoord == FIntPoint(X, Y));
        Cells[CELL_INDEX(X, Y)] = Value;
    }
    FORCEINLINE void SetEdgeH(int32 X, int32 Y, const FFlowTilemapEdge& Value) {
        check(Value.EdgeCoord == FFlowTilemapCoord(FIntPoint(X, Y), true));
        EdgesHorizontal[EDGE_INDEX(X, Y)] = Value;
    }
    FORCEINLINE void SetEdgeV(int32 X, int32 Y, const FFlowTilemapEdge& Value) {
        check(Value.EdgeCoord == FFlowTilemapCoord(FIntPoint(X, Y), false));
        EdgesVertical[EDGE_INDEX(X, Y)] = Value;
    }
    
    FORCEINLINE FFlowTilemapCell* GetSafe(int32 X, int32 Y) { return CELL_INDEX_VALID(X, Y) ? &Cells[CELL_INDEX(X, Y)] : nullptr; }
    FORCEINLINE const FFlowTilemapCell* GetSafe(int32 X, int32 Y) const { return CELL_INDEX_VALID(X, Y) ? &Cells[CELL_INDEX(X, Y)] : nullptr; }

    FORCEINLINE FFlowTilemapEdge* GetEdgeHSafe(int32 X, int32 Y) { return EdgesHorizontal.Num() > 0 && EDGE_INDEX_VALID(X, Y) ? &EdgesHorizontal[EDGE_INDEX(X, Y)] : nullptr; }
    FORCEINLINE const FFlowTilemapEdge* GetEdgeHSafe(int32 X, int32 Y) const { return EdgesHorizontal.Num() > 0 && EDGE_INDEX_VALID(X, Y) ? &EdgesHorizontal[EDGE_INDEX(X, Y)] : nullptr; }
    
    FORCEINLINE FFlowTilemapEdge* GetEdgeVSafe(int32 X, int32 Y) { return EdgesVertical.Num() > 0 && EDGE_INDEX_VALID(X, Y) ? &EdgesVertical[EDGE_INDEX(X, Y)] : nullptr; }
    FORCEINLINE const FFlowTilemapEdge* GetEdgeVSafe(int32 X, int32 Y) const { return EdgesVertical.Num() > 0 && EDGE_INDEX_VALID(X, Y) ? &EdgesVertical[EDGE_INDEX(X, Y)] : nullptr; }

    FORCEINLINE TArray<FFlowTilemapCell>& GetCells() { return Cells; }
    FORCEINLINE const TArray<FFlowTilemapCell>& GetCells() const { return Cells; }
    FORCEINLINE TArray<FFlowTilemapEdge>& GetEdgesH() { return EdgesHorizontal; }
    FORCEINLINE const TArray<FFlowTilemapEdge>& GetEdgesH() const { return EdgesHorizontal; }
    FORCEINLINE TArray<FFlowTilemapEdge>& GetEdgesV() { return EdgesVertical; }
    FORCEINLINE const TArray<FFlowTilemapEdge>& GetEdgesV() const { return EdgesVertical; }
    FORCEINLINE int32 GetWidth() const { return Width; }
    FORCEINLINE int32 GetHeight() const { return Height; }

    void SetWallMetadata(const FFlowTilemapCoord& Coord, const FFlowTilemapCellWallInfo& InWallMeta);
    void SetDoorMetadata(const FFlowTilemapCoord& Coord, const FFlowTilemapCellDoorInfo& InDoorMeta);
    bool GetWallMeta(const FFlowTilemapCoord& Coord, FFlowTilemapCellWallInfo& OutData) const;
    bool GetDoorMeta(const FFlowTilemapCoord& Coord, FFlowTilemapCellDoorInfo& OutData) const;

    const TMap<FFlowTilemapCoord, FFlowTilemapCellWallInfo>& GetWallMetaMap() const { return WallMetadataMap; }
    const TMap<FFlowTilemapCoord, FFlowTilemapCellDoorInfo>& GetDoorMetaMap() const { return DoorMetadataMap; }

    static const FName StateTypeID;
    
private:
    FORCEINLINE int32 CELL_INDEX(int32 X, int32 Y) const { return INDEX_IMPL(X, Y, Width); }
    FORCEINLINE bool CELL_INDEX_VALID(int32 X, int32 Y) const { return INDEX_VALID_IMPL(X, Y, Width, Height); }
    FORCEINLINE int32 EDGE_INDEX(int32 X, int32 Y) const { return INDEX_IMPL(X, Y, Width + 1); }
    FORCEINLINE bool EDGE_INDEX_VALID(int32 X, int32 Y) const { return INDEX_VALID_IMPL(X, Y, Width + 1, Height + 1); }

    FORCEINLINE int32 INDEX_IMPL(int32 X, int32 Y, int32 InWidth) const { return Y * InWidth + X; }
    FORCEINLINE bool INDEX_VALID_IMPL(int32 X, int32 Y, int32 InWidth, int32 InHeight) const { return X >= 0 && Y >= 0 && X < InWidth && Y < InHeight; }
private:
    UPROPERTY()
    TArray<FFlowTilemapCell> Cells;

    UPROPERTY()
    TArray<FFlowTilemapEdge> EdgesHorizontal;

    UPROPERTY()
    TArray<FFlowTilemapEdge> EdgesVertical;

    UPROPERTY()
    int32 Width = 0;

    UPROPERTY()
    int32 Height = 0;

    UPROPERTY()
    TMap<FFlowTilemapCoord, FFlowTilemapCellWallInfo> WallMetadataMap;

    UPROPERTY()
    TMap<FFlowTilemapCoord, FFlowTilemapCellDoorInfo> DoorMetadataMap;

};

struct FFlowTilemapDistanceFieldCell {
    int32 DistanceFromEdge = MAX_int32;
    int32 DistanceFromDoor = MAX_int32;
};

class DUNGEONARCHITECTRUNTIME_API FFlowTilemapDistanceField
    : public TDAArray2D<FFlowTilemapDistanceFieldCell> {
public:
    FFlowTilemapDistanceField(UFlowTilemap* Tilemap);

private:
    void FindDistanceFromEdge(UFlowTilemap* Tilemap);
    void FindDistanceFromDoor(UFlowTilemap* Tilemap);
};

