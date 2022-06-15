//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Builders/SnapMap/SnapMapDungeonBuilder.h"

#include "Builders/SnapMap/SnapMapAsset.h"
#include "Builders/SnapMap/SnapMapDungeonModel.h"
#include "Builders/SnapMap/SnapMapDungeonQuery.h"
#include "Builders/SnapMap/SnapMapDungeonSelectorLogic.h"
#include "Builders/SnapMap/SnapMapDungeonToolData.h"
#include "Builders/SnapMap/SnapMapDungeonTransformLogic.h"
#include "Core/Dungeon.h"
#include "Core/Utils/DungeonModelHelper.h"
#include "Core/Volumes/DungeonNegationVolume.h"
#include "Core/Volumes/DungeonThemeOverrideVolume.h"
#include "Frameworks/GraphGrammar/GraphGrammar.h"
#include "Frameworks/GraphGrammar/GraphGrammarProcessor.h"
#include "Frameworks/GraphGrammar/Script/GrammarScriptGraph.h"
#include "Frameworks/Snap/Lib/Connection/SnapConnectionActor.h"
#include "Frameworks/Snap/Lib/Streaming/SnapStreaming.h"
#include "Frameworks/Snap/Lib/Utils/SnapDiagnostics.h"
#include "Frameworks/Snap/SnapMap/SnapMapLibrary.h"
#include "Frameworks/ThemeEngine/SceneProviders/PooledDungeonSceneProvider.h"

#include "DrawDebugHelpers.h"
#include "Engine/Level.h"
#include "Engine/LevelStreaming.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/UObjectIterator.h"

DEFINE_LOG_CATEGORY(SnapMapDungeonBuilderLog);

USnapMapDungeonBuilder::USnapMapDungeonBuilder(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
}

void USnapMapDungeonBuilder::BuildNonThemedDungeonImpl(UWorld* World, TSharedPtr<FDungeonSceneProvider> SceneProvider) {
    SnapMapModel = Cast<USnapMapDungeonModel>(model);
    SnapMapConfig = Cast<USnapMapDungeonConfig>(config);

    if (!World || !SnapMapConfig.IsValid() || !SnapMapModel.IsValid()) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Invalid reference passed to the snap builder"));
        return;
    }

    if (!SnapMapConfig->DungeonFlowGraph) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Dungeon Flow asset not specified"));
        return;
    }

    if (!SnapMapConfig->ModuleDatabase) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Module Database asset is not specified"));
        return;
    }

    if (LevelStreamHandler.IsValid()) {
        LevelStreamHandler->ClearStreamingLevels();
        LevelStreamHandler.Reset();
    }
    
    SnapMapModel->Reset();
    WorldMarkers.Reset();
    if (Diagnostics.IsValid()) {
        Diagnostics->Clear();
    }

    const int32 MaxTries = SnapMapConfig->bSupportBuildRetries ? FMath::Max(1, SnapMapConfig->NumBuildRetries) : 1;
    int32 NumTries = 0;

    TSet<int32> ProcessedSeeds;
    int32 Seed = SnapMapConfig->Seed;
    SnapLib::FModuleNodePtr GraphRootNode = nullptr;
    while (NumTries < MaxTries && !GraphRootNode.IsValid()) {
        ProcessedSeeds.Add(Seed);
        GraphRootNode = GenerateModuleNodeGraph(Seed);
        NumTries++;

        if (!GraphRootNode.IsValid()) {
            FRandomStream SeedGenerator(Seed);
            Seed = SeedGenerator.RandRange(0, MAX_int32 - 1);
            while (ProcessedSeeds.Contains(Seed)) {
                Seed++;
            }
        }
    }

    if (!GraphRootNode) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Cannot find a valid snap solution"));
        return;
    }
    
    FSnapMapGraphSerializer::Serialize({ GraphRootNode }, SnapMapModel->ModuleInstances, SnapMapModel->Connections);
    
    if (SnapMapConfig->bGenerateSinglePersistentDungeon)
    {
        BuildPersistentSnapLevel(World, GraphRootNode, SceneProvider);
    }
    else {
        UDungeonLevelStreamingModel* LevelStreamModel = Dungeon ? Dungeon->LevelStreamingModel : nullptr;
        LevelStreamHandler = MakeShareable(new FSnapMapStreamingChunkHandler(GetWorld(), Dungeon, SnapMapModel.Get(), LevelStreamModel));
        LevelStreamHandler->ClearStreamingLevels();

        FGuid SpawnRoomId = GraphRootNode->ModuleInstanceId;
        FSnapStreaming::GenerateLevelStreamingModel(World, { GraphRootNode }, Dungeon->LevelStreamingModel, USnapStreamingChunk::StaticClass(),
            [this, &SpawnRoomId](UDungeonStreamingChunk* InChunk, SnapLib::FModuleNodePtr Node)  {
                USnapStreamingChunk* Chunk = Cast<USnapStreamingChunk>(InChunk);
                if (Chunk) {
                    Chunk->ModuleTransform = Node->WorldTransform;
                    if (Node->ModuleInstanceId == SpawnRoomId) {
                        Chunk->bSpawnRoomChunk = true;
                    }
                    if (LevelStreamHandler.IsValid()) {
                        LevelStreamHandler->RegisterEvents(Chunk);
                    }
                }
            });
    }
}

void USnapMapDungeonBuilder::BuildPersistentSnapLevel(UWorld* InWorld, SnapLib::FModuleNodePtr InGraphRootNode, TSharedPtr<FDungeonSceneProvider> InSceneProvider) {
    if (!Dungeon) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Invalid dungeon state"));
        return;
    }
    
    if (!InWorld) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Invalid world state"));
        return;
    }

    TArray<SnapLib::FModuleNodePtr> ModuleNodes;
    SnapLib::TraverseModuleGraph(InGraphRootNode, [&](SnapLib::FModuleNodePtr Node) {
        ModuleNodes.Add(Node);
    });
    
    TMap<FGuid, TArray<ASnapConnectionActor*>> ModuleConnections;
    const FName DungeonTag = UDungeonModelHelper::GetDungeonIdTag(Dungeon);
    for (const SnapLib::FModuleNodePtr& Node : ModuleNodes) {
        TArray<AActor*> ChunkActors;
        TMap<AActor*, AActor*> TemplateToSpawnedActorMap;
        
        const UWorld* ModuleLevel = Node->ModuleDBItem->GetLevel().LoadSynchronous();
        const FTransform& NodeTransform = Node->WorldTransform;
        const FGuid AbstractNodeId = Node->ModuleInstanceId;
        
        // Spawn the module actors
        for (AActor* ActorTemplate : ModuleLevel->PersistentLevel->Actors) {
            if (!ActorTemplate || ActorTemplate->IsA<AInfo>()) continue;
            AActor* Template = ActorTemplate;
            FTransform ActorTransform = FTransform::Identity;
            if (Template->IsA<ABrush>()) {
                Template = nullptr;
                ActorTransform = ActorTemplate->GetTransform();
            }
            
            FActorSpawnParameters SpawnParams;
            SpawnParams.Template = Template;
            const FTransform SpawnTransform = ActorTransform * NodeTransform;
            AActor* SpawnedModuleActor = InWorld->SpawnActor<AActor>(ActorTemplate->GetClass(), SpawnTransform, SpawnParams);
            if (SpawnedModuleActor) {
                SpawnedModuleActor->Tags.Add(DungeonTag);
                SpawnedModuleActor->Tags.Add(FSceneProviderCommand::TagComplexActor);
            }
            
            ChunkActors.Add(SpawnedModuleActor);
            TemplateToSpawnedActorMap.Add(ActorTemplate, SpawnedModuleActor);
            
            if (ASnapConnectionActor* ConnectionActor = Cast<ASnapConnectionActor>(SpawnedModuleActor)) {
                TArray<ASnapConnectionActor*>& Connections = ModuleConnections.FindOrAdd(AbstractNodeId);
                Connections.Add(ConnectionActor);
            }
        }

        // Replace template actor references
        for (AActor* ChunkActor : ChunkActors) {
            if (!ChunkActor) continue;
            for (TFieldIterator<FProperty> PropertyIterator(ChunkActor->GetClass()); PropertyIterator; ++PropertyIterator) {
                FProperty* Property = *PropertyIterator;
                if (!Property) continue;
                if (const FObjectProperty* ObjProperty = CastField<FObjectProperty>(Property)) {
                    UObject* PropertyObjectValue = ObjProperty->GetObjectPropertyValue_InContainer(ChunkActor);
                    if (PropertyObjectValue && PropertyObjectValue->HasAnyFlags(RF_DefaultSubObject | RF_ArchetypeObject)) {
                        continue;
                    }

                    if (AActor* PropertyActor = Cast<AActor>(PropertyObjectValue)) {
                        AActor** CrossReferencePtr = TemplateToSpawnedActorMap.Find(PropertyActor);
                        if (CrossReferencePtr) {
                            AActor* CrossReference = *CrossReferencePtr;
                            ObjProperty->SetObjectPropertyValue_InContainer(ChunkActor, CrossReference);
                        }
                    }
                }
            }
        } 
    }

    
    // Fix the module connections
    ULevel* PersistentLevel = InWorld->PersistentLevel;
    FSnapMapStreamingChunkHandler ChunkHandler(InWorld, Dungeon, SnapMapModel.Get(), nullptr);    // We'll use this to setup our doors
    TArray<FSnapConnectionInstance> ConnectionInstances = SnapMapModel->Connections;
    for (const auto& Entry : ModuleConnections) {
        FGuid ChunkID = Entry.Key;
        const TArray<ASnapConnectionActor*>& ConnectionActors = Entry.Value;
        ChunkHandler.Internal_SpawnChunkConnections(ChunkID, ConnectionInstances, ConnectionActors, PersistentLevel, PersistentLevel);

        for (ASnapConnectionActor* ConnectionActor : ConnectionActors) {
            if (ConnectionActor) {
                for (AActor* SpawnedConnectionVisualActor : ConnectionActor->GetSpawnedInstances()) {
                    if (SpawnedConnectionVisualActor) {
                        SpawnedConnectionVisualActor->Tags.Add(DungeonTag);
                        SpawnedConnectionVisualActor->Tags.Add(FSceneProviderCommand::TagComplexActor);
                    }
                }
            }
        } 
    }
}

namespace {
    void PopulateNegationVolumeBounds(ADungeon* InDungeon, TArray<SnapLib::FSnapNegationVolumeState>& OutNegationVolumes) {
        UWorld* World = InDungeon ? InDungeon->GetWorld() : nullptr;
        if (!World) return;

        // Grab the bounds of all the negation volumes
        for (TObjectIterator<ADungeonNegationVolume> NegationVolume; NegationVolume; ++NegationVolume) {
            if (!NegationVolume->IsValidLowLevel() || !IsValid(*NegationVolume)) {
                continue;
            }
            if (NegationVolume->Dungeon != InDungeon) {
                continue;
            }

            FVector Origin, Extent;
            NegationVolume->GetActorBounds(false, Origin, Extent);

            SnapLib::FSnapNegationVolumeState State;
            State.Bounds = FBox(Origin - Extent, Origin + Extent);
            State.bInverse = NegationVolume->Reversed;

            OutNegationVolumes.Add(State);
        }
    }
}

SnapLib::FModuleNodePtr USnapMapDungeonBuilder::GenerateModuleNodeGraph(int32 InSeed) const {
    UGrammarScriptGraph* MissionGraph = NewObject<UGrammarScriptGraph>(SnapMapModel.Get());
    SnapMapModel->MissionGraph = MissionGraph;

    UGraphGrammar* MissionGrammar = SnapMapConfig->DungeonFlowGraph->MissionGrammar;

    // build the mission graph from the mission grammar rules
    {
        FGraphGrammarProcessor GraphGrammarProcessor;
        GraphGrammarProcessor.Initialize(MissionGraph, MissionGrammar, InSeed);
        GraphGrammarProcessor.Execute(MissionGraph, MissionGrammar);
    }

    SnapLib::FGrowthStaticState StaticState;
    StaticState.Random = Random;
    StaticState.BoundsContraction = SnapMapConfig->CollisionTestContraction;
    StaticState.DungeonBaseTransform = Dungeon
                                           ? FTransform(FRotator::ZeroRotator, Dungeon->GetActorLocation())
                                           : FTransform::Identity;
    StaticState.StartTimeSecs = FPlatformTime::Seconds();
    StaticState.MaxProcessingTimeSecs = SnapMapConfig->MaxProcessingTimeSecs;
    StaticState.bAllowModuleRotations = SnapMapConfig->bAllowModuleRotations;
    StaticState.Diagnostics = Diagnostics;

    PopulateNegationVolumeBounds(Dungeon, StaticState.NegationVolumes);

    SnapLib::IModuleDatabasePtr ModDB = MakeShareable(new FSnapMapModuleDatabaseImpl(SnapMapConfig->ModuleDatabase));
    SnapLib::FSnapGraphGenerator GraphGenerator(ModDB, StaticState);
    SnapLib::ISnapGraphNodePtr StartNode = MakeShareable(new FSnapGraphGrammarNode(MissionGraph->FindRootNode()));
    return GraphGenerator.Generate(StartNode);
}

bool USnapMapDungeonBuilder::IdentifyBuildSucceeded() const {
    return SnapMapModel.IsValid() && SnapMapModel->ModuleInstances.Num() > 0;
}

void USnapMapDungeonBuilder::BuildPreviewSnapLayout() {
    SnapMapModel = Cast<USnapMapDungeonModel>(model);
    SnapMapConfig = Cast<USnapMapDungeonConfig>(config);

    UWorld* World = Dungeon ? Dungeon->GetWorld() : nullptr;

    if (!World || !SnapMapConfig.IsValid() || !SnapMapModel.IsValid()) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Invalid reference passed to the snap builder"));
        return;
    }

    if (!SnapMapConfig->DungeonFlowGraph) {
        UE_LOG(SnapMapDungeonBuilderLog, Error, TEXT("Dungeon Flow asset not specified"));
        return;
    }

    const int32 Seed = config->Seed;
    Random.Initialize(Seed);
    SnapMapModel->Reset();
    if (LevelStreamHandler.IsValid()) {
        LevelStreamHandler->ClearStreamingLevels();
    }
    WorldMarkers.Reset();

    const SnapLib::FModuleNodePtr GraphRootNode = GenerateModuleNodeGraph(Seed);
    FSnapMapGraphSerializer::Serialize({ GraphRootNode }, SnapMapModel->ModuleInstances, SnapMapModel->Connections);
}


void USnapMapDungeonBuilder::DestroyNonThemedDungeonImpl(UWorld* World) {
    UDungeonBuilder::DestroyNonThemedDungeonImpl(World);

    SnapMapModel = Cast<USnapMapDungeonModel>(model);
    SnapMapConfig = Cast<USnapMapDungeonConfig>(config);

    if (LevelStreamHandler.IsValid()) {
        LevelStreamHandler->ClearStreamingLevels();
        LevelStreamHandler.Reset();
    }

    SnapMapModel->Reset();

    if (Diagnostics.IsValid()) {
        Diagnostics->Clear();
    }
}

void USnapMapDungeonBuilder::GetSnapConnectionActors(ULevel* ModuleLevel,
                                                        TArray<ASnapConnectionActor*>& OutConnectionActors) {
    OutConnectionActors.Reset();
    for (AActor* Actor : ModuleLevel->Actors) {
        if (!Actor) continue;
        if (ASnapConnectionActor* ConnectionActor = Cast<ASnapConnectionActor>(Actor)) {
            OutConnectionActors.Add(ConnectionActor);
        }
    }
}

void USnapMapDungeonBuilder::SetDiagnostics(TSharedPtr<SnapLib::FDiagnostics> InDiagnostics) {
    Diagnostics = InDiagnostics;
}

void USnapMapDungeonBuilder::DrawDebugData(UWorld* InWorld, bool bPersistent /*= false*/, float LifeTime /*= 0*/) {
    TMap<FGuid, TArray<FSnapConnectionInstance>> ConnectionsByModuleId;
    USnapMapDungeonModel* DModel = Cast<USnapMapDungeonModel>(model);
    if (DModel) {
        for (const FSnapConnectionInstance& Connection : DModel->Connections) {
            TArray<FSnapConnectionInstance>& ModuleConnections = ConnectionsByModuleId.FindOrAdd(Connection.ModuleA);
            ModuleConnections.Add(Connection);
        }
    }

    if (Dungeon && Dungeon->LevelStreamingModel) {
        for (UDungeonStreamingChunk* Chunk : Dungeon->LevelStreamingModel->Chunks) {
            if (!Chunk) continue;

            // Draw the bounds
            FVector Center, Extent;
            Chunk->Bounds.GetCenterAndExtents(Center, Extent);
            DrawDebugBox(InWorld, Center, Extent, FQuat::Identity, FColor::Red, false, -1, 0, 10);

            // Draw the connections to the doors
            float AvgZ = 0;
            const FVector CylinderOffset = FVector(0, 0, 10);
            TArray<FSnapConnectionInstance>& ModuleConnections = ConnectionsByModuleId.FindOrAdd(Chunk->ID);
            for (const FSnapConnectionInstance& Connection : ModuleConnections) {
                FVector Location = Connection.WorldTransform.GetLocation();
                DrawDebugCylinder(InWorld, Location, Location + CylinderOffset, 50, 8, FColor::Red, false, -1, 0, 8);
                AvgZ += Location.Z;
            }

            if (ModuleConnections.Num() > 0) {
                AvgZ /= ModuleConnections.Num();
                FVector CenterPoint = Center;
                CenterPoint.Z = AvgZ;

                DrawDebugCylinder(InWorld, CenterPoint, CenterPoint + CylinderOffset, 80, 16, FColor::Green, false, -1,
                                  0, 4);

                // Draw a point in the center of the room
                for (const FSnapConnectionInstance& Connection : ModuleConnections) {
                    FVector Start = CenterPoint + CylinderOffset / 2.0f;
                    FVector End = Connection.WorldTransform.GetLocation() + CylinderOffset / 2.0f;
                    DrawDebugLine(InWorld, Start, End, FColor::Green, false, -1, 0, 20);
                }

            }
        }
    }
}

TSubclassOf<UDungeonModel> USnapMapDungeonBuilder::GetModelClass() {
    return USnapMapDungeonModel::StaticClass();
}

TSubclassOf<UDungeonConfig> USnapMapDungeonBuilder::GetConfigClass() {
    return USnapMapDungeonConfig::StaticClass();
}

TSubclassOf<UDungeonToolData> USnapMapDungeonBuilder::GetToolDataClass() {
    return USnapMapDungeonToolData::StaticClass();
}

TSubclassOf<UDungeonQuery> USnapMapDungeonBuilder::GetQueryClass() {
    return USnapMapDungeonQuery::StaticClass();
}

bool USnapMapDungeonBuilder::ProcessSpatialConstraint(UDungeonSpatialConstraint* SpatialConstraint,
                                                      const FTransform& Transform, FQuat& OutRotationOffset) {
    return false;
}

bool USnapMapDungeonBuilder::SupportsProperty(const FName& PropertyName) const {
    if (PropertyName == "Themes") return false;
    if (PropertyName == "MarkerEmitters") return false;
    if (PropertyName == "EventListeners") return false;

    return true;
}

TSharedPtr<class FDungeonSceneProvider> USnapMapDungeonBuilder::CreateSceneProvider(
    UDungeonConfig* Config, ADungeon* pDungeon, UWorld* World) {
    return MakeShareable(new FPooledDungeonSceneProvider(pDungeon, World));
}

bool USnapMapDungeonBuilder::CanBuildDungeon(FString& OutMessage) {
    ADungeon* OuterDungeon = Cast<ADungeon>(GetOuter());
    if (OuterDungeon) {
        SnapMapConfig = Cast<USnapMapDungeonConfig>(OuterDungeon->GetConfig());

        if (!SnapMapConfig.IsValid()) {
            OutMessage = "Dungeon not initialized correctly";
            return false;
        }

        if (!SnapMapConfig->DungeonFlowGraph) {
            OutMessage = "Dungeon Flow asset not assigned";
            return false;
        }

        if (!SnapMapConfig->ModuleDatabase) {
            OutMessage = "Module Database asset not assigned";
            return false;
        }
    }
    else {
        OutMessage = "Dungeon not initialized correctly";
        return false;
    }

    return true;
}

bool USnapMapDungeonBuilder::PerformSelectionLogic(const TArray<UDungeonSelectorLogic*>& SelectionLogics,
                                                   const FDAMarkerInfo& socket) {
    return false;
}

FTransform USnapMapDungeonBuilder::PerformTransformLogic(const TArray<UDungeonTransformLogic*>& TransformLogics,
                                                         const FDAMarkerInfo& socket) {
    return FTransform::Identity;
}

///////////////////////////////////// FSnapMapStreamingChunkHandler /////////////////////////////////////
FSnapMapStreamingChunkHandler::FSnapMapStreamingChunkHandler(UWorld* InWorld, TWeakObjectPtr<ADungeon> InDungeon, USnapMapDungeonModel* InSnapMapModel, UDungeonLevelStreamingModel* InLevelStreamingModel)
        : FSnapStreamingChunkHandlerBase(InWorld, InDungeon, InLevelStreamingModel)
        , SnapMapModel(InSnapMapModel)
{
}

TArray<FSnapConnectionInstance>* FSnapMapStreamingChunkHandler::GetConnections() const {
    return SnapMapModel.IsValid() ? &SnapMapModel->Connections : nullptr;
}

