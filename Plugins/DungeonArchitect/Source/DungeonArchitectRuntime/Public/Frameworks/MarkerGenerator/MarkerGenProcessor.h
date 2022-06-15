//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/DungeonProp.h"

class UDungeonConfig;
class UMarkerGenLayer;
class UMarkerGenPattern;

struct FMGExecEmitMarkerSettings {
	TArray<FString> CopyRotationFromMarkers;
	TArray<FString> CopyHeightFromMarkers;
};

class DUNGEONARCHITECTRUNTIME_API IMGPatternRuleExecutor {
public:
	virtual ~IMGPatternRuleExecutor() {}
	
	// Condition Functions
	virtual bool ContainsMarker(const FString& InMarkerName) const = 0;
	
	// Action Functions
	virtual void EmitMarker(const FString& InMarkerName, const FMGExecEmitMarkerSettings& InSettings) = 0;
	virtual void RemoveMarker(const FString& InMarkerName) = 0;
};

struct DUNGEONARCHITECTRUNTIME_API FMGActionExecContext {
	TSharedPtr<IMGPatternRuleExecutor> Executor;
};

struct DUNGEONARCHITECTRUNTIME_API FMGConditionExecContext {
	TSharedPtr<IMGPatternRuleExecutor> Executor;
}; 

class IMarkerGenProcessor {
public:
	IMarkerGenProcessor(const FTransform& InDungeonTransform) : DungeonTransform(InDungeonTransform) {}
	virtual ~IMarkerGenProcessor() {}
	virtual bool Process(const UMarkerGenLayer* InLayer, const TArray<FDAMarkerInfo>& InMarkers, const FRandomStream& InRandom, TArray<FDAMarkerInfo>& OutMarkers) = 0;

protected:
	FTransform DungeonTransform = FTransform::Identity;
};


class FMarkerGenProcessorFactory {
public:
	static TSharedPtr<IMarkerGenProcessor> Create(const UDungeonConfig* InConfig, const FTransform& InDungeonTransform);
};

