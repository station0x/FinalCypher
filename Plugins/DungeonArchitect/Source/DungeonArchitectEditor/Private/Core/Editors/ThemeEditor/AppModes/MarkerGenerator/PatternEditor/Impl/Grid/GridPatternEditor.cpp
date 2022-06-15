//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Impl/Grid/GridPatternEditor.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppModeCommands.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Impl/Grid/GridPatternEdActors.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Impl/Grid/GridPatternEditorSettings.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Viewport/PatternEditorViewport.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/Viewport/PatternEditorViewportClient.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraph.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphCompiler.h"
#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"

#include "EdGraphUtilities.h"
#include "Engine/StaticMeshActor.h"
#include "EngineUtils.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "MouseDeltaTracker.h"

#define LOCTEXT_NAMESPACE "MGGridPatternEditor"
DEFINE_LOG_CATEGORY_STATIC(LogGridPatternEd, Log, All);

FMGGridPatternEditor::FDragStartState::FDragStartState()
    : Coord(FIntPoint::ZeroValue)
    , RuleType(EMarkerGenGridPatternRuleType::Ground) 
{
}

void FMGGridPatternEditor::FDragStartState::Init(UMarkerGenGridPatternRule* InRule) {
    if (InRule) {
        Coord = InRule->Coord;
        RuleType = InRule->RuleType;
    }

    bStateInitialized = IsValid(InRule);
}

bool FMGGridPatternEditor::FDragStartState::HasChanged(UMarkerGenGridPatternRule* InRule) const {
    if (!InRule || !bStateInitialized) return false;
    return Coord != InRule->Coord || RuleType != InRule->RuleType;
}

void FMGGridPatternEditor::SetupScene() {
	struct FSceneResources {
        TSoftObjectPtr<UBlueprint> BP_Skybox;
        TSoftObjectPtr<UBlueprint> BP_PostProcess;
        TSoftObjectPtr<UStaticMesh> SM_Cube;
        TSoftObjectPtr<UMaterialInterface> M_Flat;
        TSoftObjectPtr<UMGGridPatternEditorSettings> Settings;

        FSceneResources()
            : BP_Skybox(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Blueprints/BP_Skybox.BP_Skybox")))
            , BP_PostProcess(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Blueprints/BP_PostProcess.BP_PostProcess")))
            , SM_Cube(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Meshes/SM_Cube.SM_Cube")))
            , M_Flat(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Materials/Objects/M_Flat_Inst.M_Flat_Inst")))
	        , Settings(FSoftObjectPath(TEXT("/DungeonArchitect/Core/Editors/ThemeEditor/Settings/MarkerGenerator/Grid/ThemeEditorGridPatternEdSettings.ThemeEditorGridPatternEdSettings")))
        {
        }
    };
    static FSceneResources Resources;

    const TSharedPtr<FPreviewScene> PreviewScene = PreviewScenePtr.Pin();
    if (!PreviewScene.IsValid()) return;
    
    UWorld* World = PreviewScene->GetWorld();
    if (!World) return;
    
    if (const UBlueprint* BP_Skybox = Resources.BP_Skybox.LoadSynchronous()) {
        World->SpawnActor(BP_Skybox->GeneratedClass);
    }

    if (const UBlueprint* BP_PostProcess = Resources.BP_PostProcess.LoadSynchronous()) {
        World->SpawnActor(BP_PostProcess->GeneratedClass);
    }

    GroundPlaneActor = World->SpawnActor<AGridPatternEdGroundActor>();
    BuildSettings = Resources.Settings.LoadSynchronous();
    if (!BuildSettings) {
        BuildSettings = GetDefault<UMGGridPatternEditorSettings>();
    }
    
    check (BuildSettings);
    GroundPlaneActor->Build(BuildSettings->GroundSettings);

    CursorActor = World->SpawnActor<AGridPatternEdCursorActor>();
    CursorLocation.SetTimeToTarget(0.06f);
}

void FMGGridPatternEditor::AddReferencedObjects(FReferenceCollector& Collector) {
    Collector.AddReferencedObject(GroundPlaneActor);
    Collector.AddReferencedObject(CursorActor);
    Collector.AddReferencedObject(BuildSettings);
}

void FMGGridPatternEditor::Tick(FPatternEditorViewportClient* InViewportClient, float DeltaSeconds) {
    const int32 MouseX = InViewportClient->Viewport->GetMouseX();
    const int32 MouseY = InViewportClient->Viewport->GetMouseY();

    // Highlight the ground tile
    if (LastHoveredActor.IsValid() && !DraggedActor.IsValid()) {
        if (LastHoveredActor == GroundPlaneActor) {
            const FRay TrackingRay = GetRayFromMousePos(InViewportClient, MouseX, MouseY);
            const FVector Intersection = FMath::RayPlaneIntersection(TrackingRay.Origin, TrackingRay.Direction, FPlane(FVector::UpVector, 0));
            CursorLocation.SetTarget(Intersection, true);
        }
        else if (LastHoveredActor->IsA<AGridPatternEdRuleActor>()) {
            const FVector Intersection = LastHoveredActor->GetActorLocation();
            CursorLocation.SetTarget(Intersection, true);
        }
    }
    
    CursorLocation.Tick(DeltaSeconds);
    GroundPlaneActor->OnMouseHover(CursorLocation());
    CursorActor->SetCursorLocation(CursorLocation());
}

bool FMGGridPatternEditor::InputKey(FPatternEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InControllerId, FKey InKey, EInputEvent InEvent, float InAmountDepressed, bool bInGamepad) {
    if (InKey == EKeys::LeftMouseButton) {
        
        if (InEvent == IE_Pressed) {
            DraggedActor.Reset();
            if (LastHoveredActor.IsValid()) {
                if (AGridPatternEdRuleActor* HoveredRuleActor = Cast<AGridPatternEdRuleActor>(LastHoveredActor)) {
                    DraggedActor = HoveredRuleActor;
                    DragStartState.Init(DraggedActor.IsValid() ? DraggedActor->GetRule() : nullptr);
                    
                    SetSelectedRuleActor(HoveredRuleActor);
                }
            }
        }
        else if (InEvent == IE_Released) {
            const bool bDraggedPositionChanged = DragStartState.HasChanged(DraggedActor.IsValid() ? DraggedActor->GetRule() : nullptr);
            if (bDraggedPositionChanged) {
                if (OnRulePositionChanged.IsBound()) {
                    OnRulePositionChanged.Execute(DraggedActor->GetRule());
                }
            }
            
            DraggedActor.Reset();
        }
    }
    else if (InKey == EKeys::Delete) {
        if (SelectedActor.IsValid()) {
            DeleteRule(SelectedActor.Get(), true);
        }
    }
    
    return false;
}

void FMGGridPatternEditor::CapturedMouseMove(FPatternEditorViewportClient* InViewportClient, FViewport* InViewport, int32 InMouseX, int32 InMouseY) {
    FVector RawDelta = FVector::ZeroVector;
    if (InViewportClient && InViewportClient->GetMouseDeltaTracker()) {
        RawDelta = InViewportClient->GetMouseDeltaTracker()->GetRawDelta();
    }
    const bool bNoMouseMovement = RawDelta.SizeSquared() < MOUSE_CLICK_DRAG_DELTA;
    
    if (!bNoMouseMovement && DraggedActor.IsValid()) {
        const FRay TrackingRay = GetRayFromMousePos(InViewportClient, InMouseX, InMouseY);
        const FVector Intersection = FMath::RayPlaneIntersection(TrackingRay.Origin, TrackingRay.Direction, FPlane(FVector::UpVector, 0));

        UGridPatternEdGroundItem* GroundItem = nullptr;
        EMarkerGenGridPatternRuleType GroundItemType;
        if (GroundPlaneActor && GroundPlaneActor->Deproject(Intersection, GroundItem, GroundItemType)) {
            // Check if we have an existing item in this place
            bool bTileValid = true;
            if (PatternModel.IsValid()) {
                UMarkerGenGridPatternRule* ExistingRule = PatternModel->GetRule(GroundItem->Coord, GroundItemType);
                bTileValid = (ExistingRule == nullptr);
            }
            
            if (bTileValid) {
                DraggedActor->UpdateState(GroundItem->Coord, GroundItemType, true);
                if (CursorActor) {
                    CursorLocation.SetTarget(GroundItem->GetComponentLocation(), true);
                }
            }
        }
    }
}

void FMGGridPatternEditor::ProcessClick(FPatternEditorViewportClient* InViewportClient, FSceneView& View, HHitProxy* HitProxy, FKey Key, EInputEvent Event, uint32 HitX, uint32 HitY) {
    if(HitProxy && HitProxy->IsA(HActor::StaticGetType()))
    {
        HActor* ActorHitProxy = static_cast<HActor*>(HitProxy);
        AActor* HitActor = ActorHitProxy ? ActorHitProxy->Actor : nullptr;
        if (HitActor && HitActor->IsA<AGridPatternEdGroundActor>()) {
            if (const UGridPatternEdGroundItem* ItemComponent = Cast<UGridPatternEdGroundItem>(ActorHitProxy->PrimComponent)) {
                AGridPatternEdGroundActor* GroundActor = Cast<AGridPatternEdGroundActor>(HitActor);
                HandleGroundActorClicked(InViewportClient, GroundActor, ItemComponent, Key, Event);
            }
        }
        else if (AGridPatternEdRuleActor* RuleActor = Cast<AGridPatternEdRuleActor>(ActorHitProxy->Actor)) {
            HandleRuleActorClicked(InViewportClient, RuleActor, Key, Event);
        }
    }
}

void FMGGridPatternEditor::HandleGroundActorClicked(FPatternEditorViewportClient* InViewportClient, AGridPatternEdGroundActor* InGroundActor, const UGridPatternEdGroundItem* InItemComponent, const FKey& InKey, EInputEvent InEvent)
{
    check(InGroundActor && InItemComponent);

    SetSelectedRuleActor(nullptr);
    DraggedActor.Reset();
    
    if (InKey == EKeys::RightMouseButton) {
        // TODO: Store the context menu state (the item this was clicked on)
        ShowContextMenu();
    }
}

void FMGGridPatternEditor::HandleRuleActorClicked(FPatternEditorViewportClient* InViewportClient, AGridPatternEdRuleActor* InRuleActor, const FKey& InKey, EInputEvent InEvent) {
    check(InRuleActor);
    SetSelectedRuleActor(InRuleActor);
    
    if (InKey == EKeys::RightMouseButton) {
        // TODO: Store the context menu state (the item this was clicked on)
        ShowContextMenu();
    }
}

void FMGGridPatternEditor::SetSelectedRuleActor(AGridPatternEdRuleActor* InSelected) {
    if (SelectedActor != InSelected) {
        if (SelectedActor.IsValid()) {
            SelectedActor->SetSelected(false);
        }
    
        SelectedActor = InSelected;
        if (SelectedActor.IsValid()) {
            SelectedActor->SetSelected(true);
        }
    }

    if (OnRuleSelectionChanged.IsBound()) {
        OnRuleSelectionChanged.Execute(InSelected ? InSelected->GetRule() : nullptr);
    }
}

void FMGGridPatternEditor::ShowContextMenu() const {
    OpenViewportMenu([](FMenuBuilder& InMenuBuilder) {
        InMenuBuilder.BeginSection("GridPatternEdActions", LOCTEXT("GridPatternEdActionsSectionHeader", "Grid Pattern Actions"));
        InMenuBuilder.AddMenuEntry(FMarkerGeneratorAppModeCommands::Get().AddRule);
        InMenuBuilder.AddMenuEntry(FMarkerGeneratorAppModeCommands::Get().DeleteRule);
        
        InMenuBuilder.AddMenuSeparator();
        InMenuBuilder.AddMenuEntry(FMarkerGeneratorAppModeCommands::Get().CopyRule);
        InMenuBuilder.AddMenuEntry(FMarkerGeneratorAppModeCommands::Get().PasteRule);
        
        InMenuBuilder.EndSection();
    });
}

void FMGGridPatternEditor::FocusCameraOnScene() const {
    const TSharedPtr<FPreviewScene> PreviewScene = PreviewScenePtr.Pin();
    UWorld* World = PreviewScene.IsValid() ? PreviewScene->GetWorld() : nullptr;
    if (!World) return;

    bool bContainsRuleActors = false;
    FBox FocusBounds(ForceInit);
    
    for (TActorIterator<AGridPatternEdRuleActor> It(World); It; ++It) {
        AGridPatternEdRuleActor* RuleActor = *It;
        if (RuleActor) {
            bContainsRuleActors = true;
            FocusBounds += RuleActor->GetComponentsBoundingBox();
        }
    }

    if (!bContainsRuleActors) {
        const FVector Extent(600, 200, 600);
        FocusBounds = FBox(-Extent, Extent);
    }

    const TSharedPtr<SPatternEditorViewport> EditorViewport = EditorWidgetPtr.Pin();
    if (EditorViewport.IsValid() && EditorViewport->GetPatternViewportClient().IsValid()) {
        EditorViewport->GetPatternViewportClient()->FocusViewportOnBox(FocusBounds);
    }   
}

void FMGGridPatternEditor::RefreshRuleActor(UMarkerGenPatternRule* InRule) {
    const TSharedPtr<FPreviewScene> PreviewScene = PreviewScenePtr.Pin();
    UWorld* World = PreviewScene.IsValid() ? PreviewScene->GetWorld() : nullptr;
    if (!World) return;

    for (TActorIterator<AGridPatternEdRuleActor> It(World); It; ++It) {
        if (AGridPatternEdRuleActor* RuleActor = *It) {
            if (RuleActor->GetRule() == InRule) {
                RuleActor->RefreshCanvas();
            }
        }
    }
}

void FMGGridPatternEditor::HandleAddNewRule() {
    const UGridPatternEdGroundItem* GroundItem = Cast<UGridPatternEdGroundItem>(LastHoveredComponent.Get());
    if (GroundItem) {
        AGridPatternEdRuleActor* NewRuleActor = AddNewRule(GroundItem->Coord, GroundItem->ItemType);
        SetSelectedRuleActor(NewRuleActor);
    }
}

bool FMGGridPatternEditor::CanAddNewRule() const {
    return LastHoveredActor.IsValid()
        && LastHoveredActor->IsA<AGridPatternEdGroundActor>()
        && LastHoveredComponent.IsValid()
        && LastHoveredComponent->IsA<UGridPatternEdGroundItem>();
}

void FMGGridPatternEditor::HandleDeleteRule() {
    DeleteRule(SelectedActor.Get(), true);
}

bool FMGGridPatternEditor::CanDeleteRule() const {
    return SelectedActor.IsValid();
}

void FMGGridPatternEditor::HandleCopyRule() {
    CopiedRule = SelectedActor.IsValid() ? SelectedActor->GetRule() : nullptr;
}

bool FMGGridPatternEditor::CanCopyRule() const {
    return SelectedActor.IsValid();
}

void FMGGridPatternEditor::HandlePasteRule() {
    // Undo/Redo support
    const FScopedTransaction Transaction(LOCTEXT("PasteTransactionID", "Paste Pattern Rule"));

    const UGridPatternEdGroundItem* GroundItem = Cast<UGridPatternEdGroundItem>(LastHoveredComponent.Get());
    const UMarkerGenGridPatternRule* RuleTemplate = CopiedRule.IsValid() ? CopiedRule.Get() : nullptr;
    if (GroundItem && RuleTemplate) {
        AGridPatternEdRuleActor* NewRuleActor = AddNewRule(GroundItem->Coord, GroundItem->ItemType, RuleTemplate);
        SetSelectedRuleActor(NewRuleActor);
    }
}

bool FMGGridPatternEditor::CanPasteRule() const {
    return CopiedRule.IsValid() && CanAddNewRule();
}

void FMGGridPatternEditor::DeleteRule(AGridPatternEdRuleActor* InRuleActorToDelete, bool bShowConfirmation) {
    if (!InRuleActorToDelete) return;
    
    bool bShouldDelete = true;
    if (bShowConfirmation) {
        static const FText Title = LOCTEXT("DeleteMsgTitle", "Delete Rule?");
        const EAppReturnType::Type ReturnValue = FMessageDialog::Open(EAppMsgType::YesNo, EAppReturnType::No,
                                                                LOCTEXT("DeleteMsgDesc", "Are you sure you want to delete the selected rule?"), &Title);
        bShouldDelete = (ReturnValue == EAppReturnType::Yes);
    }

    if (bShouldDelete) {
        if (SelectedActor == InRuleActorToDelete) {
            SelectedActor.Reset();
        }
        
        if (PatternModel.IsValid() && InRuleActorToDelete->GetRule()) {
            PatternModel->Rules.Remove(InRuleActorToDelete->GetRule());
        }
        InRuleActorToDelete->Destroy();
    }
}

AGridPatternEdRuleActor* FMGGridPatternEditor::AddNewRule(const FIntPoint& InCoord, EMarkerGenGridPatternRuleType InRuleType, const UMarkerGenGridPatternRule* InTemplate) {
    if (!PatternModel.IsValid()) return nullptr;
    
    const UMarkerGenGridPatternRule* ExistingRule = PatternModel->GetRule(InCoord, InRuleType);
    if (IsValid(ExistingRule)) {
        // Rule already exists
        return nullptr;
    }

    // Spawn a new actor to represent this rule
    const TSharedPtr<FPreviewScene> PreviewScene = PreviewScenePtr.Pin();
    UWorld* World = PreviewScene.IsValid() ? PreviewScene->GetWorld() : nullptr;
    if (!World) return nullptr;
    
    // Create a new rule at the location
    UMarkerGenGridPatternRule* NewRule = PatternModel->AddNewRule(InCoord, InRuleType);

    if (InTemplate && InTemplate->RuleEdGraph) {
        NewRule->bHintWillInsertAssetHere = InTemplate->bHintWillInsertAssetHere;
        NewRule->bVisuallyDominant = InTemplate->bVisuallyDominant;
        NewRule->RuleEdGraph = FEdGraphUtilities::CloneGraph(InTemplate->RuleEdGraph, NewRule);
    }
    else {
        UMGPatternGraph* PatternGraph = NewObject<UMGPatternGraph>(NewRule);
        PatternGraph->Initialize();
        NewRule->RuleEdGraph = PatternGraph;
    }
    FMGPatternGraphCompiler::Compile(NewRule);
    
    AGridPatternEdRuleActor* NewRuleActor = CreateRuleActor(World, NewRule);

    const TSharedPtr<SPatternEditorViewport> EditorViewport = EditorWidgetPtr.Pin();
    if (EditorViewport.IsValid()) {
        EditorViewport->GetPatternViewportClient()->Invalidate();
    }

    return NewRuleActor;
}

AGridPatternEdRuleActor* FMGGridPatternEditor::CreateRuleActor(UWorld* InWorld, UMarkerGenGridPatternRule* InRule) const {
    AGridPatternEdRuleActor* RuleActor = InWorld->SpawnActor<AGridPatternEdRuleActor>();
    RuleActor->Initialize(BuildSettings->GroundSettings, InRule);
    return RuleActor;
}

void FMGGridPatternEditor::CheckHoveredHitProxy(FPatternEditorViewportClient* InViewportClient, HHitProxy* HoveredHitProxy) {
    const HActor* HoveredActorHitProxy = static_cast<HActor*>(HoveredHitProxy);
    if (HoveredHitProxy) {
        LastHoveredActor = HoveredActorHitProxy->Actor;
        LastHoveredComponent = HoveredActorHitProxy->PrimComponent;
    }
    else {
        LastHoveredActor = nullptr;
        LastHoveredComponent = nullptr;
    }
}

void FMGGridPatternEditor::InvalidateRuleVisuals(const UMarkerGenPatternRule* InRule) {
    if (!InRule) return;
    
    const TSharedPtr<FPreviewScene> PreviewScene = PreviewScenePtr.Pin();
    UWorld* World = PreviewScene.IsValid() ? PreviewScene->GetWorld() : nullptr;
    if (!World) return;

    for (TActorIterator<AGridPatternEdRuleActor> It(World); It; ++It) {
        AGridPatternEdRuleActor* RuleActor = *It;
        if (RuleActor && RuleActor->GetRule() == InRule) {
            RuleActor->UpdateTransform(true);
            RuleActor->UpdateColor();
            RuleActor->RefreshCanvas();
            break;
        }
    }
}

void FMGGridPatternEditor::Load(UMarkerGenPattern* InPattern) {
    // Destroy all the rule actors in the scene
    DraggedActor.Reset();
    SelectedActor.Reset();
    
    const TSharedPtr<FPreviewScene> PreviewScene = PreviewScenePtr.Pin();
    UWorld* World = PreviewScene.IsValid() ? PreviewScene->GetWorld() : nullptr;
    if (!World) return;

    for (TActorIterator<AGridPatternEdRuleActor> It(World); It; ++It) {
        if (AGridPatternEdRuleActor* RuleActor = *It) {
            RuleActor->Destroy();
        }
    }

    UMarkerGenGridPattern* GridPattern = Cast<UMarkerGenGridPattern>(InPattern);
    PatternModel = GridPattern;
    if (!GridPattern) return;

    for (UMarkerGenPatternRule* Rule : GridPattern->Rules) {
        if (UMarkerGenGridPatternRule* GridRule = Cast<UMarkerGenGridPatternRule>(Rule)) {
            CreateRuleActor(World, GridRule);
        }
    }
    
    const TSharedPtr<SPatternEditorViewport> EditorViewport = EditorWidgetPtr.Pin();
    if (EditorViewport.IsValid()) {
        EditorViewport->GetPatternViewportClient()->Invalidate();
    }
}

#undef LOCTEXT_NAMESPACE

