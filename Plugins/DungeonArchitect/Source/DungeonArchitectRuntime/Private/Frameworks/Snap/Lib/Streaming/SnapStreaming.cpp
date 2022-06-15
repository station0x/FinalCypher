//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/Snap/Lib/Streaming/SnapStreaming.h"

#include "Frameworks/Snap/Lib/Connection/SnapConnectionActor.h"
#include "Frameworks/Snap/Lib/Serialization/ConnectionSerialization.h"
#include "Frameworks/Snap/Lib/Serialization/SnapActorSerialization.h"
#include "Frameworks/Snap/Lib/SnapLibrary.h"

#include "Components/ModelComponent.h"
#include "Engine/Engine.h"
#include "EngineUtils.h"

///////////////////////////////////// USnapStreamingChunk /////////////////////////////////////

USnapStreamingChunk::USnapStreamingChunk(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
    SerializedData = ObjectInitializer.CreateDefaultSubobject<USnapStreamingChunkActorData>(this, "SerializedData");
}

void USnapStreamingChunk::HandleChunkVisible() {
    Super::HandleChunkVisible();

    if (OnChunkVisible.IsBound()) {
        OnChunkVisible.Execute(this);
    }

    ULevel* Level = GetLoadedLevel();

    // Serialize the level actor data
    if (Level) {
        SerializedData->LoadLevel(Level);
    }
}

void USnapStreamingChunk::HandleChunkHidden() {
    Super::HandleChunkHidden();

    if (OnChunkHidden.IsBound()) {
        OnChunkHidden.Execute(this);
    }

    ULevel* Level = GetLoadedLevel();
    if (Level) {
        SerializedData->SaveLevel(Level);
    }
}


void USnapStreamingChunk::HandleChunkLoaded() {
    Super::HandleChunkLoaded();

    if (OnChunkLoaded.IsBound()) {
        OnChunkLoaded.Execute(this);
    }
}

void USnapStreamingChunk::HandleChunkUnloaded() {
    Super::HandleChunkUnloaded();

    if (OnChunkUnloaded.IsBound()) {
        OnChunkUnloaded.Execute(this);
    }
}

void USnapStreamingChunk::DestroyChunk(UWorld* InWorld) {
    Super::DestroyChunk(InWorld);
}

FSnapStreamingChunkHandlerBase::FSnapStreamingChunkHandlerBase(TWeakObjectPtr<UWorld> InWorld, TWeakObjectPtr<ADungeon> InDungeon, TWeakObjectPtr<UDungeonLevelStreamingModel> InLevelStreamingModel)
    : World(InWorld)
    , Dungeon(InDungeon)
    , LevelStreamingModel(InLevelStreamingModel)
{
}

///////////////////////////////////// FSnapStreamingChunkHandlerBase /////////////////////////////////////

namespace {
    template <typename T>
    TArray<T*> GetActorsOfType(ULevel* Level) {
        TArray<T*> Result;
        for (int i = 0; i < Level->Actors.Num(); i++) {
            if (T* TargetActor = Cast<T>(Level->Actors[i])) {
                Result.Add(TargetActor);
            }
        }
        return Result;
    }
    
    const FSnapConnectionInstance* FindConnectedDoorInstance(const FSnapConnectionInstance* InConnection,
                                                                const TArray<FSnapConnectionInstance>&
                                                                InConnectionList) {
        if (InConnection) {
            for (const FSnapConnectionInstance& Other : InConnectionList) {
                if (Other.ModuleA == InConnection->ModuleB && Other.DoorA == InConnection->DoorB
                        && Other.ModuleB == InConnection->ModuleA && Other.DoorB == InConnection->DoorA) {
                    return &Other;
                }
            }
        }
        return nullptr;
    }
}

void FSnapStreamingChunkHandlerBase::RegisterEvents(USnapStreamingChunk* InChunk) {
    InChunk->OnChunkVisible.BindSP(SharedThis(this), &FSnapStreamingChunkHandlerBase::OnChunkVisible);
    InChunk->OnChunkHidden.BindSP(SharedThis(this), &FSnapStreamingChunkHandlerBase::OnChunkHidden);
    InChunk->OnChunkLoaded.BindSP(SharedThis(this), &FSnapStreamingChunkHandlerBase::OnChunkLoaded);
    InChunk->OnChunkUnloaded.BindSP(SharedThis(this), &FSnapStreamingChunkHandlerBase::OnChunkUnloaded);
}

void FSnapStreamingChunkHandlerBase::OnChunkVisible(USnapStreamingChunk* Chunk) {
    if (!World.IsValid()) return;

    UpdateChunkDoorStates(Chunk, World->PersistentLevel);

    VisibleModules.Add(Chunk->ID);

    if (LevelStreamingModel.IsValid())
    {
        LevelStreamingModel->NotifyChunkVisible(Chunk);
    }
}

void FSnapStreamingChunkHandlerBase::OnChunkHidden(USnapStreamingChunk* Chunk) {
    HideChunkDoorActors(Chunk);
    
    if (LevelStreamingModel.IsValid())
    {
        LevelStreamingModel->NotifyChunkHidden(Chunk);
    }
}

void FSnapStreamingChunkHandlerBase::Internal_SpawnChunkConnections(const FGuid& ChunkID, TArray<FSnapConnectionInstance>& Connections,
        const TArray<ASnapConnectionActor*>& ConnectionActors, ULevel* DoorLevel, ULevel* WallLevel) const {

    // The connection info for this chunk, by their door ids
    TMap<FGuid, FSnapConnectionInstance*> ModuleConnections;
    for (FSnapConnectionInstance& ConnectionData : Connections) {
        if (ConnectionData.ModuleA == ChunkID) {
            if (!ModuleConnections.Contains(ConnectionData.DoorA)) {
                ModuleConnections.Add(ConnectionData.DoorA, &ConnectionData);
            }
        }
    }

    for (ASnapConnectionActor* ConnectionActor : ConnectionActors) {
        FSnapConnectionInstance** ConnectionDataSearchResult = ModuleConnections.Find(ConnectionActor->GetConnectionId());
        FSnapConnectionInstance* ConnectionData = ConnectionDataSearchResult ? *ConnectionDataSearchResult : nullptr;

        const FSnapConnectionInstance* OtherConnectionData = FindConnectedDoorInstance(ConnectionData, Connections);
        if (ConnectionData && OtherConnectionData) {
            // We have a connection to another chunk node.  This is a door
            
            if (!ConnectionData->bHasSpawnedDoorActor) {
                ConnectionActor->DestroyConnectionInstance();
                UpdateConnectionDoorType(ConnectionData, ConnectionActor->ConnectionComponent);

                // Check if the other door has spawned the shared door actor
                if (OtherConnectionData->bHasSpawnedDoorActor) {
                    ConnectionData->SpawnedDoorActors = OtherConnectionData->SpawnedDoorActors;
                    ConnectionData->bHasSpawnedDoorActor = true;
                }
                else {
                    ConnectionActor->BuildConnectionInstance(Dungeon.Get(), DoorLevel);
                    ConnectionData->SpawnedDoorActors = ConnectionActor->GetSpawnedInstancesPtr();
                    ConnectionData->bHasSpawnedDoorActor = true;
                    OnConnectionDoorCreated(ConnectionData);
                }
            }
            
            // Show the actor in the persistent level (it might have been previously hidden due to level streaming)
            if (ConnectionData->bHasSpawnedDoorActor) {
                for (TWeakObjectPtr<AActor> SpawnedDoorActor : ConnectionData->SpawnedDoorActors) {
                    if (SpawnedDoorActor.IsValid()) {
                        SpawnedDoorActor->SetActorHiddenInGame(false);
                    }
                }
            }
        }
        else {
            // No connection exists. This is a wall
            ConnectionActor->ConnectionComponent->ConnectionState = ESnapConnectionState::Wall;
            ConnectionActor->BuildConnectionInstance(Dungeon.Get(), WallLevel);
        }
    }
}

void FSnapStreamingChunkHandlerBase::UpdateChunkDoorStates(USnapStreamingChunk* Chunk, ULevel* PersistentLevel) const {
    if (!Chunk) {
        return;
    }
    ULevel* ChunkLevel = Chunk->GetLoadedLevel();
    if (!ChunkLevel) {
        return;
    }
    
    TArray<FSnapConnectionInstance>* ConnectionsPtr = GetConnections();
    if (!ConnectionsPtr) {
        return;
    }
    
    const TArray<ASnapConnectionActor*> ConnectionActors = GetActorsOfType<ASnapConnectionActor>(ChunkLevel);
    TArray<FSnapConnectionInstance>& Connections = *ConnectionsPtr;
    Internal_SpawnChunkConnections(Chunk->ID, Connections, ConnectionActors, PersistentLevel, ChunkLevel);
}

void FSnapStreamingChunkHandlerBase::HideChunkDoorActors(USnapStreamingChunk* Chunk) {
    if (!Chunk) {
        return;
    }
    VisibleModules.Remove(Chunk->ID);

    ULevel* ChunkLevel = Chunk->GetLoadedLevel();
    if (!ChunkLevel) {
        return;
    }
    TArray<FSnapConnectionInstance>* ConnectionsPtr = GetConnections();
    if (!ConnectionsPtr) {
        return;
    }
    TArray<FSnapConnectionInstance>& Connections = *ConnectionsPtr;
    
    // The connection info for this chunk, by their door ids
    TMap<FGuid, FSnapConnectionInstance*> ModuleConnections;
    for (FSnapConnectionInstance& ConnectionData : Connections) {
        if (ConnectionData.ModuleA == Chunk->ID) {
            if (!ModuleConnections.Contains(ConnectionData.DoorA)) {
                ModuleConnections.Add(ConnectionData.DoorA, &ConnectionData);
            }
        }
    }

    TArray<ASnapConnectionActor*> ConnectionActors = GetActorsOfType<ASnapConnectionActor>(ChunkLevel);
    for (ASnapConnectionActor* ConnectionActor : ConnectionActors) {
        if (ConnectionActor->ConnectionComponent->ConnectionState == ESnapConnectionState::Wall) {
            // Destroy the wall actors
            ConnectionActor->DestroyConnectionInstance();
        }
        else {
            FSnapConnectionInstance** ConnectionDataSearchResult = ModuleConnections.Find(ConnectionActor->GetConnectionId());
            FSnapConnectionInstance* ConnectionData = ConnectionDataSearchResult ? *ConnectionDataSearchResult : nullptr;

            if (ConnectionData) {
                // We have a door here in the persistent level
                FGuid ConnectedChunkID = (ConnectionData->ModuleA == Chunk->ID) ? ConnectionData->ModuleB : ConnectionData->ModuleA;
                if (!VisibleModules.Contains(ConnectedChunkID)) {
                    // This chunk and the connected chunks are both hidden.   Hide the persistent door actor
                    for (TWeakObjectPtr<AActor> PersistentDoorActor : ConnectionData->SpawnedDoorActors) {
                        if (PersistentDoorActor.IsValid()) {
                            PersistentDoorActor->SetActorHiddenInGame(true);
                        }
                    }
                }   
            }
        }
    }
}

void FSnapStreamingChunkHandlerBase::OnChunkLoaded(USnapStreamingChunk* Chunk) {
    if (LevelStreamingModel.IsValid())
    {
        LevelStreamingModel->NotifyChunkLoaded(Chunk);
    }
}

void FSnapStreamingChunkHandlerBase::OnChunkUnloaded(USnapStreamingChunk* Chunk) {
    if (LevelStreamingModel.IsValid())
    {
        LevelStreamingModel->NotifyChunkUnloaded(Chunk);
    }
}

void FSnapStreamingChunkHandlerBase::ClearStreamingLevels() {
    VisibleModules.Reset();
    
    if (!World.IsValid()) return;
    
    // Destroy the spawned door actors
    {
        TArray<FSnapConnectionInstance>* ConnectionInstances = GetConnections();
        if (ConnectionInstances) {
            for(FSnapConnectionInstance& ConnectionInstance : *ConnectionInstances) {
                for (TWeakObjectPtr<AActor> PersistentDoorActor : ConnectionInstance.SpawnedDoorActors) {
                    if (PersistentDoorActor.IsValid()) {
                        PersistentDoorActor->Destroy();
                        PersistentDoorActor.Reset();    
                    }
                }
                ConnectionInstance.SpawnedDoorActors.Reset();
            }
        }
    }

    if (LevelStreamingModel.IsValid()) {
        // Save a reference to this chunk so they can be removed when destroyed
        LevelStreamingModel->Release(World.Get());
    }

    World->UpdateLevelStreaming();
    World->FlushLevelStreaming(EFlushLevelStreamingType::Full);

    if (GEngine) {
        GEngine->ForceGarbageCollection(true);
    }

}

UDungeonLevelStreamingModel* FSnapStreamingChunkHandlerBase::GetLevelStreamingModel() const
{
    return LevelStreamingModel.Get();
}


///////////////////////////////////// FSnapStreaming /////////////////////////////////////

namespace {
    UDungeonStreamingChunk* GenerateLevelStreamingChunkRecursive(
            UWorld* World, UObject* ChunkOwner, TSubclassOf<UDungeonStreamingChunk> ChunkClass, SnapLib::FModuleNodePtr Node,
            UDungeonStreamingChunk* IncomingChunk, TArray<UDungeonStreamingChunk*>& OutChunks, TMap<SnapLib::FModuleNodePtr, UDungeonStreamingChunk*>& VisitedChunkMap,
            TFunction<void(UDungeonStreamingChunk*, SnapLib::FModuleNodePtr)> FnInitChunk) {
        if (!Node.IsValid()) return nullptr;

        check(!VisitedChunkMap.Contains(Node));

        UDungeonStreamingChunk* Chunk = NewObject<UDungeonStreamingChunk>(ChunkOwner, ChunkClass);
        FnInitChunk(Chunk, Node);
        
        Chunk->ID = Node->ModuleInstanceId;
        Chunk->Bounds = Node->GetModuleBounds();
        VisitedChunkMap.Add(Node, Chunk);

        for (SnapLib::FModuleDoorPtr OutgoingDoor : Node->Outgoing) {
            if (!OutgoingDoor.IsValid() || !OutgoingDoor->ConnectedDoor.IsValid()) {
                continue;
            }
            SnapLib::FModuleNodePtr ChildNode = OutgoingDoor->ConnectedDoor->Owner;
            if (!ChildNode.IsValid()) {
                continue;
            }

            if (!VisitedChunkMap.Contains(ChildNode)) {
                UDungeonStreamingChunk* ChildChunk = GenerateLevelStreamingChunkRecursive(
                    World, ChunkOwner, ChunkClass, ChildNode, Chunk, OutChunks, VisitedChunkMap, FnInitChunk);
                if (ChildChunk) {
                    Chunk->Neighbors.Add(ChildChunk);
                }
            }
            else {
                // This child has already been processed. Grab the child chunk node and add a reference
                UDungeonStreamingChunk* ChildChunk = VisitedChunkMap[ChildNode];
                if (ChildChunk) {
                    Chunk->Neighbors.Add(ChildChunk);
                    ChildChunk->Neighbors.Add(Chunk);
                }
            }
        }
        if (IncomingChunk) {
            Chunk->Neighbors.Add(IncomingChunk);
        }

        // Generate the level streaming object
        {
            const FTransform NodeTransform = Node->WorldTransform;
            const FVector Location = NodeTransform.GetLocation();
            const FRotator Rotation = NodeTransform.GetRotation().Rotator();
            const TSoftObjectPtr<UWorld> ModuleLevelAsset = Node->ModuleDBItem->GetLevel();
            bool bSuccess = false;
            Chunk->CreateLevelStreaming(World, ModuleLevelAsset, Node->NetworkId, Location, Rotation, bSuccess);
        }

        OutChunks.Add(Chunk);

        return Chunk;
    }
}


void FSnapStreamingChunkHandlerBase::UpdateConnectionDoorType(const FSnapConnectionInstance* ConnectionData, USnapConnectionComponent* ConnectionComponent) const {
    ConnectionComponent->ConnectionState = ESnapConnectionState::Door;
}

void FSnapStreaming::GenerateLevelStreamingModel(UWorld* World, const TArray<SnapLib::FModuleNodePtr>& InModuleNodes,
                                                 UDungeonLevelStreamingModel* LevelStreamingModel, TSubclassOf<UDungeonStreamingChunk> ChunkClass,
                                                 TFunction<void(UDungeonStreamingChunk*, SnapLib::FModuleNodePtr)> FnInitChunk) {

    if (!World || !LevelStreamingModel) return;

    LevelStreamingModel->Initialize(World);

    TMap<SnapLib::FModuleNodePtr, UDungeonStreamingChunk*> VisitedChunkMap;
    UObject* ChunkOwner = LevelStreamingModel;
    for (SnapLib::FModuleNodePtr Node : InModuleNodes) {
        if (VisitedChunkMap.Contains(Node)) continue;
        GenerateLevelStreamingChunkRecursive(World, ChunkOwner, ChunkClass, Node, nullptr,
                LevelStreamingModel->Chunks, VisitedChunkMap, FnInitChunk);
    }

    World->UpdateLevelStreaming();
    World->FlushLevelStreaming(EFlushLevelStreamingType::Full);
}

