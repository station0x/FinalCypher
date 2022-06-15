//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"

struct DUNGEONARCHITECTRUNTIME_API FGridBuilderMarkers {
    static const FString ST_FENCE;
    static const FString ST_FENCESEPARATOR;
    static const FString ST_DOOR;
    static const FString ST_WALL;
    static const FString ST_WALLSEPARATOR;
    static const FString ST_GROUND;
    static const FString ST_STAIR;
    static const FString ST_STAIR2X;
    static const FString ST_WALLHALF;
    static const FString ST_WALLHALFSEPARATOR;
    static const FString ST_NONE;

    static const FString ST_ROOMWALL;
    static const FString ST_ROOMWALLSEPARATOR;
    static const FString ST_ROOMOPENSPACE;
    static const FString ST_LIGHT;
};

struct DUNGEONARCHITECTRUNTIME_API FGridFlowBuilderMarkers {
    static const FString MARKER_GROUND;
    static const FString MARKER_WALL;
    static const FString MARKER_WALL_L;
    static const FString MARKER_WALL_T;
    static const FString MARKER_WALL_X;
    static const FString MARKER_DOOR;
    static const FString MARKER_DOOR_ONEWAY;
    static const FString MARKER_CAVE_FENCE;
    static const FString MARKER_CAVE_FENCE_SEPARATOR;
    static const FString MARKER_WALL_SEPARATOR;
    static const FString MARKER_FENCE;
    static const FString MARKER_FENCE_SEPARATOR;
};

struct DUNGEONARCHITECTRUNTIME_API FSimpleCityBuilderMarkers {
    static const FString MarkerHouse;
    static const FString MarkerPark;
    static const FString MarkerRoadX;
    static const FString MarkerRoadT;
    static const FString MarkerRoadS;
    static const FString MarkerRoadCorner;
    static const FString MarkerRoadEnd;
    static const FString MarkerRoad;
};

struct DUNGEONARCHITECTRUNTIME_API FFloorPlanBuilderMarkers {
    static const FString MARKER_GROUND;
    static const FString MARKER_CEILING;
    static const FString MARKER_WALL;
    static const FString MARKER_WALL_SEPARATOR;
    static const FString MARKER_DOOR;
    static const FString MARKER_BUILDING_WALL;
    static const FString MARKER_BUILDING_WALL_SEPARATOR;
};

struct DUNGEONARCHITECTRUNTIME_API FIsaacBuilderMarkers {
    static const FString ST_FENCE;
    static const FString ST_FENCESEPARATOR;
    static const FString ST_DOOR;
    static const FString ST_WALL;
    static const FString ST_WALLSEPARATOR;
    static const FString ST_GROUND;
};

