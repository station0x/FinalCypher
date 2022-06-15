//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/Flow/Domains/Tilemap/FlowTilemap.h"
#include "GridFlowTilemap.generated.h"

/////////////////////////////////////// Grid Flow Implementation Structs ///////////////////////////////////////
UCLASS()
class DUNGEONARCHITECTRUNTIME_API UGridFlowTilemap : public UFlowTilemap {
	GENERATED_BODY()
public:
    
};

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UGridFlowTilemapUserData : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
	bool bWallsAsEdges = false;

	static const FName StateTypeID;
};


UENUM(BlueprintType)
enum class EGridFlowAbstractNodeRoomType : uint8 {
	Unknown,
	Room,
	Corridor,
	Cave
};

USTRUCT(BlueprintType)
struct DUNGEONARCHITECTRUNTIME_API FGridFlowAbstractNodeTilemapMetadata {
	GENERATED_BODY()

	/** The coordinate in the tilemap where the chunk representing this layout node starts */
	UPROPERTY()
	FIntPoint TileCoordStart = FIntPoint::ZeroValue;

	/** The coordinate in the tilemap where the chunk representing this layout node end */
	UPROPERTY()
	FIntPoint TileCoordEnd = FIntPoint::ZeroValue;

	UPROPERTY()
	TArray<FIntPoint> Tiles;
};

/**
	Tilemap domain specific data that is attached to the abstract graph nodes
*/
UCLASS()
class DUNGEONARCHITECTRUNTIME_API UFANodeTilemapDomainData : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
	FGridFlowAbstractNodeTilemapMetadata TilemapMetadata;
    
	UPROPERTY()
	bool bDisablePerturb = false;

	UPROPERTY()
	EGridFlowAbstractNodeRoomType RoomType = EGridFlowAbstractNodeRoomType::Unknown;
};

