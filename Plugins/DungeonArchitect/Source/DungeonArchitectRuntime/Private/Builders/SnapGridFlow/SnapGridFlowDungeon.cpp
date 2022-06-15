//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Builders/SnapGridFlow/SnapGridFlowDungeon.h"

#include "Builders/SnapGridFlow/SnapGridFlowAsset.h"
#include "Core/Dungeon.h"
#include "Core/DungeonEventListener.h"
#include "Core/Utils/DungeonModelHelper.h"
#include "Core/Utils/MathUtils.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraph.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Core/FlowAbstractGraphUtils.h"
#include "Frameworks/Flow/Domains/AbstractGraph/Utils/GridFlowAbstractGraphVisualization.h"
#include "Frameworks/Flow/ExecGraph/FlowExecGraphScript.h"
#include "Frameworks/Flow/ExecGraph/FlowExecTask.h"
#include "Frameworks/Flow/FlowProcessor.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/SnapGridFlowAbstractGraph.h"
#include "Frameworks/FlowImpl/SnapGridFlow/LayoutGraph/SnapGridFlowAbstractGraphDomain.h"
#include "Frameworks/Snap/Lib/Connection/SnapConnectionActor.h"
#include "Frameworks/Snap/Lib/Theming/SnapTheme.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowGraphSerialization.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowLibrary.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleBounds.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowModuleResolver.h"
#include "Frameworks/Snap/SnapGridFlow/SnapGridFlowStats.h"
#include "Frameworks/ThemeEngine/DungeonThemeEngine.h"
#include "Frameworks/ThemeEngine/Markers/PlaceableMarker.h"

#include "EngineUtils.h"

class USnapGridFlowAbstractGraph;
DEFINE_LOG_CATEGORY_STATIC(SnapGridDungeonBuilderLog, Log, All);

///////////////////////////// USnapGridFlowBuilder /////////////////////////////
void USnapGridFlowBuilder::BuildNonThemedDungeonImpl(UWorld* World, TSharedPtr<FDungeonSceneProvider> SceneProvider) {
    SnapGridModel = Cast<USnapGridFlowModel>(model);
    SnapGridConfig = Cast<USnapGridFlowConfig>(config);
    SnapGridQuery = Cast<USnapGridFlowQuery>(query);

    if (!World || !SnapGridConfig.IsValid() || !SnapGridModel.IsValid()) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Invalid reference passed to the snap builder"));
        return;
    }

    USnapGridFlowAsset* FlowAsset = SnapGridConfig->FlowGraph.LoadSynchronous();
    if (!FlowAsset) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Dungeon Flow asset not specified"));
        return;
    }

    USnapGridFlowModuleDatabase* ModuleDatabase = SnapGridConfig->ModuleDatabase.LoadSynchronous();
    if (!ModuleDatabase) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Module Database asset is not specified"));
        return;
    }

    if (FlowAsset->Version != static_cast<int>(ESnapGridFlowAssetVersion::LatestVersion)) {
        UE_LOG(SnapGridDungeonBuilderLog, Warning, TEXT("SGF Flow Graph asset is of an older version. Open and Save it once in the SGF Flow Graph editor to improve performance"));
        FSnapGridFlowAssetRuntimeUpgrader::Upgrade(FlowAsset);
    }
    
    SnapGridModel->Reset();
    WorldMarkers.Reset();
    
    if (LevelStreamHandler.IsValid()) {
        LevelStreamHandler->ClearStreamingLevels();
        LevelStreamHandler.Reset();
    }

    {
        SCOPE_CYCLE_COUNTER(STAT_SGFBuild);
        ExecuteFlowGraph();
    }
    
    TArray<SnapLib::FModuleNodePtr> ModuleGraphNodes;
    {
        SCOPE_CYCLE_COUNTER(STAT_SGFResolve);
        GenerateModuleNodeGraph(ModuleGraphNodes);
    }
    FSnapGridFlowGraphSerializer::Serialize(ModuleGraphNodes, SnapGridModel->ModuleInstances, SnapGridModel->Connections);

    TSet<FGuid> SpawnRoomNodes;
    if (SnapGridModel->AbstractGraph) {
        for (UFlowAbstractNode* Node : SnapGridModel->AbstractGraph->GraphNodes) {
            if (FFlowAbstractGraphUtils::ContainsItem(Node->NodeItems, EFlowGraphItemType::Entrance)) {
                SpawnRoomNodes.Add(Node->NodeId);
            }
        } 
    }

    
    if (SnapGridConfig->bGenerateSinglePersistentDungeon)
    {
        BuildPersistentSnapLevel(World, ModuleGraphNodes, SceneProvider);
    }
    else
    {
        UDungeonLevelStreamingModel* LevelStreamModel = Dungeon ? Dungeon->LevelStreamingModel : nullptr;
        LevelStreamHandler = MakeShareable(new FSnapGridFlowStreamingChunkHandler(GetWorld(), Dungeon, SnapGridModel.Get(), LevelStreamModel));
        LevelStreamHandler->ClearStreamingLevels();
        LevelStreamHandler->ChunkLoadedEvent.BindUObject(this, &USnapGridFlowBuilder::HandleChunkLoaded);
        LevelStreamHandler->ChunkFullyLoadedEvent.BindUObject(this, &USnapGridFlowBuilder::HandleChunkLoadedAndVisible);
    
        FSnapStreaming::GenerateLevelStreamingModel(World, ModuleGraphNodes, Dungeon->LevelStreamingModel, USnapStreamingChunk::StaticClass(),
            [this, &SpawnRoomNodes](UDungeonStreamingChunk* InChunk, SnapLib::FModuleNodePtr Node)  {
                USnapStreamingChunk* Chunk = Cast<USnapStreamingChunk>(InChunk);
                if (Chunk) {
                    Chunk->ModuleTransform = Node->WorldTransform;
                    if (SpawnRoomNodes.Contains(Node->ModuleInstanceId)) {
                        Chunk->bSpawnRoomChunk = true;
                    }
                
                    if (LevelStreamHandler.IsValid()) {
                        LevelStreamHandler->RegisterEvents(Chunk);
                    }
                }
            });
    }
    
    // Create the debug draw actor
    if (Dungeon) {
        if (Dungeon->bDrawDebugData) {
            CreateDebugVisualizations(Dungeon->Uid, Dungeon->GetActorTransform());
        }
        else {
            DestroyDebugVisualizations(Dungeon->Uid);
        }
    }
}

bool USnapGridFlowBuilder::GenerateModuleNodeGraph(TArray<SnapLib::FModuleNodePtr>& OutNodes) const {
    if (!SnapGridConfig.IsValid()) {
        return false;
    }
    
    USnapGridFlowModuleDatabase* ModuleDatabase = SnapGridConfig->ModuleDatabase.LoadSynchronous();
    if (!ModuleDatabase || !ModuleDatabase->ModuleBoundsAsset) {
        return false;
    }
    
    if (!SnapGridModel->AbstractGraph) {
        return false;
    }
    
    FSnapGridFlowModuleResolverSettings ResolveSettings;
    ResolveSettings.Seed = SnapGridConfig->Seed;
    ResolveSettings.ChunkSize = ModuleDatabase->ModuleBoundsAsset->ChunkSize;
    ResolveSettings.ModulesWithMinimumDoorsProbability = SnapGridConfig->PreferModulesWithMinimumDoors;
    ResolveSettings.BaseTransform = Dungeon ? Dungeon->GetActorTransform() : FTransform::Identity;
    ResolveSettings.MaxResolveFrames = SnapGridConfig->MaxResolveFrames;
    ResolveSettings.NonRepeatingRooms = SnapGridConfig->NonRepeatingRooms;
    
    const FSnapGridFlowModuleDatabaseImplPtr ModDB = MakeShareable(new FSnapGridFlowModuleDatabaseImpl(ModuleDatabase));
    const FSnapGridFlowModuleResolver ModuleResolver(ModDB, ResolveSettings);
    
    return ModuleResolver.Resolve(SnapGridModel->AbstractGraph, OutNodes);
}

bool USnapGridFlowBuilder::ExecuteFlowGraph() {
    SnapGridModel->AbstractGraph = nullptr;

    USnapGridFlowAsset* SnapGridFlowAsset = SnapGridConfig->FlowGraph.LoadSynchronous();
    USnapGridFlowModuleDatabase* ModuleDatabase = SnapGridConfig->ModuleDatabase.LoadSynchronous();

    if (!SnapGridFlowAsset) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Missing Grid Flow graph"));
        return false;
    }

    if (!SnapGridFlowAsset->ExecScript) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Invalid Grid Flow graph state. Please resave in editor"));
        return false;
    }

    FFlowProcessor FlowProcessor;

    // Register the domains
    {
        FSnapGridFlowProcessDomainExtender Extender(ModuleDatabase, SnapGridConfig->bSupportDoorCategories);
        Extender.ExtendDomains(FlowProcessor);
    }
    
    const int32 MAX_RETRIES = FMath::Max(1, SnapGridConfig->NumLayoutBuildRetries);
    int32 NumTries = 0;
    int32 NumTimeouts = 0;
    FFlowProcessorResult Result;
    while (NumTries < MAX_RETRIES) {
        FFlowProcessorSettings FlowProcessorSettings;
        FlowProcessorSettings.AttributeList = AttributeList;
        FlowProcessorSettings.SerializedAttributeList = SnapGridConfig->ParameterOverrides;
        Result = FlowProcessor.Process(SnapGridFlowAsset->ExecScript, Random, FlowProcessorSettings);
        NumTries++;
        if (Result.ExecResult == EFlowTaskExecutionResult::Success) {
            break;
        }

        if (Result.ExecResult == EFlowTaskExecutionResult::FailHalt) {
            bool bHalt = true;

            if (Result.FailReason == EFlowTaskExecutionFailureReason::Timeout) {
                NumTimeouts++;
                if (NumTimeouts <= SnapGridConfig->NumTimeoutsRetriesAllowed) {
                    // Continue despite the timeout
                    bHalt = false;
                }
            }

            if (bHalt) {
                break;
            }
        }
    }

    if (Result.ExecResult != EFlowTaskExecutionResult::Success) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Failed to generate grid flow graph"));
        return false;
    }

    if (!SnapGridFlowAsset->ExecScript->ResultNode) {
        UE_LOG(SnapGridDungeonBuilderLog, Error,
               TEXT(
                   "Cannot find result node in the grid flow exec graph. Please resave the grid flow asset in the editor"
               ));
        return false;
    }

    const FGuid ResultNodeId = SnapGridFlowAsset->ExecScript->ResultNode->NodeId;
    if (FlowProcessor.GetNodeExecStage(ResultNodeId) != EFlowTaskExecutionStage::Executed) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Grid Flow Graph execution failed"));
        return false;
    }

    FFlowExecutionOutput ResultNodeState;
    FlowProcessor.GetNodeState(ResultNodeId, ResultNodeState);
    if (ResultNodeState.ExecutionResult != EFlowTaskExecutionResult::Success) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Grid Flow Result node execution did not succeed"));
        return false;
    }

    // Save a copy in the model
    USnapGridFlowAbstractGraph* TemplateGraph = ResultNodeState.State->GetState<USnapGridFlowAbstractGraph>(UFlowAbstractGraphBase::StateTypeID);
    SnapGridModel->AbstractGraph = NewObject<USnapGridFlowAbstractGraph>(SnapGridModel.Get(), "AbstractGraph", RF_NoFlags, TemplateGraph);
    return true;
}

void USnapGridFlowBuilder::DestroyNonThemedDungeonImpl(UWorld* World) {
    UDungeonBuilder::DestroyNonThemedDungeonImpl(World);
    SnapGridModel = Cast<USnapGridFlowModel>(model);
    SnapGridConfig = Cast<USnapGridFlowConfig>(config);


    if (LevelStreamHandler.IsValid()) {
        LevelStreamHandler->ClearStreamingLevels();
        LevelStreamHandler.Reset();
    }
    
    SnapGridModel->Reset();
    
    // Create the debug draw actor of specified
    if (Dungeon) {
        DestroyDebugVisualizations(Dungeon->Uid);
    }
}

void USnapGridFlowBuilder::CreateDebugVisualizations(const FGuid& DungeonID, const FTransform& InTransform) const {
    DestroyDebugVisualizations(DungeonID);

    if (!SnapGridModel.IsValid() || !SnapGridConfig.IsValid()) {
        return;
    }
    USnapGridFlowModuleDatabase* ModuleDatabase = SnapGridConfig->ModuleDatabase.LoadSynchronous();
    if (SnapGridModel->AbstractGraph && ModuleDatabase && ModuleDatabase->ModuleBoundsAsset) {
        const FVector ModuleSize = ModuleDatabase->ModuleBoundsAsset->ChunkSize;
    
        UWorld* World = GetWorld();
        AGridFlowAbstractGraphVisualizer* Visualizer = World->SpawnActor<AGridFlowAbstractGraphVisualizer>();
        Visualizer->DungeonID = DungeonID;
        Visualizer->SetAutoAlignToLevelViewport(true);
        
        // Set the transform
        {
            FTransform VisualizerTransform = InTransform;
            VisualizerTransform.SetScale3D(FVector::OneVector);
            Visualizer->SetActorTransform(VisualizerTransform);
        }
        
        if (SnapGridModel.IsValid() && SnapGridModel->AbstractGraph) {
            FGFAbstractGraphVisualizerSettings Settings;
            //const float ModuleWidth = FMath::Min(ModuleSize.X, ModuleSize.Y);
            Settings.NodeRadius = 60.0f; // ModuleWidth * 0.05;
            Settings.LinkThickness = Settings.NodeRadius * 0.2f;
            Settings.LinkRefThickness = Settings.LinkThickness * 0.5f;
            Settings.NodeSeparationDistance = ModuleSize;
            Settings.bRenderNodeOnCellCenter = true;
            Visualizer->Generate(SnapGridModel->AbstractGraph, Settings);
        }
    }
}

void USnapGridFlowBuilder::DestroyDebugVisualizations(const FGuid& DungeonID) const {
    UWorld* World = GetWorld();
    for (TActorIterator<AGridFlowAbstractGraphVisualizer> It(World); It; ++It) {
        AGridFlowAbstractGraphVisualizer* Visualizer = *It;
        if (Visualizer && Visualizer->DungeonID == DungeonID) {
            Visualizer->Destroy();
        }
    }
}

TArray<AActor*> USnapGridFlowBuilder::SpawnItemWithThemeEngine(UFlowGraphItem* ItemInfo, const APlaceableMarkerActor* InMarkerActor) {
    TArray<AActor*> SpawnedActors;
    if (SnapGridConfig.IsValid() && ItemInfo && Dungeon) {
        const FString MarkerName = ItemInfo->MarkerName.TrimStartAndEnd();
    
        TArray<FDAMarkerInfo> MarkersToEmit;
        FDAMarkerInfo& Marker = MarkersToEmit.AddDefaulted_GetRef();
        Marker.Id = 0;
        Marker.MarkerName = MarkerName;
        Marker.Transform = InMarkerActor->GetActorTransform();

        const TSharedPtr<FSnapThemeSceneProvider> SceneProvider = MakeShareable(new FSnapThemeSceneProvider(GetWorld(), Dungeon));
        SceneProvider->SetLevelOverride(InMarkerActor->GetLevel());
    
        FDungeonThemeEngineSettings ThemeEngineSettings;
        ThemeEngineSettings.Themes = { SnapGridConfig->ItemTheme.LoadSynchronous() };
        ThemeEngineSettings.SceneProvider = SceneProvider;
        if (Dungeon) {
            ThemeEngineSettings.bRoleAuthority = Dungeon->HasAuthority(); 
        }

        FDungeonThemeEngineEventHandlers ThemeEventHandlers;
        ThemeEventHandlers.PerformSelectionLogic = [this](const TArray<UDungeonSelectorLogic*>& SelectionLogics, const FDAMarkerInfo& socket) {
            return PerformSelectionLogic(SelectionLogics, socket);
        };
        
        ThemeEventHandlers.PerformTransformLogic = [this](const TArray<UDungeonTransformLogic*>& TransformLogics, const FDAMarkerInfo& socket) {
            return PerformTransformLogic(TransformLogics, socket);
        };

        ThemeEventHandlers.ProcessSpatialConstraint = [this](UDungeonSpatialConstraint* SpatialConstraint, const FTransform& Transform, FQuat& OutRotationOffset) {
            return ProcessSpatialConstraint(SpatialConstraint, Transform, OutRotationOffset);
        };
        
        ThemeEventHandlers.HandlePostMarkersEmit = [this](TArray<FDungeonMarkerInfo>& MarkersToEmit) {
            DungeonUtils::FDungeonEventListenerNotifier::NotifyMarkersEmitted(Dungeon, MarkersToEmit);
        };
        
        // Invoke the Theme Engine
        FDungeonThemeEngine::Apply(MarkersToEmit, Random, ThemeEngineSettings, ThemeEventHandlers);

        for (TWeakObjectPtr<AActor> SpawnedActor : SceneProvider->GetSpawnedActors()) {
            if (SpawnedActor.IsValid()) {
                SpawnedActors.Add(SpawnedActor.Get());
            }
        }
    }
    return SpawnedActors;
}

void USnapGridFlowBuilder::HandleChunkLoaded(USnapStreamingChunk* InChunk) {
    if (!InChunk) return;
    
    UWorld* World = GetWorld();
    if (World && !World->IsGameWorld()) {
        // We are building this in the editor.
        // Disable the module bounds rendering

        ULevel* Level = InChunk ? InChunk->GetLoadedLevel() : nullptr;
        if (Level) {
            for (AActor* Actor : Level->Actors) {
                if (ASnapGridFlowModuleBoundsActor* BoundsActor = Cast<ASnapGridFlowModuleBoundsActor>(Actor)) {
                    if (BoundsActor->BoundsComponent) {
                        BoundsActor->BoundsComponent->bRenderBounds = false;
                    }
                }
            }
        }
    }
}

TArray<AActor*> USnapGridFlowBuilder::SpawnChunkPlaceableMarkers(const TArray<AActor*>& ChunkActors, const FGuid& AbstractNodeId, const FBox& ChunkBounds, const FString& ChunkName) {
    // Populate the requested marker list from the abstract node 
    TArray<UFlowGraphItem*> Items;
    {
        // Grab the node from the abstract graph. The chunk id will be the same as the abstract node id
        UFlowAbstractNode* AbstractNode = SnapGridModel->AbstractGraph->GetNode(AbstractNodeId);

        // Check if abstract node has any items to spawn
        if (AbstractNode) {
            for (UFlowGraphItem* Item : AbstractNode->NodeItems) {
                if (Item) {
                    Items.Add(Item);
                }
            }
        }
    }

    TArray<APlaceableMarkerActor*> PlaceableMarkers;
    for (AActor* Actor : ChunkActors) {
        if (APlaceableMarkerActor* PlaceableMarker = Cast<APlaceableMarkerActor>(Actor)) {
            PlaceableMarkers.Add(PlaceableMarker);
        }
    }

    TArray<AActor*> AllSpawnedActors;
    
    TMap<APlaceableMarkerActor*, FString> MarkersToEmit;
    const int32 MarkerRandomSeed = HashCombine(SnapGridConfig->Seed, GetTypeHash(ChunkBounds.GetCenter()));
    const FRandomStream RandomMarkerSelection(MarkerRandomSeed);
    FMathUtils::Shuffle(PlaceableMarkers, RandomMarkerSelection);
    for (UFlowGraphItem* ItemInfo : Items) {
        FString Marker = ItemInfo->MarkerName.TrimStartAndEnd();
        if (Marker.Len() == 0) continue;
    
        APlaceableMarkerActor* MarkerActor = nullptr;
        for (APlaceableMarkerActor* PlaceableMarker : PlaceableMarkers) {
            if (PlaceableMarker->GetAllowedMarkerNames().Contains(Marker)) {
                MarkerActor = PlaceableMarker;
                break;
            }
        }

        if (MarkerActor) {
            PlaceableMarkers.Remove(MarkerActor);
            TArray<AActor*> SpawnedActors = SpawnItemWithThemeEngine(ItemInfo, MarkerActor);
            AllSpawnedActors.Append(SpawnedActors);
            
            // If it contains a item metadata component, then assign the item
            for (const TWeakObjectPtr<AActor> SpawnedActor : SpawnedActors) {
                if (UDungeonFlowItemMetadataComponent* ItemComponent = SpawnedActor->FindComponentByClass<UDungeonFlowItemMetadataComponent>()) {
                    ItemComponent->SetFlowItem(ItemInfo);
                }
            }
        } else {
            UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Cannot spawn marker \"%s\" in module chunk: %s"), *Marker, *ChunkName);
        }
    }

    return AllSpawnedActors;
}

void USnapGridFlowBuilder::HandleChunkLoadedAndVisible(USnapStreamingChunk* InChunk) {
    if (!InChunk || !InChunk->GetLoadedLevel()) return;
    if (!SnapGridModel.IsValid() || !SnapGridModel->AbstractGraph) return;

    const TArray<AActor*>& ChunkActors = InChunk->GetLoadedLevel()->Actors;
    const FGuid AbstractNodeId = InChunk->ID;
    const FBox ChunkBounds = InChunk->Bounds;
    const FString ChunkName = InChunk->GetLoadedLevel()->GetFullName();

    TArray<AActor*> SpawnedActors = SpawnChunkPlaceableMarkers(ChunkActors, AbstractNodeId, ChunkBounds, ChunkName);
    
    // Register the spawned actors so they can be cleaned up when the chunk is destroyed
    if (InChunk) {
        for (TWeakObjectPtr<AActor> SpawnedActor : SpawnedActors) {
            if (SpawnedActor.IsValid()) {
                InChunk->RegisterManagedActor(SpawnedActor.Get());
            }
        }
    }
}

bool USnapGridFlowBuilder::IdentifyBuildSucceeded() const {
    return SnapGridModel.IsValid() && SnapGridModel->AbstractGraph && SnapGridModel->ModuleInstances.Num() > 0; 
}

void USnapGridFlowBuilder::BuildPersistentSnapLevel(UWorld* InWorld, const TArray<SnapLib::FModuleNodePtr>& InModuleNodes,
        TSharedPtr<FDungeonSceneProvider> InSceneProvider)
{
    if (!Dungeon) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Invalid dungeon state"));
        return;
    }
    
    if (!InWorld) {
        UE_LOG(SnapGridDungeonBuilderLog, Error, TEXT("Invalid world state"));
        return;
    }
    
    bool bDrawModuleBounds = Dungeon && Dungeon->bDrawDebugData;
    TMap<FGuid, TArray<ASnapConnectionActor*>> ModuleConnections;
    TMap<AActor*, AActor*> TemplateToSpawnedActorMap;
    
    const FName DungeonTag = UDungeonModelHelper::GetDungeonIdTag(Dungeon);
    for (const SnapLib::FModuleNodePtr& Node : InModuleNodes) {
        TArray<AActor*> ChunkActors;
        
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

            if (ASnapGridFlowModuleBoundsActor* BoundsActor = Cast<ASnapGridFlowModuleBoundsActor>(SpawnedModuleActor)) {
                if (BoundsActor->BoundsComponent) {
                    BoundsActor->BoundsComponent->bRenderBounds = bDrawModuleBounds;
                    BoundsActor->BoundsComponent->MarkRenderStateDirty();
                }
            }
        }

        // Spawn the placeable actors
        const FBox ChunkBounds = Node->GetModuleBounds();
        const FString ChunkName = ModuleLevel->GetFullName();
        TArray<AActor*> SpawnedItemActors = SpawnChunkPlaceableMarkers(ChunkActors, AbstractNodeId, ChunkBounds, ChunkName);
        for (AActor* SpawnedItemActor : SpawnedItemActors) {
            if (SpawnedItemActor) {
                SpawnedItemActor->Tags.Add(DungeonTag);
                SpawnedItemActor->Tags.Add(FSceneProviderCommand::TagComplexActor);
            }
        }
    }

    // Fix the module connections
    ULevel* PersistentLevel = InWorld->PersistentLevel;
    FSnapGridFlowStreamingChunkHandler ChunkHandler(InWorld, Dungeon, SnapGridModel.Get(), nullptr);    // We'll use this to setup our doors
    TArray<FSnapConnectionInstance> ConnectionInstances = SnapGridModel->Connections;
    for (const auto& Entry : ModuleConnections) {
        FGuid ChunkID = Entry.Key;
        const TArray<ASnapConnectionActor*>& ConnectionActors = Entry.Value;
        ChunkHandler.Internal_SpawnChunkConnections(ChunkID, ConnectionInstances, ConnectionActors, PersistentLevel, PersistentLevel);
    }
}

void USnapGridFlowBuilder::DrawDebugData(UWorld* InWorld, bool bPersistent, float LifeTime) {
    /*
    if (!SnapGridModel.IsValid() || !SnapGridConfig.IsValid()) {
        return;
    }

    if (!SnapGridModel->AbstractGraph) {
        return;
    }
    
    if (!SnapGridConfig->ModuleDatabase || !SnapGridConfig->ModuleDatabase->ModuleBoundsAsset) {
        return;
    }

    FVector BaseOffset = Dungeon ? Dungeon->GetActorLocation() : FVector::ZeroVector;
    const FColor BoxColor = FColor(128, 0, 0);
    const FVector ModuleSize = SnapGridConfig->ModuleDatabase->ModuleBoundsAsset->ChunkSize;
    const FIntVector LayoutGridSize = SnapGridModel->AbstractGraph->GridSize;
    for (int z = 0; z < LayoutGridSize.Z; z++) {
        for (int y = 0; y < LayoutGridSize.Y; y++) {
            for (int x = 0; x < LayoutGridSize.X; x++) {
                const FVector BoxMin = BaseOffset + FVector(x, y, z) * ModuleSize;
                const FVector BoxExtent = ModuleSize * 0.5f;
                const FVector BoxCenter = BoxMin + BoxExtent;
                DrawDebugBox(InWorld, BoxCenter, BoxExtent, BoxColor, bPersistent, LifeTime);
            }
        }
    }
    */
}

bool USnapGridFlowBuilder::SupportsProperty(const FName& PropertyName) const {
    if (PropertyName == "Themes") return false;
    if (PropertyName == "MarkerEmitters") return false;
    //if (PropertyName == "EventListeners") return false;

    return  true;
}

bool USnapGridFlowBuilder::CanBuildDungeon(FString& OutMessage) {
    ADungeon* OuterDungeon = Cast<ADungeon>(GetOuter());
    if (OuterDungeon) {
        SnapGridConfig = Cast<USnapGridFlowConfig>(OuterDungeon->GetConfig());

        if (!SnapGridConfig.IsValid()) {
            OutMessage = "Dungeon not initialized correctly";
            return false;
        }

        USnapGridFlowAsset* FlowAsset = SnapGridConfig->FlowGraph.LoadSynchronous();
        if (!FlowAsset) {
            OutMessage = "Dungeon Flow asset not assigned";
            return false;
        }

        UDungeonThemeAsset* ItemThemeAsset = SnapGridConfig->ItemTheme.LoadSynchronous();
        if (!ItemThemeAsset) {
            OutMessage = "Item Theme asset not assigned";
            return false;
        }

        USnapGridFlowModuleDatabase* ModuleDatabase = SnapGridConfig->ModuleDatabase.LoadSynchronous();
        if (!ModuleDatabase) {
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

bool USnapGridFlowBuilder::PerformSelectionLogic(const TArray<UDungeonSelectorLogic*>& SelectionLogics, const FDAMarkerInfo& Socket) {
    for (UDungeonSelectorLogic* SelectionLogic : SelectionLogics) {
        USnapGridFlowSelectorLogic* GridFlowSelectionLogic = Cast<USnapGridFlowSelectorLogic>(SelectionLogic);
        if (!GridFlowSelectionLogic) {
            UE_LOG(SnapGridDungeonBuilderLog, Warning, TEXT("Invalid selection logic specified.  SnapGridFlowSelectorLogic expected"));
            return false;
        }

        // Perform blueprint based selection logic
        const bool bSelected = GridFlowSelectionLogic->SelectNode(SnapGridModel.Get(), SnapGridConfig.Get(), this, SnapGridQuery.Get(), Random, Socket.Transform);
        if (!bSelected) {
            return false;
        }
    }
    return true;
}

FTransform USnapGridFlowBuilder::PerformTransformLogic(const TArray<UDungeonTransformLogic*>& DungeonTransformLogics, const FDAMarkerInfo& Socket) {
    FTransform result = FTransform::Identity;

    for (UDungeonTransformLogic* TransformLogic : DungeonTransformLogics) {
        USnapGridFlowTransformLogic* GridFlowTransformLogic = Cast<USnapGridFlowTransformLogic>(TransformLogic);
        if (!GridFlowTransformLogic) {
            UE_LOG(SnapGridDungeonBuilderLog, Warning, TEXT("Invalid transform logic specified.  SnapGridFlowTransformLogic expected"));
            continue;
        }

        FTransform LogicOffset;
        if (TransformLogic) {
            GridFlowTransformLogic->GetNodeOffset(SnapGridModel.Get(), SnapGridConfig.Get(), SnapGridQuery.Get(), Random,  LogicOffset);
        }
        else {
            LogicOffset = FTransform::Identity;
        }

        FTransform out;
        FTransform::Multiply(&out, &LogicOffset, &result);
        result = out;
    }
    return result;
}

TSubclassOf<UDungeonModel> USnapGridFlowBuilder::GetModelClass() {
    return USnapGridFlowModel::StaticClass();
}

TSubclassOf<UDungeonConfig> USnapGridFlowBuilder::GetConfigClass() {
    return USnapGridFlowConfig::StaticClass();
}

TSubclassOf<UDungeonToolData> USnapGridFlowBuilder::GetToolDataClass() {
    return USnapGridFlowToolData::StaticClass();
}

TSubclassOf<UDungeonQuery> USnapGridFlowBuilder::GetQueryClass() {
    return USnapGridFlowQuery::StaticClass();
}

///////////////////////////// USnapGridFlowModel /////////////////////////////
void USnapGridFlowModel::Reset() {
    Connections.Reset();
    ModuleInstances.Reset();
    AbstractGraph = nullptr;
}

///////////////////////////// FSnapGridFlowStreamingChunkHandler /////////////////////////////
FSnapGridFlowStreamingChunkHandler::FSnapGridFlowStreamingChunkHandler(UWorld* InWorld, TWeakObjectPtr<ADungeon> InDungeon,
        USnapGridFlowModel* InSnapGridModel, UDungeonLevelStreamingModel* InLevelStreamingModel)
    : FSnapStreamingChunkHandlerBase(InWorld, InDungeon, InLevelStreamingModel)
    , SnapGridModel(InSnapGridModel)
{
}

TArray<FSnapConnectionInstance>* FSnapGridFlowStreamingChunkHandler::GetConnections() const {
    return SnapGridModel.IsValid() ? &SnapGridModel->Connections : nullptr;
}

void FSnapGridFlowStreamingChunkHandler::OnChunkVisible(USnapStreamingChunk* Chunk) {
    FSnapStreamingChunkHandlerBase::OnChunkVisible(Chunk);

    if (Chunk) {
        if (!FullyLoadedChunks.Contains(Chunk->ID)) {
            ChunkFullyLoadedEvent.ExecuteIfBound(Chunk);
        }

        FullyLoadedChunks.Add(Chunk->ID);
    }
}

void FSnapGridFlowStreamingChunkHandler::OnChunkHidden(USnapStreamingChunk* Chunk)
{
    FSnapStreamingChunkHandlerBase::OnChunkHidden(Chunk);

   
}

void FSnapGridFlowStreamingChunkHandler::OnChunkLoaded(USnapStreamingChunk* Chunk) {
    FSnapStreamingChunkHandlerBase::OnChunkLoaded(Chunk);

    ChunkLoadedEvent.ExecuteIfBound(Chunk);
}

void FSnapGridFlowStreamingChunkHandler::OnChunkUnloaded(USnapStreamingChunk* Chunk) {
    FSnapStreamingChunkHandlerBase::OnChunkUnloaded(Chunk);

    if (Chunk) {
        FullyLoadedChunks.Remove(Chunk->ID);
    }
    
    ChunkUnloadedEvent.ExecuteIfBound(Chunk);
}

void FSnapGridFlowStreamingChunkHandler::OnConnectionDoorCreated(FSnapConnectionInstance* ConnectionData) const {
    if (!ConnectionData || !LevelStreamingModel.IsValid() || !SnapGridModel.IsValid() || !SnapGridModel->AbstractGraph) {
        return;
    }
    if (!ConnectionData->bHasSpawnedDoorActor) {
        return;
    }

    UFlowAbstractLink* Link = SnapGridModel->AbstractGraph->GetLink(ConnectionData->ModuleA, ConnectionData->ModuleB, true);
    if (Link && Link->LinkItems.Num() > 0) {
        for (TWeakObjectPtr<AActor> PersistentDoorActor : ConnectionData->SpawnedDoorActors) {
            if (PersistentDoorActor.IsValid()) {
                if (UDungeonFlowItemMetadataComponent* ItemComponent = PersistentDoorActor->FindComponentByClass<UDungeonFlowItemMetadataComponent>()) {
                    ItemComponent->SetFlowItem(Link->LinkItems[0]);
                }
            }
        }
    }
}

void FSnapGridFlowStreamingChunkHandler::UpdateConnectionDoorType(const FSnapConnectionInstance* ConnectionData, USnapConnectionComponent* ConnectionComponent) const {
    if (!ConnectionData) {
        ConnectionComponent->ConnectionState = ESnapConnectionState::Unknown;
        return;
    }

    UFlowAbstractLink* Link = SnapGridModel->AbstractGraph->GetLink(ConnectionData->ModuleA, ConnectionData->ModuleB, true);
    if (!Link) {
        ConnectionComponent->ConnectionState = ESnapConnectionState::Wall;
        return;
    }

    ConnectionComponent->SpawnOffset = FTransform::Identity;
    ConnectionComponent->ConnectionState = ESnapConnectionState::Door;
    ConnectionComponent->DoorType = ESnapConnectionDoorType::NormalDoor;
    
    if (Link->Type == EFlowAbstractLinkType::OneWay) {
        // Check if this is a vertical link
        const FGuid SourceNodeId = Link->SourceSubNode.IsValid() ? Link->SourceSubNode : Link->Source;
        const FGuid DestNodeId = Link->DestinationSubNode.IsValid() ? Link->DestinationSubNode : Link->Destination;

        USnapGridFlowAbstractGraph* Graph = SnapGridModel->AbstractGraph;
        UFlowAbstractNode* SourceNode = Link->SourceSubNode.IsValid() ? Graph->FindSubNode(Link->SourceSubNode) : Graph->GetNode(Link->Source);
        UFlowAbstractNode* DestNode = Link->DestinationSubNode.IsValid() ? Graph->FindSubNode(Link->DestinationSubNode) : Graph->GetNode(Link->Destination);
        check(SourceNode && DestNode);
        
        const float SourceZ = SourceNode->Coord.Z;
        const float DestZ = DestNode->Coord.Z;

        
        if (FMath::IsNearlyEqual(SourceZ, DestZ, 1e-4f)) {
            ConnectionComponent->DoorType = ESnapConnectionDoorType::OneWayDoor;

            // Handle orientation towards the correct one-way direction
            const FVector Forward = ConnectionComponent->GetComponentTransform().GetRotation().GetForwardVector();
            FVector LinkDir = DestNode->Coord - SourceNode->Coord;
            LinkDir.Normalize();

            const float Dot = FVector::DotProduct(Forward, LinkDir);
            // Dot value will be -1 if the vectors point in the opposite direction: https://chortle.ccsu.edu/VectorLessons/vch09/vch09_6.html
            // If this is the case, rotate the spawned one-way door by 180, to fix the direction
            if (Dot < 0) {
                // Points in the opposite direction
                ConnectionComponent->SpawnOffset = FTransform(FQuat(FVector::UpVector, PI));
            }
        }
        else if (SourceZ < DestZ) {
            ConnectionComponent->DoorType = ESnapConnectionDoorType::OneWayDoorUp;
        }
        else if (SourceZ > DestZ) {
            ConnectionComponent->DoorType = ESnapConnectionDoorType::OneWayDoorDown;
        }
    }
    else if (Link->LinkItems.Num() > 0) {
        UFlowGraphItem* Item = Link->LinkItems[0];
        if (Item && Item->ItemType == EFlowGraphItemType::Lock) {
            ConnectionComponent->DoorType = ESnapConnectionDoorType::LockedDoor;
            ConnectionComponent->MarkerName = Item->MarkerName;
        }
    }
}


///////////////////////////// FSnapGridFlowStreamingChunkHandler /////////////////////////////
void FSnapGridFlowProcessDomainExtender::ExtendDomains(FFlowProcessor& InProcessor) {
    // Register the Abstract Layout Graph Domain
    const TSharedPtr<FSnapGridFlowAbstractGraphDomain> SGFLayoutGraphDomain = MakeShareable(new FSnapGridFlowAbstractGraphDomain);
    SGFLayoutGraphDomain->SetModuleDatabase(ModuleDatabase);
    SGFLayoutGraphDomain->SetSupportsDoorCategory(bSupportsDoorCategory);
    InProcessor.RegisterDomain(SGFLayoutGraphDomain);
}


///////////////////////////// USnapGridFlowSelectorLogic /////////////////////////////
bool USnapGridFlowSelectorLogic::SelectNode_Implementation(USnapGridFlowModel* Model, USnapGridFlowConfig* Config, USnapGridFlowBuilder* Builder,
            USnapGridFlowQuery* Query, const FRandomStream& RandomStream, const FTransform& MarkerTransform) {
    return false;
}

///////////////////////////// USnapGridFlowTransformLogic /////////////////////////////
void USnapGridFlowTransformLogic::GetNodeOffset_Implementation(USnapGridFlowModel* Model, USnapGridFlowConfig* Config, USnapGridFlowQuery* Query,
            const FRandomStream& RandomStream, FTransform& Offset) {
    
}

