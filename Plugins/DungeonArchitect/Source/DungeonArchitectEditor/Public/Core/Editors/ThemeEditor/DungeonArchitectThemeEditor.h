//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "CoreGlobals.h"
#include "Tickable.h"
#include "WorkflowOrientedApp/WorkflowCentricApplication.h"

class SThemeEditorDropTarget;
class UEdGraphNode_DungeonMarker;
class FThemeEditorAppModeBase;
class UDungeonThemeAsset;
class SThemePreviewViewport;

class FDungeonEditorThumbnailPool : public FAssetThumbnailPool {
public:
    FDungeonEditorThumbnailPool(int NumObjectsInPool) : FAssetThumbnailPool(NumObjectsInPool) {
    }

    static TSharedPtr<FDungeonEditorThumbnailPool> Get() { return Instance; }

    static void Create() {
        if (!IsRunningCommandlet()) {
            Instance = MakeShareable(new FDungeonEditorThumbnailPool(512));
        }
    }

private:
    static TSharedPtr<FDungeonEditorThumbnailPool> Instance;
};

class FDungeonArchitectThemeEditor;

class FThemeEditorToolbar : public TSharedFromThis<FThemeEditorToolbar> {
public:
    FThemeEditorToolbar(TWeakPtr<FDungeonArchitectThemeEditor> InThemeEditor) : ThemeEditorPtr(InThemeEditor) {}
    void AddModesToolbar(TSharedPtr<FExtender> Extender) const;
    void AddThemeGraphToolbar(TSharedPtr<FExtender> Extender);
    void AddMarkerGeneratorToolbar(TSharedPtr<FExtender> Extender);

private:
    void FillModesToolbar(FToolBarBuilder& ToolbarBuilder) const;
    void FillThemeGraphToolbar(FToolBarBuilder& ToolbarBuilder);
    void FillMarkerGeneratorToolbar(FToolBarBuilder& ToolbarBuilder);
    
private:
    TWeakPtr<FDungeonArchitectThemeEditor> ThemeEditorPtr;
};

class FDungeonArchitectThemeEditor : public FWorkflowCentricApplication,
                                     public FTickableGameObject
{
public:
    virtual ~FDungeonArchitectThemeEditor() override;
    
    // FAssetEditorToolkit
    virtual FName GetToolkitFName() const override;
    virtual FText GetBaseToolkitName() const override;
    virtual FText GetToolkitName() const override;
    virtual FLinearColor GetWorldCentricTabColorScale() const override;
    virtual FString GetWorldCentricTabPrefix() const override;
    virtual FString GetDocumentationLink() const override;
    // End of FAssetEditorToolkit

    // FTickableGameObject Interface
    virtual bool IsTickableInEditor() const override;
    virtual void Tick(float DeltaTime) override;
    virtual bool IsTickable() const override;
    virtual TStatId GetStatId() const override;
    // End of FTickableGameObject Interface

    void InitDungeonEditor(EToolkitMode::Type Mode, const TSharedPtr<class IToolkitHost>& InitToolkitHost, UDungeonThemeAsset* PropData);
    UDungeonThemeAsset* GetAssetBeingEdited() const { return AssetBeingEdited; }
    bool CanAccessModeThemeGraph() const;
    bool CanAccessModeMarkerGenerator() const;

    void HandleOpenHelpSystem();
    void CompileThemeGraph() const;
    static FText GetLocalizedMode(FName InMode);
    TSharedPtr<FThemeEditorToolbar> GetToolbarBuilder() const { return ToolbarBuilder; };
    void RegisterToolbarTab(const TSharedRef<class FTabManager>& InTabManager);
    
    template<typename T>
    TSharedPtr<T> GetAppMode(const FName& InModeName) {
        TSharedPtr<FThemeEditorAppModeBase>* SearchResult = AppModes.Find(InModeName);
        return SearchResult ? StaticCastSharedPtr<T>(*SearchResult) : nullptr;
    }
    

protected:
    void ExtendMenu();
    virtual void SaveAsset_Execute() override;
    void BindCommands();
    void UpdateThumbnail() const;
    void NotifyBuilderClassChanged(TSubclassOf<class UDungeonBuilder> InBuilderClass);
    
public:
    static const FName AppModeID_GraphEditor;
    static const FName AppModeID_MarkerGenerator;

protected:
    TMap<FName, TSharedPtr<FThemeEditorAppModeBase>> AppModes;
    UDungeonThemeAsset* AssetBeingEdited = nullptr;
    TSharedPtr<FThemeEditorToolbar> ToolbarBuilder;
	TSharedPtr<SThemePreviewViewport> PreviewViewport;

    friend class SThemePreviewViewport;
};


class FDungeonArchitectThemeEditorUtils {
public:
    static bool GetBoundsForSelectedNodes(const UEdGraph* Graph, class FSlateRect& Rect, float Padding = 0.0f);

private:
    static TSharedPtr<FDungeonArchitectThemeEditor> GetThemeEditorForAsset(const UEdGraph* Graph);
};

