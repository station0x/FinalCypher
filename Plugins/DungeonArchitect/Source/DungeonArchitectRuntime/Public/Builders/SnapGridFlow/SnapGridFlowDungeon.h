//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/DungeonBuilder.h"
#include "Core/DungeonConfig.h"
#include "Core/DungeonModel.h"
#include "Core/DungeonToolData.h"
#include "Core/Utils/Attributes.h"
#include "Frameworks/Flow/FlowProcessor.h"
#include "Frameworks/Snap/Lib/Serialization/ConnectionSerialization.h"
#include "Frameworks/Snap/Lib/Streaming/SnapStreaming.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowGraphSerialization.h"
#include "Frameworks/ThemeEngine/Markers/PlaceableMarker.h"
#include "SnapGridFlowDungeon.generated.h"

class UFlowGraphItem;
class USnapGridFlowAbstractGraph;
class USnapGridFlowModuleDatabase;
class USnapGridFlowAsset;

UCLASS(Blueprintable)
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowConfig : public UDungeonConfig {
    GENERATED_BODY()
public:
   
    /** A module database asset that contains the references of the modules to use for building the dungeon */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    TSoftObjectPtr<USnapGridFlowModuleDatabase> ModuleDatabase;

    /** A flow graph allows you to design the flow of your dungeon paths */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    TSoftObjectPtr<USnapGridFlowAsset> FlowGraph;

    /** Spawn the item actors using a theme file.   Here you'd define markers for every item you specified in the flow graph */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)  
    TSoftObjectPtr<UDungeonThemeAsset> ItemTheme;
    
    /**
     * Supports door categories names. When stitching the modules, only connections with the same door categories will be stitched together
     * This will require more processing power, use it only when required. Disable if not needed, to get faster dungeon builds
     */
    UPROPERTY(EditAnywhere, Category = "Dungeon")
    bool bSupportDoorCategories = false;
    
    /** If the layout graph build fails, it will be retried with another seed multiple tiles based on this count */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    int32 NumLayoutBuildRetries = 50;
    
    /**
    * When choosing modules to stitch,  prefer modules with minimum possible doors.
    * E.g. If a module requires 3 doors,  a module with 4 doors will not be used as a better alternative with 3 doors is available
    * Disable this (bring close to 0) if you want some randomness.   Fully enabling this (setting it to 1.0) will not make some
    * modules show up in the final graph as the version with lesser doors will always show up
    */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon, Meta=(ClampMin="0.0", ClampMax = "1.0", UIMin = "0.0", UIMax = "1.0"))
    float PreferModulesWithMinimumDoors = 0.5f;

    /** Override any of the flow graph variables at runtime */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    TMap<FString, FString> ParameterOverrides;

    /** The number of timeouts allowed while building the dungeon with multiple retries */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    int32 NumTimeoutsRetriesAllowed = 20;
    
    /**
     * Build the entire dungeon in the main level by copying over the module actors to the persistent level. This will disable level streaming
     * and is meant to be used in the editor only.  You may bake your lightmaps with this method
     **/
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Advanced)
    bool bGenerateSinglePersistentDungeon = false;
    
    /**
     * The processing power allotted to the module resolve stage.  This allows the algorithm to bail out early if a solution is not
     * found and make another retry.  Provide a value around (10,000)
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    int32 MaxResolveFrames = 10000;

    /**
     * The system tries to not repeat a module within the last few room depths specified below
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Dungeon)
    int32 NonRepeatingRooms = 3;
};

UCLASS(Blueprintable)
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowModel : public UDungeonModel {
    GENERATED_BODY()
public:
    UPROPERTY()
    TArray<FSnapConnectionInstance> Connections;

    UPROPERTY()
    TArray<FSnapGridFlowModuleInstanceSerializedData> ModuleInstances;
    
    UPROPERTY()
    USnapGridFlowAbstractGraph* AbstractGraph = nullptr;
    
public:
    virtual void Reset() override;
    virtual bool ShouldAutoResetOnBuild() const override { return false; }
    
};

UCLASS(Blueprintable)
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowQuery : public UDungeonQuery {
    GENERATED_BODY()
};

UCLASS(Blueprintable)
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowToolData : public UDungeonToolData {
    GENERATED_BODY()
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowBuilder : public UDungeonBuilder {
    GENERATED_BODY()
public:
    virtual void BuildNonThemedDungeonImpl(UWorld* World, TSharedPtr<FDungeonSceneProvider> SceneProvider) override;
    virtual void DestroyNonThemedDungeonImpl(UWorld* World) override;
    virtual void DrawDebugData(UWorld* InWorld, bool bPersistent = false, float LifeTime = -1.0f) override;
    virtual bool SupportsProperty(const FName& PropertyName) const override;
    virtual bool CanBuildDungeon(FString& OutMessage) override;
    virtual bool SupportsBackgroundTask() const override { return false; }
    virtual bool SupportsTheming() const override { return false; }
    virtual bool SupportsLevelStreaming() const override { return true; }
    virtual bool PerformSelectionLogic(const TArray<UDungeonSelectorLogic*>& SelectionLogics, const FDAMarkerInfo& socket) override;
    virtual FTransform PerformTransformLogic(const TArray<UDungeonTransformLogic*>& TransformLogics, const FDAMarkerInfo& socket) override;
    
    virtual TSubclassOf<UDungeonModel> GetModelClass() override;
    virtual TSubclassOf<UDungeonConfig> GetConfigClass() override;
    virtual TSubclassOf<UDungeonToolData> GetToolDataClass() override;
    virtual TSubclassOf<UDungeonQuery> GetQueryClass() override;
    
protected:
    bool GenerateModuleNodeGraph(TArray<SnapLib::FModuleNodePtr>& OutNodes) const;
    bool ExecuteFlowGraph();
    void CreateDebugVisualizations(const FGuid& DungeonID, const FTransform& InTransform) const;
    void DestroyDebugVisualizations(const FGuid& DungeonID) const;
    void HandleChunkLoaded(USnapStreamingChunk* InChunk);
    void HandleChunkLoadedAndVisible(USnapStreamingChunk* InChunk);
    virtual bool IdentifyBuildSucceeded() const override;
    TArray<AActor*> SpawnItemWithThemeEngine(UFlowGraphItem* ItemInfo, const APlaceableMarkerActor* InMarkerActor);
    TArray<AActor*> SpawnChunkPlaceableMarkers(const TArray<AActor*>& ChunkActors, const FGuid& AbstractNodeId, const FBox& ChunkBounds, const FString& ChunkName);
    void BuildPersistentSnapLevel(UWorld* InWorld, const TArray<SnapLib::FModuleNodePtr>& InModuleNodes, TSharedPtr<FDungeonSceneProvider> InSceneProvider);
    
protected:
    FDAAttributeList AttributeList;
    TWeakObjectPtr<USnapGridFlowModel> SnapGridModel;
    TWeakObjectPtr<USnapGridFlowConfig> SnapGridConfig;
    TWeakObjectPtr<USnapGridFlowQuery> SnapGridQuery;
    TSharedPtr<class FSnapGridFlowStreamingChunkHandler> LevelStreamHandler; 
};


class FSnapGridFlowStreamingChunkHandler : public FSnapStreamingChunkHandlerBase {
public:
    FSnapGridFlowStreamingChunkHandler(UWorld* InWorld, TWeakObjectPtr<ADungeon> InDungeon, USnapGridFlowModel* InSnapGridModel, UDungeonLevelStreamingModel* InLevelStreamingModel);
    virtual TArray<struct FSnapConnectionInstance>* GetConnections() const override;
    virtual void OnChunkVisible(USnapStreamingChunk* Chunk) override;
    virtual void OnChunkHidden(USnapStreamingChunk* Chunk) override;
    virtual void OnChunkLoaded(USnapStreamingChunk* Chunk) override;
    virtual void OnChunkUnloaded(USnapStreamingChunk* Chunk) override;
    
protected:
    virtual void OnConnectionDoorCreated(FSnapConnectionInstance* ConnectionData) const override;
    virtual void UpdateConnectionDoorType(const FSnapConnectionInstance* ConnectionData, USnapConnectionComponent* ConnectionComponent) const override;
    
public:
    TWeakObjectPtr<USnapGridFlowModel> SnapGridModel;
    
    FSnapChunkEvent ChunkFullyLoadedEvent;
    
    FSnapChunkEvent ChunkLoadedEvent;
    FSnapChunkEvent ChunkUnloadedEvent;

private:
    TSet<FGuid> FullyLoadedChunks;
};


class DUNGEONARCHITECTRUNTIME_API FSnapGridFlowProcessDomainExtender : public IFlowProcessDomainExtender {
public:
    FSnapGridFlowProcessDomainExtender(TWeakObjectPtr<USnapGridFlowModuleDatabase> InModuleDatabase, bool bInSupportsDoorCategory)
        : ModuleDatabase(InModuleDatabase)
        , bSupportsDoorCategory(bInSupportsDoorCategory)
    {}
    virtual void ExtendDomains(FFlowProcessor& InProcessor) override;

private:
    TWeakObjectPtr<USnapGridFlowModuleDatabase> ModuleDatabase;
    bool bSupportsDoorCategory;
};

UCLASS(Blueprintable, HideDropDown)
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowSelectorLogic : public UDungeonSelectorLogic {
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintNativeEvent, Category = "Dungeon")
    bool SelectNode(USnapGridFlowModel* Model, USnapGridFlowConfig* Config, USnapGridFlowBuilder* Builder, USnapGridFlowQuery* Query, const FRandomStream& RandomStream, const FTransform& MarkerTransform);
    virtual bool SelectNode_Implementation(USnapGridFlowModel* Model, USnapGridFlowConfig* Config, USnapGridFlowBuilder* Builder, USnapGridFlowQuery* Query, const FRandomStream& RandomStream, const FTransform& MarkerTransform);
};


UCLASS(Blueprintable, HideDropDown)
class DUNGEONARCHITECTRUNTIME_API USnapGridFlowTransformLogic : public UDungeonTransformLogic {
    GENERATED_BODY()
    
public:
    UFUNCTION(BlueprintNativeEvent, Category = "Dungeon")
    void GetNodeOffset(USnapGridFlowModel* Model, USnapGridFlowConfig* Config, USnapGridFlowQuery* Query, const FRandomStream& RandomStream, FTransform& Offset);
    virtual void GetNodeOffset_Implementation(USnapGridFlowModel* Model, USnapGridFlowConfig* Config, USnapGridFlowQuery* Query, const FRandomStream& RandomStream, FTransform& Offset);
};

