//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppMode.h"

#include "Core/Editors/ThemeEditor/AppModes/Common/ThemeEditorAppTabFactoryMacros.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/EditorImpl/AppModeEditorImplFactory.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/EditorImpl/AppModeEditorInterface.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/MarkerGeneratorAppModeCommands.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternEditor/PatternEditorWidget.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Editor/PatternGraphHandler.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraph.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphCompiler.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNode.h"
#include "Core/Editors/ThemeEditor/DungeonArchitectThemeEditor.h"
#include "Core/Editors/ThemeEditor/Widgets/SThemePreviewViewport.h"
#include "Frameworks/MarkerGenerator/MarkerGenModel.h"
#include "Frameworks/MarkerGenerator/MarkerGenPattern.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"

#define LOCTEXT_NAMESPACE "FMarkerGeneratorAppMode"

DEFINE_LOG_CATEGORY_STATIC(LogMarkerGen, Log, All);

namespace MarkerGeneratorAppModeTabs {
	static const FName TabID_Layers(TEXT("Layers"));
	static const FName TabID_Pattern(TEXT("Pattern"));
	static const FName TabID_PatternRuleGraph(TEXT("PatternRuleGraph"));
	static const FName TabID_Preview(TEXT("Preview"));
	static const FName TabID_Preview2D(TEXT("Preview2D"));
	static const FName TabID_Details(TEXT("Details"));
	
	DEFINE_THEME_EDITOR_TAB_FACTORY(Layers,
				LOCTEXT("Caption_Layers", "Layers"),
				"LevelEditor.Tabs.Layers",
				LOCTEXT("TooltipMenu_Layers", "Generate markers in the scene through different layered passes"),
				LOCTEXT("TooltipTab_Layers", "Generate markers in the scene through different layered passes"))

	DEFINE_THEME_EDITOR_TAB_FACTORY(Pattern,
				LOCTEXT("Caption_Pattern", "Pattern"),
				"LevelEditor.Tabs.Viewports",
				LOCTEXT("TooltipMenu_Pattern", "Define the patter for your marker generation / deletion rules"),
				LOCTEXT("TooltipTab_Pattern", "Define the patter for your marker generation / deletion rules"))
	
	DEFINE_THEME_EDITOR_TAB_FACTORY(PatternRuleGraph,
				LOCTEXT("Caption_PatternNodeRule", "Pattern Node Rule"),
				"LevelEditor.Tabs.Viewports",
				LOCTEXT("TooltipMenu_PatternRuleGraph", "Define the pattern matching rule for a rule node"),
				LOCTEXT("TooltipTab_PatternRuleGraph", "Define the pattern matching rule for a rule node"))
	
	DEFINE_THEME_EDITOR_TAB_FACTORY(Details,
				LOCTEXT("Caption_Details", "Details"),
				"LevelEditor.Tabs.Details",
				LOCTEXT("TooltipMenu_Details", "Details panel for modifying the node properties"),
				LOCTEXT("TooltipTab_Details", "Details panel for modifying the node properties"))

	
	DEFINE_THEME_EDITOR_TAB_FACTORY_CUSTOM_SPAWNER(Preview,
				LOCTEXT("Caption_PreviewViewport", "Preview"),
				"LevelEditor.Tabs.Viewports",
				LOCTEXT("TooltipMenu_Preview", "Preview the generated dungeon in a 3D viewport"),
				LOCTEXT("TooltipTab_Preivew", "Preview the generated dungeon in a 3D viewport"))

	
	DEFINE_THEME_EDITOR_TAB_FACTORY(Preview2D,
				LOCTEXT("Caption_Preview2DViewport", "Preview2D"),
				"LevelEditor.Tabs.Viewports",
				LOCTEXT("TooltipMenu_Preview2D", "Preview the generated dungeon in a 2D viewport"),
				LOCTEXT("TooltipTab_Preivew2D", "Preview the generated dungeon in a 2D viewport"))

	
	TSharedRef<SDockTab> FThemeEditorTabFactory_Preview::SpawnTab(const FWorkflowTabSpawnInfo& Info) const {
		TSharedRef<SDockTab> DockTab = FWorkflowTabFactory::SpawnTab(Info);
		const TSharedPtr<SThemePreviewViewport> PreviewViewport = StaticCastSharedPtr<SThemePreviewViewport>(WidgetPtr.Pin());
		if (PreviewViewport.IsValid()) {
			PreviewViewport->SetParentTab(FDungeonArchitectThemeEditor::AppModeID_MarkerGenerator, DockTab);
		}
		return DockTab;;
	}
	
	
}

FMarkerGeneratorAppMode::FMarkerGeneratorAppMode(TSharedPtr<FDungeonArchitectThemeEditor> InThemeEditor, TSharedPtr<SThemePreviewViewport> InPreviewViewport)
	: FThemeEditorAppModeBase(InThemeEditor, FDungeonArchitectThemeEditor::AppModeID_MarkerGenerator)
	, PreviewViewportPtr(InPreviewViewport)
{
}

void FMarkerGeneratorAppMode::Init(TSubclassOf<UDungeonBuilder> InBuilderClass) {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
	if (!ThemeEditor.IsValid()) return;
	
	const TSharedPtr<SThemePreviewViewport> PreviewViewport = PreviewViewportPtr.Pin();
	check(PreviewViewport.IsValid());

	BindCommands(ThemeEditor->GetToolkitCommands());

	PatternEditorHost = SNew(SBox);
	PatternEditorInvalidMessage = SNew(SBorder)
		.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
		.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f))
		.VAlign(VAlign_Center)
		.HAlign(HAlign_Center)
		[
			SNew(STextBlock)
			.Text_Raw(this, &FMarkerGeneratorAppMode::GetPatternEditorInvalidMessage)
			.Justification(ETextJustify::Center)
		];
	
	PreviewViewport2D = SNew(SMGPreview2DViewport);
	PropertyEditor = CreatePropertyEditorWidget();
	PatternRuleGraphEditorHost = SNew(SBox);
	
	SetBuilderClass(InBuilderClass);
	UpdatePatternRuleGraphEditor();
	
	LayerList = SNew(SEditableListView<UMarkerGenLayer*>)
		.GetListSource(this, &FMarkerGeneratorAppMode::GetLayerList)
		.OnSelectionChanged(this, &FMarkerGeneratorAppMode::OnLayerSelectionChanged)
		.OnAddItem(this, &FMarkerGeneratorAppMode::OnLayerAdd)
		.OnDeleteItem(this, &FMarkerGeneratorAppMode::OnLayerDelete)
		.OnReorderItem(this, &FMarkerGeneratorAppMode::OnLayerReordered)
		.GetItemText(this, &FMarkerGeneratorAppMode::GetLayerListRowText)
		.CreateItemWidget(this, &FMarkerGeneratorAppMode::CreateLayerListItemWidget)
		.IconBrush(FDungeonArchitectStyle::Get().GetBrush("DA.SnapEd.GraphIcon"))
		.AllowDropOnGraph(true);
	
	TabFactories.RegisterFactory(MakeShareable(new MarkerGeneratorAppModeTabs::FThemeEditorTabFactory_Layers(ThemeEditor, LayerList)));
	TabFactories.RegisterFactory(MakeShareable(new MarkerGeneratorAppModeTabs::FThemeEditorTabFactory_Pattern(ThemeEditor, PatternEditorHost)));
	TabFactories.RegisterFactory(MakeShareable(new MarkerGeneratorAppModeTabs::FThemeEditorTabFactory_PatternRuleGraph(ThemeEditor, PatternRuleGraphEditorHost)));
	TabFactories.RegisterFactory(MakeShareable(new MarkerGeneratorAppModeTabs::FThemeEditorTabFactory_Details(ThemeEditor, PropertyEditor)));
	TabFactories.RegisterFactory(MakeShareable(new MarkerGeneratorAppModeTabs::FThemeEditorTabFactory_Preview(ThemeEditor, PreviewViewport)));
	//TabFactories.RegisterFactory(MakeShareable(new MarkerGeneratorAppModeTabs::FThemeEditorTabFactory_Preview2D(ThemeEditor, PreviewViewport2D)));

	TabLayout = FTabManager::NewLayout(
			"MarkerGeneratorAppMode_Layout_v0.0.9")
		->AddArea
		(
			FTabManager::NewPrimaryArea()
			->SetOrientation(Orient_Vertical)
			->Split
			(
				FTabManager::NewSplitter()
				->SetOrientation(Orient_Horizontal)
				->Split
				(
					FTabManager::NewSplitter()
					->SetOrientation(Orient_Vertical)
					->SetSizeCoefficient(0.2f)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.35f)
						->AddTab(MarkerGeneratorAppModeTabs::TabID_Layers, ETabState::OpenedTab)
					)
					->Split
					(
						FTabManager::NewStack()
						->SetSizeCoefficient(0.65f)
						->AddTab(MarkerGeneratorAppModeTabs::TabID_Details, ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewSplitter()
                	->SetOrientation(Orient_Vertical)
                	->SetSizeCoefficient(0.5f)
                	->Split
                	(
                		FTabManager::NewStack()
                		->SetSizeCoefficient(0.55f)
                		->AddTab(MarkerGeneratorAppModeTabs::TabID_Pattern, ETabState::OpenedTab)
                	)
                	->Split
                	(
                		FTabManager::NewStack()
                		->SetSizeCoefficient(0.45f)
                		->AddTab(MarkerGeneratorAppModeTabs::TabID_PatternRuleGraph, ETabState::OpenedTab)
                	)
					
				)
				->Split
				(
					FTabManager::NewStack()
                	->SetSizeCoefficient(0.4f)
					->AddTab(MarkerGeneratorAppModeTabs::TabID_Preview, ETabState::OpenedTab)
				)
			)
		);

	ThemeEditor->GetToolbarBuilder()->AddModesToolbar(ToolbarExtender);
	ThemeEditor->GetToolbarBuilder()->AddMarkerGeneratorToolbar(ToolbarExtender);
}

void FMarkerGeneratorAppMode::SetBuilderClass(TSubclassOf<UDungeonBuilder> InBuilderClass) {
	AppModeImpl = FMGAppModeEditorImplFactory::Create(InBuilderClass);

	if (PatternEditor.IsValid()) {
		PatternEditor->DeactivateEdMode();
	}
	
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
	if (AppModeImpl.IsValid() && ThemeEditor.IsValid()) {
		const TSharedPtr<IMGPatternEditor> PatternEditorImpl = AppModeImpl->CreatePatternEditorImpl();
		PatternEditorImpl->GetOnRuleSelectionChanged().BindRaw(this, &FMarkerGeneratorAppMode::SetActivePatternRuleGraph);
		PatternEditorImpl->GetOnRulePositionChanged().BindRaw(this, &FMarkerGeneratorAppMode::HandleRulePositionChanged);
		PatternEditor = SNew(SMGPatternEditorWidget, ThemeEditor);
		PatternEditor->SetEditorImpl(PatternEditorImpl);
		PatternEditor->ActivateEdMode(ThemeEditor->GetToolkitHost());
		
		PatternEditorHost->SetContent(PatternEditor.ToSharedRef());
	}
	else {
		PatternEditor.Reset();
		PatternEditorHost->SetContent(PatternEditorInvalidMessage.ToSharedRef());
	}
}

FText FMarkerGeneratorAppMode::GetPatternEditorInvalidMessage() const {
	if (!AppModeImpl.IsValid()) {
		return LOCTEXT("PatternEditorUnsupportedClassLabel", "Builder is not supported.  Please choose another builder (e.g. Grid, GridFlow etc)");
	}
	else if (ActiveLayer.IsValid() && !AppModeImpl->IsLayerCompatible(ActiveLayer.Get())) {
		return LOCTEXT("PatternEditorUnsupportedLayerLabel", "Layer is incompatible with this builder, please select or create another layer");
	}
	else {
		return LOCTEXT("PatternEditorInvalidLayerLabel", "Please select a layer to start editing");
	}
}

void FMarkerGeneratorAppMode::RebuildPreviewScene() const {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
	if (ThemeEditor.IsValid()) {
		ThemeEditor->CompileThemeGraph();
	}
	
	const TSharedPtr<SThemePreviewViewport> PreviewViewport = PreviewViewportPtr.Pin();
	if (PreviewViewport.IsValid()) {
		PreviewViewport->RebuildMeshes();
	}
}

void FMarkerGeneratorAppMode::RegisterTabFactories(TSharedPtr<FTabManager> InTabManager) {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();

	ThemeEditor->RegisterToolbarTab(InTabManager.ToSharedRef());
	ThemeEditor->PushTabFactories(TabFactories);

	FApplicationMode::RegisterTabFactories(InTabManager);
}

void FMarkerGeneratorAppMode::PreDeactivateMode() {
}

void FMarkerGeneratorAppMode::PostActivateMode() {
	SetActiveLayer(ActiveLayer.Get());
}

void FMarkerGeneratorAppMode::BindCommands(TSharedRef<FUICommandList> ToolkitCommands) {
    const FMarkerGeneratorAppModeCommands& Commands = FMarkerGeneratorAppModeCommands::Get();

	ToolkitCommands->MapAction(
		Commands.Build,
		FExecuteAction::CreateSP(this, &FMarkerGeneratorAppMode::Compile));

}

void FMarkerGeneratorAppMode::UpdatePatternRuleGraphEditor() {
	// Cleanup the old graph editor
	if (PatternRuleGraphEditor.IsValid() && PatternRuleGraphEditor->GetCurrentGraph()) {
		PatternRuleGraphEditor->GetCurrentGraph()->RemoveOnGraphChangedHandler(OnGraphChangedHandle);
	}
	
	UEdGraph* PatternRuleGraph = SelectedPatternRule.IsValid() ? SelectedPatternRule->RuleEdGraph : nullptr;
	if (PatternRuleGraph) {
		OnGraphChangedHandle = PatternRuleGraph->AddOnGraphChangedHandler(
					FOnGraphChanged::FDelegate::CreateRaw(this, &FMarkerGeneratorAppMode::OnRuleGraphChanged));

		OnGraphPropertyChangedHandle = PatternRuleGraph->AddPropertyChangedNotifier(
			FOnPropertyChanged::FDelegate::CreateRaw(this, &FMarkerGeneratorAppMode::OnRuleGraphPropertyChanged));
    

		// Create the appearance info
		FGraphAppearanceInfo AppearanceInfo;
		AppearanceInfo.CornerText = LOCTEXT("PatternRuleGraphBranding", "Pattern Rule");
		PatternRuleGraphHandler = MakeShareable(new FMGPatternGraphHandler);
		PatternRuleGraphHandler->Bind();
		PatternRuleGraphHandler->SetPropertyEditor(PropertyEditor);
		//PatternRuleGraphHandler->OnNodeSelectionChanged.BindRaw(this, &FMarkerGeneratorAppMode::OnExecNodeSelectionChanged);
		//PatternRuleGraphHandler->OnNodeDoubleClicked.BindRaw(this, &FMarkerGeneratorAppMode::OnExecNodeDoubleClicked);

		PatternRuleGraphEditor = SNew(SGraphEditor)
			.AdditionalCommands(PatternRuleGraphHandler->GraphEditorCommands)
			.Appearance(AppearanceInfo)
			.GraphToEdit(PatternRuleGraph)
			.IsEditable(true)
			.ShowGraphStateOverlay(false)
			.GraphEvents(PatternRuleGraphHandler->GraphEvents);

		PatternRuleGraphHandler->SetGraphEditor(PatternRuleGraphEditor);
		PatternRuleGraphEditorHost->SetContent(PatternRuleGraphEditor.ToSharedRef());
	}
	else {
		PatternRuleGraphEditor.Reset();
		PatternRuleGraphHandler.Reset();
		PatternRuleGraphEditorHost->SetContent(
			SNew(SBorder)
			.BorderImage(FEditorStyle::GetBrush("ToolPanel.DarkGroupBorder"))
			.BorderBackgroundColor(FLinearColor(0.5f, 0.5f, 0.5f))
			.VAlign(VAlign_Center)
			.HAlign(HAlign_Center)
			[
				SNew(STextBlock)
				.Text(LOCTEXT("PatternRuleGraphEmptyText", "Select a pattern rule block to edit"))
				.Justification(ETextJustify::Center)
			]);
			
	}
}

void FMarkerGeneratorAppMode::NotifyPostChange(const FPropertyChangedEvent& PropertyChangedEvent, FProperty* PropertyThatChanged) {
	const FName PropertyName = PropertyThatChanged ? PropertyThatChanged->GetFName() : NAME_None; 
	for (int32 Idx = 0; Idx < PropertyChangedEvent.GetNumObjectsBeingEdited(); Idx++) {
		const UObject* Obj = PropertyChangedEvent.GetObjectBeingEdited(Idx);
		if (!Obj) continue;
		if (Obj->IsA<UMarkerGenPatternRule>()) {
			if (PropertyName == GET_MEMBER_NAME_CHECKED(UMarkerGenPatternRule, bVisuallyDominant) ||
					PropertyName == GET_MEMBER_NAME_CHECKED(UMarkerGenPatternRule, Color)) {
				// Notify the viewport to update the visuals of the rule object
				if (PatternEditor.IsValid() && PatternEditor->GetEditorImpl().IsValid()) {
					const TSharedPtr<IMGPatternEditor> PatternEditorImpl = PatternEditor->GetEditorImpl();
					PatternEditorImpl->InvalidateRuleVisuals(Cast<UMarkerGenPatternRule>(Obj));
				}
			}
		}
		else if (Obj->IsA<UMGPatternScriptNode>() && SelectedPatternRule.IsValid()) {
			// Reconstruct the node if the property has been changed
			if (SelectedPatternRule->RuleEdGraph && PatternRuleGraphEditor.IsValid()) {
				OnRuleGraphUpdated();

				const bool bNeedsReconstruction =
					(Obj->IsA<UMGPatternScriptNode_MarkerExists>() && PropertyName == GET_MEMBER_NAME_CHECKED(UMGPatternScriptNode_MarkerExists, MarkerName)) ||
					(Obj->IsA<UMGPatternScriptNode_EmitMarker>() && PropertyName == GET_MEMBER_NAME_CHECKED(UMGPatternScriptNode_EmitMarker, MarkerName)) ||
					(Obj->IsA<UMGPatternScriptNode_RemoveMarker>() && PropertyName == GET_MEMBER_NAME_CHECKED(UMGPatternScriptNode_RemoveMarker, MarkerName));

				if (bNeedsReconstruction) {
					for (UEdGraphNode* EdNode : SelectedPatternRule->RuleEdGraph->Nodes) {
						if (UMGPatternGraphNode* RuleEdNode = Cast<UMGPatternGraphNode>(EdNode)) {
							if (RuleEdNode->NodeTemplate == Obj) {
								PatternRuleGraphEditor->RefreshNode(*RuleEdNode);
							}
						}
					}
				}
			}
		}
		else if (Obj->IsA<UMarkerGenLayer>()) {
			if (PropertyName != GET_MEMBER_NAME_CHECKED(UMarkerGenLayer, LayerName)) {
				RebuildPreviewScene();
			}
		}
	}
}

void FMarkerGeneratorAppMode::OnLayerSelectionChanged(UMarkerGenLayer* Item, ESelectInfo::Type SelectInfo) {
	SetActiveLayer(Item);
}

TSharedPtr<SWidget> FMarkerGeneratorAppMode::CreateLayerListItemWidget(UMarkerGenLayer* InLayer) {
	return SNew(SVerticalBox)
		+SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(this, &FMarkerGeneratorAppMode::GetLayerListRowText, InLayer)
			.ColorAndOpacity(this, &FMarkerGeneratorAppMode::GetLayerListRowColor, InLayer)
			.Font(FDungeonArchitectStyle::Get().GetFontStyle("DungeonArchitect.ListView.LargeFont"))
		]
		+SVerticalBox::Slot()
		[
			SNew(STextBlock)
			.Text(this, &FMarkerGeneratorAppMode::GetLayerListRowFeatureText, InLayer)
			.Visibility(this, &FMarkerGeneratorAppMode::IsLayerListRowFeatureVisible, InLayer)
			.ColorAndOpacity(FLinearColor::Gray)
			.Font(FDungeonArchitectStyle::Get().GetFontStyle("DungeonArchitect.ListView.Font"))
		];
	
}

FText FMarkerGeneratorAppMode::GetLayerListRowText(UMarkerGenLayer* InItem) const {
	static const FText UnknownText = LOCTEXT("MissingLayerName", "Unknown");
	return InItem ? InItem->LayerName : UnknownText;
}

FText FMarkerGeneratorAppMode::GetLayerListRowFeatureText(UMarkerGenLayer* InItem) const {
	static const FText IncompatibleText = LOCTEXT("IncompatibleLayerName", "Incompatible Layer");
	if (!AppModeImpl.IsValid() || !AppModeImpl->IsLayerCompatible(InItem)) {
		return IncompatibleText;
	}
	return FText::GetEmpty();
}

const TArray<UMarkerGenLayer*>* FMarkerGeneratorAppMode::GetLayerList() const {
	UMarkerGenModel* MarkerGenerationModel = GetMarkerGenerationModel();
	return MarkerGenerationModel ? &MarkerGenerationModel->Layers : nullptr;
}

void FMarkerGeneratorAppMode::OnLayerAdd() const {
	static const FText DefaultLayerName = LOCTEXT("DefaultLayerName", "Layer");
	
	if (UMarkerGenModel* Model = GetMarkerGenerationModel()) {
		UMarkerGenLayer* NewLayer = CreateNewLayer(Model, DefaultLayerName);
		if (NewLayer) {
			Model->Layers.Add(NewLayer);
			Model->Modify();
			LayerList->RefreshListView();
			LayerList->SetItemSelection(NewLayer);
		}
	}
}

UMarkerGenLayer* FMarkerGeneratorAppMode::CreateNewLayer(UObject* InOuter, const FText& InLayerName) const {
	if (!AppModeImpl.IsValid()) {
		return nullptr;
	}
	
	UMarkerGenLayer* NewLayer = AppModeImpl->CreateNewLayer(InOuter);
	NewLayer->LayerName = InLayerName;
	return NewLayer;
}

void FMarkerGeneratorAppMode::OnLayerDelete(UMarkerGenLayer* InItem) {
	if (!InItem) return;
	
	static const FText Title = LOCTEXT("DADeleteLayerTitle", "Delete Layer?");
	const EAppReturnType::Type ReturnValue = FMessageDialog::Open(EAppMsgType::YesNo, EAppReturnType::No,
															LOCTEXT("DADeleteLayer","Are you sure you want to delete the layer?"),
															&Title);

	if (ReturnValue == EAppReturnType::Yes) {
		// Delete the layer
		if (UMarkerGenModel* MarkerGenerationModel = GetMarkerGenerationModel()) {
			MarkerGenerationModel->Layers.Remove(InItem);
			MarkerGenerationModel->Modify();

			SetActiveLayer(nullptr);
		}
	}
}

void FMarkerGeneratorAppMode::OnLayerReordered(UMarkerGenLayer* Source, UMarkerGenLayer* Dest) const {
	if (UMarkerGenModel* Model = GetMarkerGenerationModel()) {
		int32 DestIndex = -1;
		if (!Model->Layers.Find(Dest, DestIndex)) {
			DestIndex = 0;
		}
		Model->Layers.Remove(Source);
		Model->Layers.Insert(Source, DestIndex);	
	}
}

EVisibility FMarkerGeneratorAppMode::IsLayerListRowFeatureVisible(UMarkerGenLayer* InLayer) const {
	if (!AppModeImpl.IsValid() || !AppModeImpl->IsLayerCompatible(InLayer)) {
		return EVisibility::Visible;
	}
	return EVisibility::Collapsed;
}

FSlateColor FMarkerGeneratorAppMode::GetLayerListRowColor(UMarkerGenLayer* InLayer) const {
	if (!AppModeImpl.IsValid() || !AppModeImpl->IsLayerCompatible(InLayer)) {
		return FLinearColor::Gray;
	}
	return FLinearColor::White;
}

UMarkerGenModel* FMarkerGeneratorAppMode::GetMarkerGenerationModel() const {
	const TSharedPtr<FDungeonArchitectThemeEditor> ThemeEditor = ThemeEditorPtr.Pin();
	if (ThemeEditor.IsValid()) {
		if (const UDungeonThemeAsset* ThemeAsset = ThemeEditor->GetAssetBeingEdited()) {
			return ThemeAsset->MarkerGenerationModel;
		}
	}
	return nullptr;
}

void FMarkerGeneratorAppMode::SetActiveLayer(UMarkerGenLayer* InLayer) {
	ActiveLayer = InLayer;
	bool bLayerValid = (InLayer != nullptr);
	if (!AppModeImpl.IsValid() || !AppModeImpl->IsLayerCompatible(InLayer)) {
		bLayerValid = false;
	}

	if (bLayerValid) {
		if (PatternEditor.IsValid()) {
			UMarkerGenPattern* Pattern = InLayer ? InLayer->Pattern : nullptr; 
			PatternEditor->Load(Pattern);
			
			PatternEditorHost->SetContent(PatternEditor.ToSharedRef());
		}

		// Reset the graph editor
		SetActivePatternRuleGraph(nullptr);
	
		if (PropertyEditor.IsValid()) {
			PropertyEditor->SetObject(InLayer);
		}
	}
	else {
		PatternEditorHost->SetContent(PatternEditorInvalidMessage.ToSharedRef());
		SetActivePatternRuleGraph(nullptr);
		PropertyEditor->SetObject(nullptr);
	}
}


void FMarkerGeneratorAppMode::SetActivePatternRuleGraph(UMarkerGenPatternRule* InPatternRule) {
	if (SelectedPatternRule != InPatternRule) {
		SelectedPatternRule = InPatternRule;
		UpdatePatternRuleGraphEditor();
	}
	PropertyEditor->SetObject(InPatternRule);
}

void FMarkerGeneratorAppMode::HandleRulePositionChanged(UMarkerGenPatternRule* InPatternRule) const {
	RebuildPreviewScene();
}

void FMarkerGeneratorAppMode::OnRuleGraphChanged(const FEdGraphEditAction& InAction) const {
	OnRuleGraphUpdated();
}

void FMarkerGeneratorAppMode::OnRuleGraphPropertyChanged(const FPropertyChangedEvent& InEvent, const FString& InPropertyName) const {
	OnRuleGraphUpdated();
}

void FMarkerGeneratorAppMode::OnRuleGraphUpdated() const {
	// Compile the rule graph script
	if (SelectedPatternRule.IsValid()) {
		FMGPatternGraphCompiler::Compile(SelectedPatternRule.Get());
	}

	// Notify the pattern editor viewport that the rule has been modified
	if (PatternEditor.IsValid()) {
		PatternEditor->OnPatternRuleModified(SelectedPatternRule.Get());
	}

	RebuildPreviewScene();
}

void FMarkerGeneratorAppMode::Compile() const {
	UMarkerGenModel* MarkerGenerationModel = GetMarkerGenerationModel();
	if (!MarkerGenerationModel) return;

	for (const UMarkerGenLayer* Layer : MarkerGenerationModel->Layers) {
		FMGPatternGraphCompiler::Compile(Layer->Pattern);
	}
	MarkerGenerationModel->Modify();
}

#undef LOCTEXT_NAMESPACE

