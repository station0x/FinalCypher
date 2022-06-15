//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/DungeonArchitectThemeEditor.h"

#include "Core/Common/DungeonArchitectCommands.h"
#include "Core/DungeonBuilder.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppMode.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppModeCommands.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeEditorAppModeBase.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/Graph/EdGraph_DungeonProp.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/ThemeGraphAppMode.h"
#include "Core/Editors/ThemeEditor/Widgets/SThemePreviewViewport.h"
#include "Core/LevelEditor/Customizations/DungeonArchitectStyle.h"
#include "Frameworks/MarkerGenerator/MarkerGenModel.h"

#include "AdvancedPreviewScene.h"
#include "ContentBrowserModule.h"
#include "EdGraph/EdGraph.h"
#include "EditorSupportDelegates.h"
#include "EngineUtils.h"
#include "FileHelpers.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IContentBrowserSingleton.h"
#include "SSingleObjectDetailsPanel.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Toolkits/IToolkitHost.h"
#include "Toolkits/ToolkitManager.h"
#include "WorkflowOrientedApp/ApplicationMode.h"
#include "WorkflowOrientedApp/SModeWidget.h"

#define LOCTEXT_NAMESPACE "FDungeonArchitectThemeEditor"

DEFINE_LOG_CATEGORY_STATIC(DungeonEditor, Log, All);

const FName DungeonEditorAppName = FName(TEXT("DungeonEditorApp"));
TSharedPtr<FDungeonEditorThumbnailPool> FDungeonEditorThumbnailPool::Instance;


//////////////////////////////////////////////////////////////////////////
///
const FName FDungeonArchitectThemeEditor::AppModeID_GraphEditor(TEXT("GraphEditor"));
const FName FDungeonArchitectThemeEditor::AppModeID_MarkerGenerator(TEXT("MarkerGenerator"));

void FDungeonArchitectThemeEditor::InitDungeonEditor(const EToolkitMode::Type Mode,
                                                     const TSharedPtr<class IToolkitHost>& InitToolkitHost,
                                                     UDungeonThemeAsset* PropData) {
    // Initialize the asset editor and spawn nothing (dummy layout)
    GEditor->GetEditorSubsystem<UAssetEditorSubsystem>()->CloseOtherEditors(PropData, this);
    AssetBeingEdited = PropData;

    const TSubclassOf<UDungeonBuilder> ActiveDungeonBuilderClass = AssetBeingEdited->PreviewViewportProperties->BuilderClass; 
    if (!AssetBeingEdited->UpdateGraph) {
        UEdGraph_DungeonProp* DungeonGraph = NewObject<UEdGraph_DungeonProp>(
                AssetBeingEdited, UEdGraph_DungeonProp::StaticClass(), NAME_None, RF_Transactional);
        DungeonGraph->RecreateDefaultMarkerNodes(ActiveDungeonBuilderClass);
        AssetBeingEdited->UpdateGraph = DungeonGraph;
        AssetBeingEdited->Modify();
    }

    if (!AssetBeingEdited->MarkerGenerationModel) {
        AssetBeingEdited->MarkerGenerationModel = NewObject<UMarkerGenModel>(AssetBeingEdited);
        AssetBeingEdited->Modify();
    }

    BindCommands();
    ExtendMenu();
    ToolbarBuilder = MakeShareable(new FThemeEditorToolbar(SharedThis(this)));

    PreviewViewport = SNew(SThemePreviewViewport)
        .DungeonEditor(SharedThis(this))
        .ObjectToEdit(AssetBeingEdited);

    constexpr bool bCreateDefaultStandaloneMenu = true;
    constexpr bool bCreateDefaultToolbar = true;
    const TSharedRef<FTabManager::FLayout> DummyLayout = FTabManager::NewLayout("NullLayout")->AddArea(FTabManager::NewPrimaryArea());
    InitAssetEditor(Mode, InitToolkitHost, DungeonEditorAppName, DummyLayout, bCreateDefaultStandaloneMenu, bCreateDefaultToolbar, PropData);
    
    AppModes.Add(AppModeID_GraphEditor, MakeShareable(new FThemeGraphAppMode(SharedThis(this), PreviewViewport)));
    AppModes.Add(AppModeID_MarkerGenerator, MakeShareable(new FMarkerGeneratorAppMode(SharedThis(this), PreviewViewport)));

    for (const auto& Entry : AppModes) {
        TSharedPtr<FThemeEditorAppModeBase> AppMode = Entry.Value;
        AppMode->Init(ActiveDungeonBuilderClass);
        AddApplicationMode(Entry.Key, AppMode.ToSharedRef());        
    }

    SetCurrentMode(AppModeID_GraphEditor);
}

bool FDungeonArchitectThemeEditor::CanAccessModeThemeGraph() const {
    return true;
}

bool FDungeonArchitectThemeEditor::CanAccessModeMarkerGenerator() const {
    return true;
}

FName FDungeonArchitectThemeEditor::GetToolkitFName() const {
    return FName("DungeonEditor");
}

FText FDungeonArchitectThemeEditor::GetBaseToolkitName() const {
    return LOCTEXT("DungeonEditorAppLabel", "Dungeon Editor");
}

FText FDungeonArchitectThemeEditor::GetToolkitName() const {
    if (!AssetBeingEdited) {
        return FWorkflowCentricApplication::GetToolkitName();
    }
    
    return FText::FromString(AssetBeingEdited->GetName());
}

FString FDungeonArchitectThemeEditor::GetWorldCentricTabPrefix() const {
    return TEXT("DungeonEditor");
}

FString FDungeonArchitectThemeEditor::GetDocumentationLink() const {
    return TEXT("Dungeon/DungeonEditor");
}

FLinearColor FDungeonArchitectThemeEditor::GetWorldCentricTabColorScale() const {
    return FLinearColor::White;
}

bool FDungeonArchitectThemeEditor::IsTickableInEditor() const {
    return true;
}

void FDungeonArchitectThemeEditor::Tick(float DeltaTime) {
    const TSharedPtr<FThemeEditorAppModeBase> CurrentMode = StaticCastSharedPtr<FThemeEditorAppModeBase>(GetCurrentModePtr());
    if (CurrentMode.IsValid()) {
        CurrentMode->Tick(DeltaTime);
    }
}

bool FDungeonArchitectThemeEditor::IsTickable() const {
    return true;
}

TStatId FDungeonArchitectThemeEditor::GetStatId() const {
    return TStatId();
}

void FDungeonArchitectThemeEditor::BindCommands() {
    const FDungeonArchitectCommands& Commands = FDungeonArchitectCommands::Get();

    ToolkitCommands->MapAction(
        Commands.OpenHelpSystem,
        FExecuteAction::CreateSP(this, &FDungeonArchitectThemeEditor::HandleOpenHelpSystem));

}

void FDungeonArchitectThemeEditor::NotifyBuilderClassChanged(TSubclassOf<UDungeonBuilder> InBuilderClass) {
    for (const auto& Entry : AppModes) {
        Entry.Value->SetBuilderClass(InBuilderClass);        
    }
}

void FDungeonArchitectThemeEditor::SaveAsset_Execute() {
    UE_LOG(DungeonEditor, Log, TEXT("Saving dungeon theme %s"), *GetEditingObjects()[0]->GetName());
    CompileThemeGraph();

    UPackage* Package = AssetBeingEdited->GetOutermost();
    if (Package) {
        TArray<UPackage*> PackagesToSave;
        PackagesToSave.Add(Package);
        
        UpdateThumbnail();
        
        FEditorFileUtils::PromptForCheckoutAndSave(PackagesToSave, false, false);

        if (PreviewViewport.IsValid()) {
            PreviewViewport->RebuildMeshes();
        }
    }
}

void FDungeonArchitectThemeEditor::CompileThemeGraph() const {
    if (const UEdGraph_DungeonProp* DungeonGraphEditor = Cast<UEdGraph_DungeonProp>(AssetBeingEdited->UpdateGraph)) {
        TArray<FPropTypeData> Props;
        TArray<FDungeonGraphBuildError> CompileErrors;
        DungeonGraphEditor->RebuildGraph(AssetBeingEdited, Props, CompileErrors);
        if (CompileErrors.Num() == 0) {
            AssetBeingEdited->Props = Props;
        }
        else {
            // TODO: Display the error messages on the graph nodes
        }
    }
    FEditorDelegates::RefreshEditor.Broadcast();
    FEditorSupportDelegates::RedrawAllViewports.Broadcast();
}

FDungeonArchitectThemeEditor::~FDungeonArchitectThemeEditor() {
    
}

void FDungeonArchitectThemeEditor::ExtendMenu() {

}

void FDungeonArchitectThemeEditor::UpdateThumbnail() const {
    if (!AssetBeingEdited) return;
    FViewport* PreviewViewportPtr = nullptr;
    if (PreviewViewport.IsValid() && PreviewViewport->GetViewportClient().IsValid()) {
        PreviewViewportPtr = PreviewViewport->GetViewportClient()->Viewport;
    }

    if (PreviewViewportPtr) {
        const FAssetData AssetData(AssetBeingEdited);
        TArray<FAssetData> ThemeAssetList;
        ThemeAssetList.Add(AssetData);

        IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();
        ContentBrowserSingleton.CaptureThumbnailFromViewport(PreviewViewportPtr, ThemeAssetList);
    }
}


void FDungeonArchitectThemeEditor::HandleOpenHelpSystem() {
    static const FName HelpSystemApp = FName(TEXT("DAHelpSystemApp"));
    FGlobalTabmanager::Get()->TryInvokeTab(HelpSystemApp);
}

FText FDungeonArchitectThemeEditor::GetLocalizedMode(FName InMode) {
    static TMap<FName, FText> LocModes;

    if (LocModes.Num() == 0) {
        LocModes.Add(AppModeID_GraphEditor, LOCTEXT("ThemeGraphModeLabel", "Theme Graph"));
        LocModes.Add(AppModeID_MarkerGenerator, LOCTEXT("MarkerGeneratorModeLabel", "Pattern Matcher"));
    }

    check(InMode != NAME_None);
    const FText* OutDesc = LocModes.Find(InMode);
    check(OutDesc);
    return *OutDesc;
}

void FDungeonArchitectThemeEditor::RegisterToolbarTab(const TSharedRef<FTabManager>& InTabManager) {
    FAssetEditorToolkit::RegisterTabSpawners(InTabManager);
}

//////////////////////////////////////// FThemeEditorToolbar ////////////////////////////////////////  
void FThemeEditorToolbar::AddModesToolbar(TSharedPtr<FExtender> Extender) const {
    const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
    if (ThemeEditor.IsValid()) {
        Extender->AddToolBarExtension(
        "Asset",
        EExtensionHook::After,
        ThemeEditor->GetToolkitCommands(),
        FToolBarExtensionDelegate::CreateSP(this, &FThemeEditorToolbar::FillModesToolbar));
    }
}

void FThemeEditorToolbar::AddThemeGraphToolbar(TSharedPtr<FExtender> Extender) {
    const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
    if (ThemeEditor.IsValid()) {
        Extender->AddToolBarExtension("Asset",
            EExtensionHook::After,
            ThemeEditor->GetToolkitCommands(),
            FToolBarExtensionDelegate::CreateSP(this, &FThemeEditorToolbar::FillThemeGraphToolbar));
    }
}

void FThemeEditorToolbar::AddMarkerGeneratorToolbar(TSharedPtr<FExtender> Extender) {
    const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
    if (ThemeEditor.IsValid()) {
        Extender->AddToolBarExtension("Support",
            EExtensionHook::Before,
            ThemeEditor->GetToolkitCommands(),
            FToolBarExtensionDelegate::CreateSP(this, &FThemeEditorToolbar::FillMarkerGeneratorToolbar));
    }
}


void FThemeEditorToolbar::FillModesToolbar(FToolBarBuilder& ToolbarBuilder) const {
    ToolbarBuilder.BeginSection("Support");
    {
        ToolbarBuilder.AddToolBarButton(FDungeonArchitectCommands::Get().OpenHelpSystem);
    }
    ToolbarBuilder.EndSection();
    
    const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
    if (!ThemeEditor.IsValid()) return;

    const TAttribute<FName> GetActiveMode(ThemeEditor.ToSharedRef(), &FDungeonArchitectThemeEditor::GetCurrentMode);
    const FOnModeChangeRequested SetActiveMode = FOnModeChangeRequested::CreateSP(ThemeEditor.ToSharedRef(), &FDungeonArchitectThemeEditor::SetCurrentMode);
    
    // Left side padding
    ThemeEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

    // Design Mode
    ThemeEditor->AddToolbarWidget(
        SNew(SModeWidget, FDungeonArchitectThemeEditor::GetLocalizedMode(FDungeonArchitectThemeEditor::AppModeID_GraphEditor),
                FDungeonArchitectThemeEditor::AppModeID_GraphEditor)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(ThemeEditor.Get(), &FDungeonArchitectThemeEditor::CanAccessModeThemeGraph)
		.ToolTipText(LOCTEXT("ModeButtonTooltip_ThemeGraph", "Switch to Theme Graph Mode"))
		.IconImage(FDungeonArchitectStyle::Get().GetBrush("DA.SnapEd.SwitchToDesignMode"))
    );

    ThemeEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));

    // Visualization Mode
    ThemeEditor->AddToolbarWidget(
        SNew(SModeWidget, FDungeonArchitectThemeEditor::GetLocalizedMode(FDungeonArchitectThemeEditor::AppModeID_MarkerGenerator),
                FDungeonArchitectThemeEditor::AppModeID_MarkerGenerator)
		.OnGetActiveMode(GetActiveMode)
		.OnSetActiveMode(SetActiveMode)
		.CanBeSelected(ThemeEditor.Get(), &FDungeonArchitectThemeEditor::CanAccessModeMarkerGenerator)
		.ToolTipText(LOCTEXT("ModeButtonTooltip_MarkerGenerator", "Switch to Marker generation mode"))
		.IconImage(FDungeonArchitectStyle::Get().GetBrush("DA.SnapEd.SwitchToVisualizeMode"))
    );

    // Right side padding
    ThemeEditor->AddToolbarWidget(SNew(SSpacer).Size(FVector2D(4.0f, 1.0f)));
}

void FThemeEditorToolbar::FillThemeGraphToolbar(FToolBarBuilder& ToolbarBuilder) {
    
}

void FThemeEditorToolbar::FillMarkerGeneratorToolbar(FToolBarBuilder& ToolbarBuilder) {
    const FMarkerGeneratorAppModeCommands& Commands = FMarkerGeneratorAppModeCommands::Get();
    ToolbarBuilder.AddSeparator();
    ToolbarBuilder.AddToolBarButton(Commands.Build);
}


//////////////////////////////////////// FDungeonArchitectThemeEditorUtils ////////////////////////////////////////  
bool FDungeonArchitectThemeEditorUtils::GetBoundsForSelectedNodes(const UEdGraph* Graph, FSlateRect& Rect, float Padding) {
    const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = GetThemeEditorForAsset(Graph);
    if (ThemeEditor.IsValid()) {
        const TSharedPtr<FThemeGraphAppMode> AppMode = ThemeEditor->GetAppMode<FThemeGraphAppMode>(FDungeonArchitectThemeEditor::AppModeID_GraphEditor);
        if (AppMode.IsValid()) {
            const TSharedPtr<SGraphEditor> GraphEditor = AppMode->GetGraphEditor();
            if (GraphEditor.IsValid()) {
                return GraphEditor->GetBoundsForSelectedNodes(Rect, Padding);
            }
        }
    }
    return false;
}

TSharedPtr<FDungeonArchitectThemeEditor> FDungeonArchitectThemeEditorUtils::GetThemeEditorForAsset(const UEdGraph* Graph) {
    if (!Graph) {
        return nullptr;
    }
    
    UDungeonThemeAsset* ThemeAsset = Cast<UDungeonThemeAsset>(Graph->GetOuter());
    TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor;
    if (ThemeAsset) {
        TSharedPtr< IToolkit > FoundAssetEditor = FToolkitManager::Get().FindEditorForAsset(ThemeAsset);
        if (FoundAssetEditor.IsValid()) {
            ThemeEditor = StaticCastSharedPtr<FDungeonArchitectThemeEditor>(FoundAssetEditor);
        }
    }
    return ThemeEditor;
}

#undef LOCTEXT_NAMESPACE

