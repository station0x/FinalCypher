//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

class UTexture2D;
class UTextureRenderTarget2D;
class UFlowTilemap;
struct FFlowTilemapCell;

struct FFlowTilemapRendererTextureSettings {
    UTexture2D* TileTexture = nullptr;
    UTexture2D* OneWayTileTextureUp = nullptr;
    UTexture2D* OneWayTileTextureDown = nullptr;
    UTexture2D* OneWayTileTextureLeft = nullptr;
    UTexture2D* OneWayTileTextureRight = nullptr;
};

struct FFlowTilemapRendererSettings {
    FFlowTilemapRendererTextureSettings Textures;
    int32 TileSize = 10;
    bool bUseTextureTileSize = true;
    bool bDrawGridLines = true;
    FLinearColor GridLineColor = FLinearColor(0, 0, 0, 0.05f);
    FLinearColor BackgroundColor = FLinearColor::Transparent;
    TFunction<bool(const FVector&)> FuncCellSelected = [](const FVector&) -> bool { return false; };
};

class DUNGEONARCHITECTRUNTIME_API FFlowTilemapRenderer {
public:
    static UTextureRenderTarget2D* Create(UFlowTilemap* Tilemap, const FFlowTilemapRendererSettings& Settings);

private:
    static FLinearColor GetCellColor(const FFlowTilemapCell& Cell);

    static FLinearColor GetSelectedCellColor(FLinearColor InColor);
};

