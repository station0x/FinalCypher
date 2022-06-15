//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/AppModes/ThemeEditorAppModeBase.h"

#include "WorkflowOrientedApp/WorkflowTabManager.h"

class SThemeEditorDropTarget;
class SThemePreviewViewport;
class SGraphPalette_PropActions;
class SMarkerListView;
struct FMarkerListEntry;
class SAdvancedPreviewDetailsTab;
class UDungeonThemeAsset;

typedef TSharedPtr<class FDungeonArchitectThemeGraphHandler> FDungeonArchitectThemeEditorActionsPtr;

class FThemeGraphAppMode : public FThemeEditorAppModeBase {
public:
	FThemeGraphAppMode(TSharedPtr<FDungeonArchitectThemeEditor> InThemeEditor, TSharedPtr<SThemePreviewViewport> InPreviewViewport);
	virtual ~FThemeGraphAppMode() override;
	virtual void Init(TSubclassOf<UDungeonBuilder> InBuilderClass) override;
	virtual void RegisterTabFactories(TSharedPtr<class FTabManager> InTabManager) override;
	virtual void Tick(float DeltaTime) override;
	virtual void ShowObjectDetails(UObject* ObjectProperties, bool bForceRefresh = false) const override;

	TSharedPtr<SGraphEditor> GetGraphEditor() const { return GraphEditor; }
	void RecreateDefaultMarkerNodes() const;
	
private:
	void HandleAssetsDropped(const FDragDropEvent& InEvent, TArrayView<FAssetData> InAssets) const;
	bool AreAssetsAcceptableForDrop(TArrayView<FAssetData> InAssets) const;
	void RefreshMarkerListView() const;
	void OnMarkerListDoubleClicked(TSharedPtr<FMarkerListEntry> Entry) const;
	void OnNodeDoubleClicked(class UEdGraphNode* Node) const;
	void SelectedNodesChanged(const TSet<UObject*>& SelectedNodes) const;
	void OnGraphChanged(const FEdGraphEditAction& Action);
	void OnNodePropertyChanged(const FEdGraphEditAction& Action);
	void HandleGraphChanged() const;
	void InitThemeGraph(class UEdGraph_DungeonProp* ThemeGraph) const;
	void CreateGraphEditorWidget(UEdGraph* InGraph);
	void OnNodeTitleCommitted(const FText& NewText, ETextCommit::Type CommitInfo, UEdGraphNode* NodeBeingChanged);
	FVector2D GetAssetDropGridLocation() const;
	
	UDungeonThemeAsset* GetThemeAsset() const;
	
private:
	FWorkflowAllowedTabSet TabFactories;
	
	TSharedPtr<SGraphEditor> GraphEditor;
	TSharedPtr<SWidget> GraphEditorHost;
	TSharedPtr<IDetailsView> PropertyEditor;
	TSharedPtr<SThemeEditorDropTarget> AssetDropTarget;
	TSharedPtr<SGraphPalette_PropActions> ActionPalette;
	TSharedPtr<SMarkerListView> MarkerListView;
	TSharedPtr<SWidget> PreviewSettingsWidget;

	FDelegateHandle OnGraphChangedDelegateHandle;
	FDelegateHandle OnNodePropertyChangedDelegateHandle;
	FDungeonArchitectThemeEditorActionsPtr ThemeGraphHandler;
	bool bGraphStateChanged = false;

	TWeakPtr<SThemePreviewViewport> PreviewViewportPtr;
};



