//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/Widgets/SThemePreviewViewport.h"

#include "Builders/Grid/GridDungeonBuilder.h"
#include "Builders/GridFlow/GridFlowConfig.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/ThemeGraphAppMode.h"
#include "Core/Editors/ThemeEditor/DungeonArchitectThemeEditor.h"
#include "Core/Editors/ThemeEditor/Visualization/ThemeEditorMarkerVisualizer.h"
#include "Core/Editors/ThemeEditor/Widgets/DungeonEditorViewportClient.h"
#include "Core/Editors/ThemeEditor/Widgets/SDungeonEditorViewportToolbar.h"
#include "Core/Utils/DungeonModelHelper.h"
#include "Frameworks/ThemeEngine/DungeonThemeEngine.h"
#include "Frameworks/ThemeEngine/SceneProviders/DungeonSceneProvider.h"

#include "AdvancedPreviewScene.h"
#include "Components/DirectionalLightComponent.h"
#include "Components/LightComponent.h"
#include "Components/LightComponentBase.h"
#include "Components/SkyAtmosphereComponent.h"
#include "Components/SkyLightComponent.h"
#include "EditorViewportClient.h"
#include "EngineUtils.h"
#include "PreviewScene.h"
#include "Widgets/Docking/SDockTab.h"

void SThemePreviewViewport::Construct(const FArguments& InArgs) {
    bRequestRebuildPreviewMesh = false;
    ThemeEditorPtr = InArgs._DungeonEditor;
    ObjectToEdit = InArgs._ObjectToEdit;
    bDrawDebugData = false;
    DungeonBuilder = nullptr;

    {
        FAdvancedPreviewScene::ConstructionValues CVS;
        CVS.bCreatePhysicsScene = false;
        CVS.LightBrightness = 3;
        CVS.SkyBrightness = 1;
        PreviewScene = MakeShareable(new FAdvancedPreviewScene(CVS));
        PreviewScene->SetFloorVisibility(false);

        // Make sure the floor is not visible even if enabled from the properties tab
        PreviewScene->SetFloorOffset(-100000);
    }
    SEditorViewport::Construct(SEditorViewport::FArguments());
    ObjectToEdit->PreviewViewportProperties->PropertyChangeListener = SharedThis(this);
    CreateDungeonBuilder(ObjectToEdit->PreviewViewportProperties->BuilderClass);

    UWorld* World = PreviewScene->GetWorld();
    DungeonBuilder->BuildDungeon(DungeonModel, ObjectToEdit->PreviewViewportProperties->DungeonConfig, DungeonQuery, World);

    Skylight = NewObject<USkyLightComponent>();
    //PreviewScene->AddComponent(Skylight, FTransform::Identity);

    AtmosphericFog = NewObject<USkyAtmosphereComponent>();
    PreviewScene->AddComponent(AtmosphericFog, FTransform::Identity);

    PreviewScene->DirectionalLight->SetMobility(EComponentMobility::Movable);
    PreviewScene->DirectionalLight->CastShadows = true;
    PreviewScene->DirectionalLight->CastStaticShadows = true;
    PreviewScene->DirectionalLight->CastDynamicShadows = true;
    PreviewScene->DirectionalLight->SetIntensity(3);
    PreviewScene->SetSkyBrightness(1.0f);

    UDungeonConfig* DungeonConfig = ObjectToEdit->PreviewViewportProperties->DungeonConfig;
    if (DungeonConfig) {
        ListenToConfigChanges(DungeonConfig);
    }

    // Add the visualizer actor
    Visualizer = World->SpawnActor<AThemeEditorMarkerVisualizer>();
}


SThemePreviewViewport::~SThemePreviewViewport() {
    FCoreUObjectDelegates::OnObjectPropertyChanged.RemoveAll(this);
    if (EditorViewportClient.IsValid()) {
        EditorViewportClient->Viewport = nullptr;
    }

    if (DungeonBuilder) {
        DungeonBuilder->RemoveFromRoot();
        DungeonBuilder = nullptr;
    }
    if (DungeonModel) {
        DungeonModel->RemoveFromRoot();
        DungeonModel = nullptr;
    }
    if (DungeonQuery) {
        DungeonQuery->RemoveFromRoot();
        DungeonQuery = nullptr;
    }
}

void SThemePreviewViewport::AddReferencedObjects(FReferenceCollector& Collector) {
    Collector.AddReferencedObject(ObjectToEdit);
    Collector.AddReferencedObject(DungeonBuilder);
    Collector.AddReferencedObject(DungeonModel);
    Collector.AddReferencedObject(Visualizer);
}

void SThemePreviewViewport::SetParentTab(const FName& InAppMode, TSharedRef<SDockTab> InParentTab) {
    if (!ParentTabs.Contains(InAppMode)) {
        ParentTabs.Add(InAppMode, nullptr);
    }
    
    TWeakPtr<SDockTab>& ParentTabRef = ParentTabs.FindOrAdd(InAppMode);
    ParentTabRef = InParentTab;
}

void SThemePreviewViewport::RebuildMeshes() {
    bRequestRebuildPreviewMesh = true;
}

void SThemePreviewViewport::PerformMeshRebuild() {
    if (!DungeonBuilder || !DungeonBuilder->SupportsTheming()) {
        return;
    }

    if (bForcePreviewRebuildLayout) {
        DungeonBuilder->BuildDungeon(DungeonModel, ObjectToEdit->PreviewViewportProperties->DungeonConfig, DungeonQuery, GetWorld());
    }
    else {
        // Since we do not rebuild the layout again for optimization in the theme editor, perform cleanup before reapplying the theme
        if (DungeonQuery && DungeonQuery->UserState) {
            DungeonQuery->UserState->ClearAllState();
        }
    }

    DungeonBuilder->EmitDungeonMarkers();

    // Emit custom markers in user-created blueprints
    //TArray<UDungeonMarkerEmitter*> Emitters = UDungeonModelHelper::CreateInstances<UDungeonMarkerEmitter>(
    //		ObjectToEdit->PreviewViewportProperties->MarkerEmitters);
    ensure(ObjectToEdit);
    const TArray<UDungeonMarkerEmitter*> Emitters = ObjectToEdit->PreviewViewportProperties->MarkerEmitters;
    DungeonBuilder->EmitCustomMarkers(Emitters);

    UDungeonConfig* Config = ObjectToEdit->PreviewViewportProperties->DungeonConfig;
    SceneProvider = DungeonBuilder->CreateSceneProvider(Config, nullptr, PreviewScene->GetWorld());

    // While building the scene over multiple frames, start building the objects closer to the camera first
    const FVector BuildPriorityLocation = EditorViewportClient->GetViewLocation();
    SceneProvider->SetBuildPriorityLocation(BuildPriorityLocation);

    CleanupModifiedNodeObjects();
    TArray<UDungeonThemeAsset*> Themes;
    Themes.Add(ObjectToEdit);

    const TArray<FClusterThemeInfo> EmptyClusteredThemes; // No clustered themes shown in the theme editor viewport
    DungeonBuilder->ApplyDungeonTheme(Themes, EmptyClusteredThemes, SceneProvider, PreviewScene->GetWorld());
}


void SThemePreviewViewport::OnShowPropertyDungeon() const {
    ShowObjectProperties(ObjectToEdit->PreviewViewportProperties);
}

void SThemePreviewViewport::OnShowPropertySkylight() const {
    ShowObjectProperties(Skylight);
}

void SThemePreviewViewport::OnShowPropertyDirectionalLight() const {
    ShowObjectProperties(PreviewScene->DirectionalLight);
}

void SThemePreviewViewport::OnShowPropertyAtmosphericFog() const {
    ShowObjectProperties(AtmosphericFog);
}

void SThemePreviewViewport::OnToggleDebugData() {
    bDrawDebugData = !bDrawDebugData;
}

void SThemePreviewViewport::HandleForceRebuildPreviewLayout() {
    bForcePreviewRebuildLayout = !bForcePreviewRebuildLayout;
}

bool SThemePreviewViewport::HandleForceRebuildPreviewLayoutIsChecked() const {
    return bForcePreviewRebuildLayout;
}

bool SThemePreviewViewport::HandleToggleDebugDataIsChecked() const {
    return bDrawDebugData;
}

void SThemePreviewViewport::ShowObjectProperties(UObject* Object, bool bForceRefresh) const {
    const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
    if (ThemeEditor.IsValid()) {
        const TSharedPtr<FThemeEditorAppModeBase> AppMode = StaticCastSharedPtr<FThemeEditorAppModeBase>(ThemeEditor->GetCurrentModePtr());
        if (AppMode.IsValid()) {
            AppMode->ShowObjectDetails(Object, bForceRefresh);
        }
    }
}

void SThemePreviewViewport::ListenToConfigChanges(UDungeonConfig* Config) const {
    Config->ConfigPropertyChanged.Unbind();
    Config->ConfigPropertyChanged.BindUObject(ObjectToEdit->PreviewViewportProperties, &UDungeonEditorViewportProperties::PostEditChangeConfigProperty);
}

void SThemePreviewViewport::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime,
                                  const float InDeltaTime) {
    SEditorViewport::Tick(AllottedGeometry, InCurrentTime, InDeltaTime);
    if (bRequestRebuildPreviewMesh) {
        bRequestRebuildPreviewMesh = false;
        PerformMeshRebuild();
    }

    if (bDrawDebugData) {
        DungeonBuilder->DrawDebugData(GetWorld());
    }

    if (SceneProvider.IsValid()) {
        SceneProvider->RunGameThreadCommands(30);
    }
}

EVisibility SThemePreviewViewport::GetToolbarVisibility() const {
    return EVisibility::Visible;
}

void SThemePreviewViewport::VisualizeMarkers(const TSet<FName>& InMarkerNames) const {
    UDungeonConfig* DungeonConfig = ObjectToEdit->PreviewViewportProperties->DungeonConfig;
    if (Visualizer && DungeonBuilder && DungeonConfig) {
        Visualizer->BuildVisualization(DungeonConfig, DungeonModel, DungeonBuilder, InMarkerNames);
    }
}

void SThemePreviewViewport::ClearMarkerVisualizations() const {
    if (Visualizer) {
        Visualizer->ClearVisualization();
    }
}

void SThemePreviewViewport::ZoomOnNearestNodeMesh(const FName& InNodeId) const {
    if (GetAdvancedPreview().IsValid() && GetViewportClient().IsValid()) {
        // Zoom in on the nearest spawned actor in the viewport
        const FName NodeTag = FDungeonThemeEngineUtils::CreateNodeTagFromId(InNodeId);
        const FVector ViewLocation = GetViewportClient()->GetViewLocation();
        UWorld* World = GetAdvancedPreview()->GetWorld();

        AActor* BestActor = nullptr;
        float BestDist = MAX_flt;
        if (World) {
            for (TActorIterator<AActor> It(World); It; ++It) {
                AActor* Actor = *It;
                if (Actor->Tags.Contains(NodeTag)) {
                    const float DistanceSq = (Actor->GetActorLocation() - ViewLocation).SizeSquared();
                    if (!BestActor || DistanceSq < BestDist) {
                        BestDist = DistanceSq;
                        BestActor = Actor;
                    }
                }
            }
        }

        if (BestActor) {
            // Zoom in on the actor
            FVector BoundsOrigin, BoundsExtent;
            BestActor->GetActorBounds(false, BoundsOrigin, BoundsExtent);
            BoundsExtent += FVector(100, 100, 100);
            const FBox Bounds(BoundsOrigin - BoundsExtent, BoundsOrigin + BoundsExtent);
            GetViewportClient()->FocusViewportOnBox(Bounds);
        }
    }
}

TSharedRef<FEditorViewportClient> SThemePreviewViewport::MakeEditorViewportClient() {
    EditorViewportClient = MakeShareable(
        new FDungeonEditorViewportClient(SharedThis(this), *PreviewScene, ObjectToEdit));

    EditorViewportClient->bSetListenerPosition = false;
    EditorViewportClient->SetRealtime(true); // TODO: Check if real-time is needed
    EditorViewportClient->VisibilityDelegate.BindSP(this, &SThemePreviewViewport::IsVisible);

    return EditorViewportClient.ToSharedRef();
}

EVisibility SThemePreviewViewport::OnGetViewportContentVisibility() const {
    return EVisibility::Visible;
}

void SThemePreviewViewport::BindCommands() {
    SEditorViewport::BindCommands();

    const FDungeonEditorViewportCommands& ViewportActions = FDungeonEditorViewportCommands::Get();
    CommandList->MapAction(
        ViewportActions.ShowPropertyDungeon,
        FExecuteAction::CreateSP(this, &SThemePreviewViewport::OnShowPropertyDungeon));

    CommandList->MapAction(
        ViewportActions.ShowPropertySkylight,
        FExecuteAction::CreateSP(this, &SThemePreviewViewport::OnShowPropertySkylight));

    CommandList->MapAction(
        ViewportActions.ShowPropertyDirectionalLight,
        FExecuteAction::CreateSP(this, &SThemePreviewViewport::OnShowPropertyDirectionalLight));

    CommandList->MapAction(
        ViewportActions.ShowPropertyAtmosphericFog,
        FExecuteAction::CreateSP(this, &SThemePreviewViewport::OnShowPropertyAtmosphericFog));

    CommandList->MapAction(
        ViewportActions.ToggleDebugData,
        FExecuteAction::CreateSP(this, &SThemePreviewViewport::OnToggleDebugData));

    CommandList->MapAction(
        ViewportActions.ForceRebuildPreviewLayout,
        FExecuteAction::CreateSP(this, &SThemePreviewViewport::HandleForceRebuildPreviewLayout),
        FCanExecuteAction(),
        FIsActionChecked::CreateSP(this, &SThemePreviewViewport::HandleForceRebuildPreviewLayoutIsChecked));

    CommandList->MapAction(
        ViewportActions.DrawDebugData,
        FExecuteAction::CreateSP(this, &SThemePreviewViewport::OnToggleDebugData),
        FCanExecuteAction(),
        FIsActionChecked::CreateSP(this, &SThemePreviewViewport::HandleToggleDebugDataIsChecked));
}

void SThemePreviewViewport::OnFocusViewportToSelection() {
    SEditorViewport::OnFocusViewportToSelection();
}

TSharedPtr<SWidget> SThemePreviewViewport::MakeViewportToolbar() {
    // Build our toolbar level toolbar
    TSharedRef<SDungeonEditorViewportToolBar> ToolBar =
        SNew(SDungeonEditorViewportToolBar)
		.Viewport(SharedThis(this))
		.Visibility(this, &SThemePreviewViewport::GetToolbarVisibility)
		.IsEnabled(FSlateApplication::Get().GetNormalExecutionAttribute());

    return
        SNew(SVerticalBox)
        .Visibility(EVisibility::SelfHitTestInvisible)
        + SVerticalBox::Slot()
          .AutoHeight()
          .VAlign(VAlign_Top)
        [
            ToolBar
        ];
}

bool SThemePreviewViewport::IsVisible() const {
    if (ViewportWidget.IsValid()) {
        for (const auto& Entry : ParentTabs) {
            TWeakPtr<SDockTab> ParentTabPtr = Entry.Value;
            TSharedPtr<SDockTab> ParentTab = ParentTabPtr.Pin();
            if (ParentTab.IsValid()) {
                if (ParentTab->IsForeground()) {
                    return true;
                }
            }
        }
    }
    return false;
}

UDungeonModel* SThemePreviewViewport::CreateModelInstance(UObject* Outer) const {
    if (!DungeonBuilder) return nullptr;
    UClass* ModelClass = DungeonBuilder->GetModelClass();
    if (!ModelClass) return nullptr;

    return NewObject<UDungeonModel>(Outer, ModelClass);
}

UDungeonConfig* SThemePreviewViewport::CreateConfigInstance(UObject* Outer) const {
    if (!DungeonBuilder) return nullptr;
    UClass* ConfigClass = DungeonBuilder->GetConfigClass();
    if (!ConfigClass) return nullptr;

    UDungeonConfig* Config = NewObject<UDungeonConfig>(Outer, ConfigClass);

    // Builder specific config defaults
    if (UGridFlowConfig* GridFlowConfig = Cast<UGridFlowConfig>(Config)) {
        const UGridFlowAsset* DefaultGridFlowGraph = Cast<UGridFlowAsset>(StaticLoadObject(
                UGridFlowAsset::StaticClass(), nullptr, TEXT("/DungeonArchitect/Core/Builders/GridFlowContent/FlowGraph/DefaultGridFlow")));
        GridFlowConfig->GridFlow = DefaultGridFlowGraph;
    }
    
    return Config;
}

UDungeonQuery* SThemePreviewViewport::CreateQueryInstance(UObject* Outer) const {
    if (!DungeonBuilder) return nullptr;
    UClass* QueryClass = DungeonBuilder->GetQueryClass();
    if (!QueryClass) return nullptr;

    return NewObject<UDungeonQuery>(Outer, QueryClass);
}

void SThemePreviewViewport::CreateDungeonBuilder(TSubclassOf<UDungeonBuilder> BuilderClass) {
    if (BuilderClass == nullptr) {
        BuilderClass = UGridDungeonBuilder::StaticClass();
    }

    if (DungeonBuilder) {
        DungeonBuilder->RemoveFromRoot();
    }

    DungeonBuilder = NewObject<UDungeonBuilder>(static_cast<UObject*>(GetTransientPackage()), BuilderClass);
    DungeonBuilder->AddToRoot();

    if (DungeonModel) {
        DungeonModel->RemoveFromRoot();
    }
    DungeonModel = CreateModelInstance(static_cast<UObject*>(GetTransientPackage()));
    DungeonModel->AddToRoot();

    UDungeonConfig* OldConfig = ObjectToEdit->PreviewViewportProperties->DungeonConfig;
    if (!OldConfig || !OldConfig->IsValidLowLevel() || OldConfig->GetClass() != DungeonBuilder->GetConfigClass()) {
        UDungeonConfig* DungeonConfig = CreateConfigInstance(ObjectToEdit);
        ObjectToEdit->PreviewViewportProperties->DungeonConfig = DungeonConfig;
        ListenToConfigChanges(DungeonConfig);
    }

    if (DungeonQuery) {
        DungeonQuery->RemoveFromRoot();
    }
    DungeonQuery = CreateQueryInstance(static_cast<UObject*>(GetTransientPackage()));
    DungeonQuery->Initialize(ObjectToEdit->PreviewViewportProperties->DungeonConfig, DungeonModel,
                             FTransform::Identity);
    DungeonQuery->AddToRoot();

    DestroyDungeonActors();
}

void SThemePreviewViewport::DestroyDungeonActors() {
    const FName DungeonTag = UDungeonModelHelper::GetDungeonIdTag(nullptr);
    // Destroy all actors that have a "Dungeon" tag
    for (TActorIterator<AActor> ActorItr(PreviewScene->GetWorld()); ActorItr; ++ActorItr) {
        if (ActorItr->ActorHasTag(DungeonTag) || ActorItr->ActorHasTag(UDungeonModelHelper::GenericDungeonIdTag)) {
            ActorItr->Destroy();
        }
    }
}

void SThemePreviewViewport::CleanupModifiedNodeObjects() {
    const FName DungeonTag = UDungeonModelHelper::GetDungeonIdTag(nullptr);
    for (const FName& NodeId : NodeObjectsToRebuild) {
        const FName NodeTag = FDungeonThemeEngineUtils::CreateNodeTagFromId(NodeId);
        // Destroy all actors that have a "Dungeon" tag
        for (TActorIterator<AActor> ActorItr(PreviewScene->GetWorld()); ActorItr; ++ActorItr) {
            if (ActorItr->ActorHasTag(DungeonTag) && ActorItr->ActorHasTag(NodeTag)) {
                ActorItr->Destroy();
            }
        }
    }
    NodeObjectsToRebuild.Reset();
}

void SThemePreviewViewport::OnPropertyChanged(FString PropertyName, UDungeonEditorViewportProperties* Properties) {
    if (PropertyName == "BuilderClass") {
        CreateDungeonBuilder(Properties->BuilderClass);
        check(ObjectToEdit->PreviewViewportProperties->DungeonConfig);
        ShowObjectProperties(ObjectToEdit->PreviewViewportProperties, true);

        // Recreate the default builder marker nodes
        if (ThemeEditorPtr.IsValid()) {
            ThemeEditorPtr.Pin()->NotifyBuilderClassChanged(Properties->BuilderClass);
        }
    }
    else if (PropertyName == "Instanced") {
        DestroyDungeonActors();
    }

    DungeonModel = CreateModelInstance(ObjectToEdit);
    DungeonBuilder->BuildDungeon(DungeonModel, Properties->DungeonConfig, DungeonQuery, PreviewScene->GetWorld());
    bRequestRebuildPreviewMesh = true;
}

void SThemePreviewViewport::SetNodesToRebuild(const TSet<FName>& NodeIds) {
    NodeObjectsToRebuild.Append(NodeIds);
}

