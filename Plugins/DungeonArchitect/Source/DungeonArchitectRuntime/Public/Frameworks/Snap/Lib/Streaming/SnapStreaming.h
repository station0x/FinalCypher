//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/LevelStreaming/DungeonLevelStreamingModel.h"
#include "Frameworks/Snap/Lib/Connection/SnapConnectionComponent.h"
#include "SnapStreaming.generated.h"

class USnapStreamingChunk;
class USnapStreamingChunkActorData;
class ASnapConnectionActor;

DECLARE_DELEGATE_OneParam(FSnapChunkEvent, USnapStreamingChunk*);
UCLASS()
class DUNGEONARCHITECTRUNTIME_API USnapStreamingChunk : public UDungeonStreamingChunk {
    GENERATED_UCLASS_BODY()

public:
    FSnapChunkEvent OnChunkLoaded;
    FSnapChunkEvent OnChunkUnloaded;
    FSnapChunkEvent OnChunkVisible;
    FSnapChunkEvent OnChunkHidden;

    UPROPERTY()
    FTransform ModuleTransform;

public:
    //// UDungeonStreamingChunk Interface ////
    virtual void HandleChunkVisible() override;
    virtual void HandleChunkLoaded() override;
    virtual void HandleChunkUnloaded() override;
    virtual void HandleChunkHidden() override;
    virtual void DestroyChunk(UWorld* InWorld) override;
    //// End of UDungeonStreamingChunk Interface ////

private:
    UPROPERTY()
    USnapStreamingChunkActorData* SerializedData;
};

class FSnapStreamingChunkHandlerBase : public TSharedFromThis<FSnapStreamingChunkHandlerBase> {
public:
    FSnapStreamingChunkHandlerBase(TWeakObjectPtr<UWorld> InWorld, TWeakObjectPtr<ADungeon> InDungeon, TWeakObjectPtr<UDungeonLevelStreamingModel> InLevelStreamingModel);
    virtual ~FSnapStreamingChunkHandlerBase() {}
    void RegisterEvents(USnapStreamingChunk* InChunk);
    void ClearStreamingLevels();
    UDungeonLevelStreamingModel* GetLevelStreamingModel() const;
    UWorld* GetWorld() const { return World.Get(); }


    virtual void OnChunkVisible(USnapStreamingChunk* Chunk);
    virtual void OnChunkHidden(USnapStreamingChunk* Chunk);
    virtual void OnChunkLoaded(USnapStreamingChunk* Chunk);
    virtual void OnChunkUnloaded(USnapStreamingChunk* Chunk);
    virtual TArray<struct FSnapConnectionInstance>* GetConnections() const = 0;
    
    void Internal_SpawnChunkConnections(const FGuid& ChunkID, TArray<FSnapConnectionInstance>& Connections,
            const TArray<ASnapConnectionActor*>& ConnectionActors, ULevel* DoorLevel, ULevel* WallLevel) const;
    
private:
    void UpdateChunkDoorStates(USnapStreamingChunk* Chunk, ULevel* PersistentLevel) const;
    void HideChunkDoorActors(USnapStreamingChunk* Chunk);

protected:
    virtual void OnConnectionDoorCreated(FSnapConnectionInstance* ConnectionData) const {}
    virtual void UpdateConnectionDoorType(const FSnapConnectionInstance* ConnectionData, USnapConnectionComponent* ConnectionComponent) const;

protected:
    TWeakObjectPtr<UWorld> World;
    TWeakObjectPtr<ADungeon> Dungeon;
    TWeakObjectPtr<UDungeonLevelStreamingModel> LevelStreamingModel;

private:
    TArray<FGuid> VisibleModules;
};


namespace SnapLib {
    typedef TSharedPtr<struct FModuleNode> FModuleNodePtr;
    typedef TSharedPtr<struct FModuleDoor> FModuleDoorPtr;
};

class DUNGEONARCHITECTRUNTIME_API FSnapStreaming {
public:
    static void GenerateLevelStreamingModel(UWorld* World, const TArray<SnapLib::FModuleNodePtr>& InModuleNodes,
            UDungeonLevelStreamingModel* LevelStreamingModel, TSubclassOf<UDungeonStreamingChunk> ChunkClass,
            TFunction<void(UDungeonStreamingChunk*, SnapLib::FModuleNodePtr)> FnInitChunk);
};

