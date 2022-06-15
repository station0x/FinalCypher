//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/ThemeGraphAppMode.h"

#include "Core/DungeonBuilder.h"
#include "Core/Editors/ThemeEditor/AppModes/Common/ThemeEditorAppTabFactoryMacros.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/Graph/EdGraphNode_DungeonActorTemplate.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/Graph/EdGraphNode_DungeonBase.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/Graph/EdGraphNode_DungeonMarker.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeGraph/Graph/EdGraph_DungeonProp.h"
#include "Core/Editors/ThemeEditor/DungeonThemeGraphHandler.h"
#include "Core/Editors/ThemeEditor/Widgets/SGraphPalette_PropActions.h"
#include "Core/Editors/ThemeEditor/Widgets/SMarkerListView.h"
#include "Core/Editors/ThemeEditor/Widgets/SThemeEditorDropTarget.h"
#include "Core/Editors/ThemeEditor/Widgets/SThemePreviewViewport.h"

#include "AdvancedPreviewScene.h"
#include "AssetRegistry/AssetData.h"
#include "AssetSelection.h"
#include "ContentBrowserModule.h"
#include "EdGraph/EdGraph.h"
#include "EdGraphUtilities.h"
#include "Editor/WorkspaceMenuStructure/Public/WorkspaceMenuStructureModule.h"
#include "EngineUtils.h"
#include "GraphEditAction.h"
#include "GraphEditor.h"
#include "GraphEditorActions.h"
#include "HAL/PlatformApplicationMisc.h"
#include "IContentBrowserSingleton.h"
#include "IDetailsView.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "PropertyEditorModule.h"
#include "SAdvancedPreviewDetailsTab.h"
#include "SNodePanel.h"
#include "Widgets/Docking/SDockTab.h"

#define LOCTEXT_NAMESPACE "FThemeGraphAppMode"
DEFINE_LOG_CATEGORY_STATIC(ThemeEditorGraphMode, Log, All);

namespace ThemeGraphAppModeTabs {
	static const FName TabID_GraphEditor(TEXT("GraphEditor"));
	static const FName TabID_Preview(TEXT("Preview"));
	static const FName TabID_Details(TEXT("Details"));
	static const FName TabID_ContentBrowser(TEXT("ContentBrowser"));
	static const FName TabID_Markers(TEXT("Markers"));
	static const FName TabID_Actions(TEXT("Actions"));
	static const FName TabID_PreviewSettings(TEXT("PreviewSettings"));

	DEFINE_THEME_EDITOR_TAB_FACTORY(GraphEditor,
				LOCTEXT("Caption_GraphEditor", "Theme Graph"),
				"LevelEditor.Tabs.Viewports",
				LOCTEXT("TooltipMenu_GraphEditor", "Design your theme by placing assets in this graph editor"),
				LOCTEXT("TooltipTab_GraphEditor", "Design your theme by placing assets in this graph editor"))
	
	DEFINE_THEME_EDITOR_TAB_FACTORY(Details,
				LOCTEXT("Caption_Details", "Details"),
				"LevelEditor.Tabs.Details",
				LOCTEXT("TooltipMenu_Details", "Details panel for modifying the node properties"),
				LOCTEXT("TooltipTab_Details", "Details panel for modifying the node properties"))

	DEFINE_THEME_EDITOR_TAB_FACTORY(Markers,
				LOCTEXT("Caption_Markers", "Markers"),
				"Kismet.Tabs.Palette",
				LOCTEXT("TooltipMenu_Markers", "Displays a list of markers in the theme graph."),
				LOCTEXT("TooltipTab_Markers", "Displays a list of markers in the theme graph. Double click to jump to a node"))

	DEFINE_THEME_EDITOR_TAB_FACTORY(Actions,
				LOCTEXT("Caption_Actions", "Actions"),
				"LevelEditor.Tabs.Layers",
				LOCTEXT("TooltipMenu_Markers", "Displays a list of actions to perform on the theme graph"),
				LOCTEXT("TooltipTab_Markers", "Displays a list of actions to perform on the theme graph."))

	
	DEFINE_THEME_EDITOR_TAB_FACTORY(PreviewSettings,
				LOCTEXT("Caption_PreviewSettings", "Preview Settings"),
				"LevelEditor.Tabs.Properties",
				LOCTEXT("TooltipMenu_PreviewSettings", "Displays a list of actions to perform on the theme graph"),
				LOCTEXT("TooltipTab_PreviewSettings", "Displays a list of actions to perform on the theme graph."))

	DEFINE_THEME_EDITOR_TAB_FACTORY_CUSTOM_SPAWNER(Preview,
				LOCTEXT("Caption_PreviewViewport", "Preview"),
				"LevelEditor.Tabs.Viewports",
				LOCTEXT("TooltipMenu_Preview", "Preview the generated dungeon in a 3D viewport"),
				LOCTEXT("TooltipTab_Preivew", "Preview the generated dungeon in a 3D viewport"))

	TSharedRef<SDockTab> FThemeEditorTabFactory_Preview::SpawnTab(const FWorkflowTabSpawnInfo& Info) const {
		TSharedRef<SDockTab> DockTab = FWorkflowTabFactory::SpawnTab(Info);
		const TSharedPtr<SThemePreviewViewport> PreviewViewport = StaticCastSharedPtr<SThemePreviewViewport>(WidgetPtr.Pin());
		if (PreviewViewport.IsValid()) {
			PreviewViewport->SetParentTab(FDungeonArchitectThemeEditor::AppModeID_GraphEditor, DockTab);
		}
		return DockTab;;
	}
	
	DEFINE_THEME_EDITOR_TAB_FACTORY_CUSTOM_SPAWNER(ContentBrowser,
				LOCTEXT("Caption_ContentBrowser", "Content Browser"),
				"LevelEditor.Tabs.ContentBrowser",
				LOCTEXT("TooltipMenu_ContentBrowser", "Content browser to help drag-drop assets into the theme graph"),
				LOCTEXT("TooltipTab_ContentBrowser", "Content browser to help drag-drop assets into the theme graph"))

	TSharedRef<SDockTab> FThemeEditorTabFactory_ContentBrowser::SpawnTab(const FWorkflowTabSpawnInfo& Info) const {
		TSharedRef<SDockTab> SpawnedTab = FWorkflowTabFactory::SpawnTab(Info);
		IContentBrowserSingleton& ContentBrowserSingleton = FModuleManager::LoadModuleChecked<FContentBrowserModule>("ContentBrowser").Get();

		const FName ContentBrowserID = TEXT("DA_ThemeEditor_ContentBrowser");
		FContentBrowserConfig ContentBrowserConfig;
		{
			/*
			ContentBrowserConfig.ThumbnailLabel =  EThumbnailLabel::ClassName ;
			ContentBrowserConfig.ThumbnailScale = 0.1f;
			ContentBrowserConfig.InitialAssetViewType = EAssetViewType::Column;
			ContentBrowserConfig.bShowBottomToolbar = true;
			ContentBrowserConfig.bCanShowClasses = true;
			ContentBrowserConfig.bUseSourcesView = true;
			ContentBrowserConfig.bExpandSourcesView = true;
			ContentBrowserConfig.bUsePathPicker = true;
			ContentBrowserConfig.bCanShowFilters = true;
			ContentBrowserConfig.bCanShowAssetSearch = true;
			ContentBrowserConfig.bCanShowFolders = true;
			ContentBrowserConfig.bCanShowRealTimeThumbnails = true;
			ContentBrowserConfig.bCanShowLockButton = true;
			*/
			ContentBrowserConfig.bCanShowDevelopersFolder = true;
			ContentBrowserConfig.bCanSetAsPrimaryBrowser = false;
		}
		
		const TSharedRef<SWidget> ContentBrowser = ContentBrowserSingleton.CreateContentBrowser(ContentBrowserID, SpawnedTab, &ContentBrowserConfig);
		SpawnedTab->SetContent(ContentBrowser);
		
		return SpawnedTab;;
	}
	
}

namespace {
	template <typename T>
	void GetGraphNodes(UEdGraph* Graph, TArray<T*>& OutResult) {
		OutResult.Reset();
		for (UEdGraphNode* Node : Graph->Nodes) {
			if (Node && Node->IsA<T>()) {
				OutResult.Add(Cast<T>(Node));
			}
		}
	}

	void GetMarkerNodes(UDungeonThemeAsset* ThemeAsset, TArray<TSharedPtr<FMarkerListEntry>>& OutResult) {
		OutResult.Reset();
		if (ThemeAsset && ThemeAsset->UpdateGraph) {
			TArray<UEdGraphNode_DungeonMarker*> MarkerNodes;
			GetGraphNodes<UEdGraphNode_DungeonMarker>(ThemeAsset->UpdateGraph, MarkerNodes);
			for (UEdGraphNode_DungeonMarker* MarkerNode : MarkerNodes) {
				TSharedPtr<FMarkerListEntry> Item = MakeShareable(new FMarkerListEntry);
				Item->MarkerName = MarkerNode->MarkerName;
				Item->MarkerNode = MarkerNode;
				OutResult.Add(Item);
			}
		}
	}
	
	template <typename T>
	bool ContainsNodesOfType(TSet<const UEdGraphNode*> Nodes) {
		for (const UEdGraphNode* Node : Nodes) {
			if (Cast<const T>(Node)) {
				return true;
			}
		}
		return false;
	}

}


FThemeGraphAppMode::FThemeGraphAppMode(TSharedPtr<FDungeonArchitectThemeEditor> InThemeEditor, TSharedPtr<SThemePreviewViewport> InPreviewViewport)
		: FThemeEditorAppModeBase(InThemeEditor, FDungeonArchitectThemeEditor::AppModeID_GraphEditor)
		, PreviewViewportPtr(InPreviewViewport)
{
}

void FThemeGraphAppMode::Init(TSubclassOf<UDungeonBuilder> InBuilderClass) {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
	if (!ThemeEditor.IsValid()) return;
	
    ActionPalette = SNew(SGraphPalette_PropActions, GraphEditor);
	
	MarkerListView = SNew(SMarkerListView)
		.OnMarkerDoubleClicked(this, &FThemeGraphAppMode::OnMarkerListDoubleClicked);
	RefreshMarkerListView();

	ThemeGraphHandler = MakeShareable(new FDungeonArchitectThemeGraphHandler);
	ThemeGraphHandler->Bind();
	ThemeGraphHandler->GetGraphEvents().OnTextCommitted = FOnNodeTextCommitted::CreateSP(this, &FThemeGraphAppMode::OnNodeTitleCommitted);
	ThemeGraphHandler->NodeDoubleClicked = FDungeonArchitectThemeGraphHandler::FSingleNodeDelegate::CreateSP(this, &FThemeGraphAppMode::OnNodeDoubleClicked);
	ThemeGraphHandler->SelectedNodesChanged = FDungeonArchitectThemeGraphHandler::FMultiNodeDelegate::CreateSP(this, &FThemeGraphAppMode::SelectedNodesChanged);

	UDungeonThemeAsset* AssetBeingEdited = ThemeEditor->GetAssetBeingEdited();
	
	CreateGraphEditorWidget(AssetBeingEdited->UpdateGraph);
	
	PropertyEditor = CreatePropertyEditorWidget();
	ThemeGraphHandler->Initialize(GraphEditor, PropertyEditor, AssetBeingEdited->UpdateGraph, AssetBeingEdited->PreviewViewportProperties);

	const TSharedPtr<SThemePreviewViewport> PreviewViewport = PreviewViewportPtr.Pin();
	if (PreviewViewport.IsValid()) {
		TSharedPtr<FAdvancedPreviewScene> PreviewScene;
    	if (PreviewViewport.IsValid()) {
    		PreviewScene = PreviewViewport->GetAdvancedPreview();
    	}

    	if (PreviewScene.IsValid()) {
	        const TSharedPtr<SAdvancedPreviewDetailsTab> SettingsWidget = SNew(SAdvancedPreviewDetailsTab, PreviewScene.ToSharedRef());
    		SettingsWidget->Refresh();
    		PreviewSettingsWidget = SettingsWidget;
    	}
    	else {
			PreviewSettingsWidget = SNullWidget::NullWidget;
    	}
	}
	
	TabFactories.RegisterFactory(MakeShareable(new ThemeGraphAppModeTabs::FThemeEditorTabFactory_GraphEditor(ThemeEditor, GraphEditorHost)));
	TabFactories.RegisterFactory(MakeShareable(new ThemeGraphAppModeTabs::FThemeEditorTabFactory_Preview(ThemeEditor, PreviewViewport)));
	TabFactories.RegisterFactory(MakeShareable(new ThemeGraphAppModeTabs::FThemeEditorTabFactory_Details(ThemeEditor, PropertyEditor)));
	TabFactories.RegisterFactory(MakeShareable(new ThemeGraphAppModeTabs::FThemeEditorTabFactory_ContentBrowser(ThemeEditor)));
	TabFactories.RegisterFactory(MakeShareable(new ThemeGraphAppModeTabs::FThemeEditorTabFactory_Markers(ThemeEditor, MarkerListView)));
	TabFactories.RegisterFactory(MakeShareable(new ThemeGraphAppModeTabs::FThemeEditorTabFactory_Actions(ThemeEditor, ActionPalette)));
	TabFactories.RegisterFactory(MakeShareable(new ThemeGraphAppModeTabs::FThemeEditorTabFactory_PreviewSettings(ThemeEditor, PreviewSettingsWidget)));

	TabLayout = FTabManager::NewLayout(
            "ThemeGraphAppMode_Layout_v0.0.2")
        ->AddArea
        (
            FTabManager::NewPrimaryArea()
            ->SetOrientation(Orient_Vertical)
            ->Split
            (
                FTabManager::NewSplitter()
                ->SetOrientation(Orient_Horizontal)
                ->SetSizeCoefficient(0.63f)
                ->Split
                (

                    FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Vertical)
                    ->SetSizeCoefficient(0.66f)
                    ->Split // Graph Widget
                    (
                        FTabManager::NewStack()
                        ->SetSizeCoefficient(0.66f)
                        ->SetHideTabWell(true)
                        ->AddTab(ThemeGraphAppModeTabs::TabID_GraphEditor, ETabState::OpenedTab)
                    )
                    ->Split // ContentBrowser
                    (
                        FTabManager::NewStack()
                        ->SetSizeCoefficient(0.33f)
                        ->AddTab(ThemeGraphAppModeTabs::TabID_ContentBrowser, ETabState::OpenedTab)
                    )
                )
                ->Split
                (
                    FTabManager::NewSplitter()
                    ->SetOrientation(Orient_Vertical)
                    ->SetSizeCoefficient(0.37f)
                    ->Split
                    (
                        //FTabManager::NewStack()
                        FTabManager::NewSplitter()
                        ->SetOrientation(Orient_Horizontal)
                        ->SetSizeCoefficient(0.33f)
                        ->Split
                        (
                            FTabManager::NewStack()
                            ->SetSizeCoefficient(0.6f)
                            ->AddTab(ThemeGraphAppModeTabs::TabID_Details, ETabState::OpenedTab)
                        )
                        ->Split
                        (
                            FTabManager::NewStack()
                            ->SetSizeCoefficient(0.6f)
                            ->AddTab(ThemeGraphAppModeTabs::TabID_PreviewSettings, ETabState::OpenedTab)
                            ->AddTab(ThemeGraphAppModeTabs::TabID_Actions, ETabState::OpenedTab)
                            ->AddTab(ThemeGraphAppModeTabs::TabID_Markers, ETabState::OpenedTab)
                            ->SetForegroundTab(ThemeGraphAppModeTabs::TabID_Markers)
                        )
                    )
                    ->Split
                    (
                        FTabManager::NewStack()
                        ->SetSizeCoefficient(0.667f)
                        ->AddTab(ThemeGraphAppModeTabs::TabID_Preview, ETabState::OpenedTab)
                    )
                )
            )
        );

	
	// Listen for graph changed event
	OnGraphChangedDelegateHandle = GraphEditor->GetCurrentGraph()->AddOnGraphChangedHandler(
			FOnGraphChanged::FDelegate::CreateRaw(this, &FThemeGraphAppMode::OnGraphChanged));
	bGraphStateChanged = true;

	UEdGraph_DungeonProp* ThemeGraph = Cast<UEdGraph_DungeonProp>(GraphEditor->GetCurrentGraph());
	if (ThemeGraph) {
		OnNodePropertyChangedDelegateHandle = ThemeGraph->AddOnNodePropertyChangedHandler(
				FOnGraphChanged::FDelegate::CreateRaw(this, &FThemeGraphAppMode::OnNodePropertyChanged));
	}

	InitThemeGraph(ThemeGraph);

	// Show the dungeon properties
	if (PropertyEditor.IsValid()) {
		PropertyEditor->SetObject(AssetBeingEdited ? AssetBeingEdited->PreviewViewportProperties : nullptr, false);
	}

	ThemeEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	ThemeEditor->GetToolbarBuilder()->AddThemeGraphToolbar(ToolbarExtender);
}

FThemeGraphAppMode::~FThemeGraphAppMode() {
	if (GraphEditor->GetCurrentGraph()) {
		GraphEditor->GetCurrentGraph()->RemoveOnGraphChangedHandler(OnGraphChangedDelegateHandle);
		UEdGraph_DungeonProp* ThemeGraph = Cast<UEdGraph_DungeonProp>(GraphEditor->GetCurrentGraph());
		if (ThemeGraph) {
			ThemeGraph->RemoveOnNodePropertyChangedHandler(OnNodePropertyChangedDelegateHandle);
		}
	}
}

void FThemeGraphAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();

	ThemeEditor->RegisterToolbarTab(InTabManager.ToSharedRef());
	ThemeEditor->PushTabFactories(TabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FThemeGraphAppMode::Tick(float DeltaTime) {
	if (bGraphStateChanged) {
		bGraphStateChanged = false;
		HandleGraphChanged();
	}
}

UDungeonThemeAsset* FThemeGraphAppMode::GetThemeAsset() const {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
	return ThemeEditor.IsValid() ? ThemeEditor->GetAssetBeingEdited() : nullptr;
}

void FThemeGraphAppMode::RefreshMarkerListView() const {
    if (MarkerListView.IsValid()) {
        TArray<TSharedPtr<FMarkerListEntry>> MarkerEntries;
        GetMarkerNodes(GetThemeAsset(), MarkerEntries);
        MarkerListView->Refresh(MarkerEntries);
    }
}


void FThemeGraphAppMode::OnMarkerListDoubleClicked(TSharedPtr<FMarkerListEntry> Entry) const {
    if (Entry.IsValid()) {
        UEdGraphNode* Node = Entry->MarkerNode.Get();
        if (Node) {
            GraphEditor->JumpToNode(Node);
        }
    }
}

void FThemeGraphAppMode::OnNodeDoubleClicked(UEdGraphNode* InNode) const {
    // Focus of the nearest spawned actor from this theme node
    if (const UEdGraphNode_DungeonActorBase* ActorNode = Cast<UEdGraphNode_DungeonActorBase>(InNode)) {
    	const TSharedPtr<SThemePreviewViewport> PreviewViewport = PreviewViewportPtr.Pin();
    	if (PreviewViewport.IsValid()) {
    		const FName NodeId = FName(*ActorNode->NodeGuid.ToString());
    		PreviewViewport->ZoomOnNearestNodeMesh(NodeId);
    	}
    }
}

void FThemeGraphAppMode::SelectedNodesChanged(const TSet<UObject*>& SelectedNodes) const {
    UEdGraphNode_DungeonMarker* MarkerNode = nullptr;
    if (SelectedNodes.Num() == 1) {
        MarkerNode = Cast<UEdGraphNode_DungeonMarker>(SelectedNodes.Array()[0]);
    }
    
	const TSharedPtr<SThemePreviewViewport> PreviewViewport = PreviewViewportPtr.Pin();
	if (PreviewViewport.IsValid()) {
		if (MarkerNode) {
			PreviewViewport->VisualizeMarkers({ *MarkerNode->MarkerName });
		}
		else {
			PreviewViewport->ClearMarkerVisualizations();
		}
	}
}

void FThemeGraphAppMode::HandleAssetsDropped(const FDragDropEvent& InEvent, TArrayView<FAssetData> InAssets) const {
	if (GraphEditor.IsValid()) {
		if (UEdGraph_DungeonProp* ThemeGraph = Cast<UEdGraph_DungeonProp>(GraphEditor->GetCurrentGraph())) {
			FVector2D Offset = FVector2D::ZeroVector;
			for (const FAssetData& InAsset : InAssets) {
				if (UObject* AssetObject = InAsset.GetAsset()) {
					const FVector2D GridLocation = GetAssetDropGridLocation() + Offset;
					ThemeGraph->CreateNewNode(AssetObject, GridLocation);
					Offset += FVector2D(20, 20);
				}
			}
		}
	}
}

bool FThemeGraphAppMode::AreAssetsAcceptableForDrop(TArrayView<FAssetData> InAssets) const {
	if (GraphEditor.IsValid()) {
		if (const UEdGraph_DungeonProp* ThemeGraph = Cast<UEdGraph_DungeonProp>(GraphEditor->GetCurrentGraph())) {
			bool bAllAssetsValidForDrop = true;
			for (const FAssetData AssetData : InAssets) {
				if (const UObject* AssetObject = AssetData.GetAsset()) {
					bool bCanDrop = ThemeGraph->IsAssetAcceptableForDrop(AssetObject);

					if (!bCanDrop) {
						// Check if a broker can convert this asset to an actor
						const bool bHasActorFactory = FActorFactoryAssetProxy::GetFactoryForAsset(AssetData) != nullptr;

						if (bHasActorFactory) {
							bCanDrop = true;
						}
					}
					
					if (!bCanDrop) {
						bAllAssetsValidForDrop = false;
						break;
					}
				}
			}
			
			return bAllAssetsValidForDrop;
		}
	}
	return false;
}

FVector2D FThemeGraphAppMode::GetAssetDropGridLocation() const {
	if (!AssetDropTarget.IsValid()) return FVector2D::ZeroVector;

	FVector2D PanelCoord = AssetDropTarget->GetPanelCoordDropPosition();
	FVector2D ViewLocation = FVector2D::ZeroVector;
	float ZoomAmount = 1.0f;
	GraphEditor->GetViewLocation(ViewLocation, ZoomAmount);
	FVector2D GridLocation = PanelCoord / ZoomAmount + ViewLocation;

	return GridLocation;
}

void FThemeGraphAppMode::CreateGraphEditorWidget(UEdGraph* InGraph) {
	// Create the appearance info
	FGraphAppearanceInfo AppearanceInfo;
	AppearanceInfo.CornerText = LOCTEXT("AppearanceCornerText", "Dungeon Architect");

	GraphEditor = SNew(SGraphEditor)
		.AdditionalCommands(ThemeGraphHandler->GetCommands())
		.Appearance(AppearanceInfo)
		.GraphToEdit(InGraph)
		.GraphEvents(ThemeGraphHandler->GraphEvents);

	AssetDropTarget = SNew(SThemeEditorDropTarget)
		.OnAssetsDropped(this, &FThemeGraphAppMode::HandleAssetsDropped)
		.OnAreAssetsAcceptableForDrop(this, &FThemeGraphAppMode::AreAssetsAcceptableForDrop)
		.Visibility(EVisibility::HitTestInvisible);

	GraphEditorHost = SNew(SOverlay)
		+ SOverlay::Slot()
		[
			GraphEditor.ToSharedRef()
		]
		+ SOverlay::Slot()
		[
			AssetDropTarget.ToSharedRef()
		];
}

void FThemeGraphAppMode::OnGraphChanged(const FEdGraphEditAction& Action) {
	bGraphStateChanged = true;

	if (ContainsNodesOfType<UEdGraphNode_DungeonMarker>(Action.Nodes)) {
		RefreshMarkerListView();
	}
}

void FThemeGraphAppMode::OnNodePropertyChanged(const FEdGraphEditAction& Action) {
	// The nodes that were modified and requires a clean rebuild by the scene provider
	TSet<FName> NodeObjectsToRebuild;

	// Flag the node id to generate it cleanly in the scene provider
	for (const UEdGraphNode* Node : Action.Nodes) {
		const UEdGraphNode_DungeonActorBase* ActorNode = Cast<const UEdGraphNode_DungeonActorBase>(Node);
		if (ActorNode) {
			FName NodeId(*ActorNode->NodeGuid.ToString());
			NodeObjectsToRebuild.Add(NodeId);
		}
	}

	const TSharedPtr<SThemePreviewViewport> PreviewViewport = PreviewViewportPtr.Pin();
	if (PreviewViewport.IsValid()) {
		PreviewViewport->SetNodesToRebuild(NodeObjectsToRebuild);
	}

	if (ContainsNodesOfType<UEdGraphNode_DungeonMarker>(Action.Nodes)) {
		RefreshMarkerListView();
	}

	bGraphStateChanged = true;
}

void FThemeGraphAppMode::HandleGraphChanged() const {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
	if (ThemeEditor.IsValid()) {
		ThemeEditor->CompileThemeGraph();
	}
	
	const TSharedPtr<SThemePreviewViewport> PreviewViewport = PreviewViewportPtr.Pin();
	if (PreviewViewport.IsValid()) {
		PreviewViewport->RebuildMeshes();
	}
	
	if (ActionPalette.IsValid()) {
		ActionPalette->Refresh();
	}
}

void FThemeGraphAppMode::InitThemeGraph(UEdGraph_DungeonProp* ThemeGraph) const {
	if (!ThemeGraph) return;

	for (UEdGraphNode* Node : ThemeGraph->Nodes) {
		UEdGraphNode_DungeonBase* DungeonNode = Cast<UEdGraphNode_DungeonBase>(Node);
		if (DungeonNode) {
			// Make sure the actor node is initialized
			DungeonNode->OnThemeEditorLoaded();
		}
	}
}

void FThemeGraphAppMode::OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged) {
	if (NodeBeingChanged)
	{
		const FScopedTransaction Transaction( LOCTEXT( "RenameNode", "Rename Node" ) );
		NodeBeingChanged->Modify();
		NodeBeingChanged->OnRenameNode(NewText.ToString());
	}
}

void FThemeGraphAppMode::ShowObjectDetails(UObject* ObjectProperties, bool bForceRefresh) const {
	if (PropertyEditor.IsValid()) {
		if (!ObjectProperties) {
			// No object selected.  Show the dungeon properties by default
			UDungeonThemeAsset* ThemeAsset = GetThemeAsset();
			ObjectProperties = ThemeAsset ? ThemeAsset->PreviewViewportProperties : nullptr;
		}
		
		PropertyEditor->SetObject(ObjectProperties, bForceRefresh);
	}
}

void FThemeGraphAppMode::RecreateDefaultMarkerNodes() const {
	UDungeonThemeAsset* ThemeAsset = GetThemeAsset();
	if (ThemeAsset && ThemeAsset->UpdateGraph && ThemeAsset->PreviewViewportProperties) {
		if (UEdGraph_DungeonProp* DungeonGraph = Cast<UEdGraph_DungeonProp>(ThemeAsset->UpdateGraph)) {
			const TSubclassOf<UDungeonBuilder> BuilderClass = ThemeAsset->PreviewViewportProperties->BuilderClass;
			if (BuilderClass) {
				DungeonGraph->RecreateDefaultMarkerNodes(BuilderClass);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE

