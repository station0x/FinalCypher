//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Dungeon.h"
#include "Frameworks/ThemeEngine/SceneProviders/DungeonSceneProvider.h"

class DUNGEONARCHITECTRUNTIME_API FSnapThemeSceneProvider : public FDungeonSceneProvider
{
public:
    FSnapThemeSceneProvider(UWorld* InWorld, ADungeon* InDungeon);
    virtual UWorld* GetDungeonWorld() override;

    virtual void AddStaticMesh(UDungeonMesh* Mesh, const FDungeonSceneProviderContext& Context) override;
    virtual void AddLight(UPointLightComponent* LightTemplate, const FDungeonSceneProviderContext& Context) override;
    virtual void AddParticleSystem(UParticleSystem* ParticleTemplate, const FDungeonSceneProviderContext& Context) override;
    virtual void AddActorFromTemplate(UClass* ClassTemplate, const FDungeonSceneProviderContext& Context) override;
    virtual void AddClonedActor(UDungeonActorTemplate* ActorTemplate, const FDungeonSceneProviderContext& Context) override;
    virtual void ExecuteCustomCommand(TSharedPtr<FSceneProviderCommand> SceneCommand) override;
    TArray<TWeakObjectPtr<AActor>> GetSpawnedActors() const { return SpawnedActors; }
    
private:
    void OnActorSpawned(AActor* InActor);
    
private:
    TWeakObjectPtr<UWorld> WorldPtr;
    TArray<TWeakObjectPtr<AActor>> SpawnedActors;
};

