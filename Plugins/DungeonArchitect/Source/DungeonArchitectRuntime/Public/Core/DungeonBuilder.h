//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/DungeonModel.h"
#include "Core/DungeonQuery.h"
#include "Core/Utils/PMRandom.h"
#include "Frameworks/ThemeEngine/DungeonThemeAsset.h"

#include "Templates/SubclassOf.h"
#include "DungeonBuilder.generated.h"

typedef TMap<FString, TArray<FPropTypeData>> PropBySocketType_t;
typedef TMap<UDungeonThemeAsset*, PropBySocketType_t> PropBySocketTypeByTheme_t;

DECLARE_LOG_CATEGORY_EXTERN(DungeonBuilderLog, Log, All);

class ADungeon;
class UDungeonMarkerEmitter;
class ADungeonMirrorVolume;
class UDungeonModel;
class UDungeonConfig;
class UDungeonQuery;
class UDungeonToolData;
class UDungeonSpatialConstraint;

/**
*
*/
UCLASS(Blueprintable, abstract)
class DUNGEONARCHITECTRUNTIME_API UDungeonBuilder : public UObject {
    GENERATED_BODY()

public:
    void BuildDungeon(ADungeon* InDungeon, UWorld* World);
    void BuildDungeon(UDungeonModel* InModel, UDungeonConfig* InConfig, UDungeonQuery* InQuery, UWorld* InWorld);

    // Non-themed dungeons
    void BuildNonThemedDungeon(ADungeon* InDungeon, TSharedPtr<class FDungeonSceneProvider> InSceneProvider, UWorld* World);
    void BuildNonThemedDungeon(UDungeonModel* InModel, UDungeonConfig* InConfig, UDungeonQuery* InQuery, TSharedPtr<class FDungeonSceneProvider> InSceneProvider, UWorld* InWorld);
    void DestroyNonThemedDungeon(UDungeonModel* InModel, UDungeonConfig* InConfig, UDungeonQuery* InQuery, ADungeon* InDungeon, UWorld* InWorld);

    void ApplyDungeonTheme(const TArray<UDungeonThemeAsset*>& InThemes, const TArray<FClusterThemeInfo>& InClusteredThemes,
                                   TSharedPtr<FDungeonSceneProvider> InSceneProvider, UWorld* InWorld);
    
    virtual void MirrorDungeon();
    virtual void DrawDebugData(UWorld* InWorld, bool bPersistant = false, float LifeTime = -1.0f) {}

    virtual bool SupportsBackgroundTask() const { return true; }
    virtual TSubclassOf<UDungeonModel> GetModelClass() { return nullptr; }
    virtual TSubclassOf<UDungeonConfig> GetConfigClass() { return nullptr; }
    virtual TSubclassOf<UDungeonToolData> GetToolDataClass() { return nullptr; }
    virtual TSubclassOf<UDungeonQuery> GetQueryClass() { return nullptr; }
    virtual bool SupportsProperty(const FName& PropertyName) const { return true; }
    virtual bool SupportsTheming() const { return true; }
    virtual TSharedPtr<class FDungeonSceneProvider> CreateSceneProvider(UDungeonConfig* Config, ADungeon* pDungeon,
                                                                        UWorld* World);
    virtual bool CanBuildDungeon(FString& OutMessage) { return true; }

    UDungeonModel* GetModel() const { return model; }

    /** Implementations should process the spatial constraint and return true if it passes */
    virtual bool ProcessSpatialConstraint(UDungeonSpatialConstraint* SpatialConstraint, const FTransform& Transform,
                                          FQuat& OutRotationOffset);

    void AddMarker(const FString& SocketType, const FTransform& transform,
                   TSharedPtr<class IDungeonMarkerUserData> InUserData = nullptr);
    void AddMarker(const FString& SocketType, const FTransform& _transform, int count, const FVector& InterOffset,
                   TSharedPtr<class IDungeonMarkerUserData> InUserData = nullptr);
    void AddMarker(TArray<FDAMarkerInfo>& pPropSockets, const FString& SocketType, const FTransform& transform,
                   TSharedPtr<class IDungeonMarkerUserData> InUserData = nullptr);

    virtual void GetDefaultMarkerNames(TArray<FString>& OutMarkerNames) {
    }

    UFUNCTION(BlueprintNativeEvent, Category = "Dungeon")
    void EmitDungeonMarkers();
    virtual void EmitDungeonMarkers_Implementation();

    UFUNCTION(BlueprintCallable, Category = Dungeon)
    void EmitMarker(const FString& SocketType, const FTransform& Transform);


    UFUNCTION(BlueprintCallable, BlueprintPure, Category = Dungeon)
    TArray<FDAMarkerInfo> GetMarkers() const { return WorldMarkers; }

    const TArray<FDAMarkerInfo>& GetMarkerList() const { return WorldMarkers; }

    void EmitCustomMarkers(TArray<UDungeonMarkerEmitter*> MarkerEmitters);

    static UClass* DefaultBuilderClass();
    void ProcessMarkerReplacementVolumes();

    virtual bool SupportsLevelStreaming() const { return false; }
    bool HasBuildSucceeded() const { return bBuildSucceeded; }

    virtual void ProcessThemeItemUserData(TSharedPtr<IDungeonMarkerUserData> UserData, AActor* SpawnedActor);

protected:
    virtual void BuildDungeonImpl(UWorld* World) { };
    virtual void BuildNonThemedDungeonImpl(UWorld* World, TSharedPtr<class FDungeonSceneProvider> SceneProvider) { };
    virtual void DestroyNonThemedDungeonImpl(UWorld* World) { };
    void ProcessMarkerReplacementVolume(class ADungeonMarkerReplaceVolume* MarkerReplaceVolume);
    virtual void MirrorDungeonWithVolume(ADungeonMirrorVolume* MirrorVolume) { }
    virtual bool IdentifyBuildSucceeded() const { return true; }

    void ClearSockets() {
        _SocketIdCounter = 0;
        WorldMarkers.Reset();
    }

    virtual bool PerformSelectionLogic(const TArray<UDungeonSelectorLogic*>& SelectionLogics,
                                       const FDAMarkerInfo& socket) {
        return false;
    }

    virtual FTransform PerformTransformLogic(const TArray<UDungeonTransformLogic*>& TransformLogics,
                                             const FDAMarkerInfo& socket) {
        return FTransform::Identity;
    }


public:
    UPROPERTY()
    UDungeonConfig* config;

protected:
    PMRandom nrandom;
    FRandomStream Random;

    UPROPERTY()
    ADungeon* Dungeon;

    UPROPERTY()
    UDungeonModel* model;

    UPROPERTY()
    UDungeonQuery* query;

    int32 _SocketIdCounter;

    bool bBuildSucceeded = true;

    // The marker list
    TArray<FDAMarkerInfo> WorldMarkers;
};

