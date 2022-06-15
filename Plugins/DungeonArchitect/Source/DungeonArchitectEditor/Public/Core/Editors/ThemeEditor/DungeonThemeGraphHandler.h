//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "IDetailsView.h"

class FDungeonArchitectThemeGraphHandler : public TSharedFromThis<FDungeonArchitectThemeGraphHandler> {
public:
    virtual ~FDungeonArchitectThemeGraphHandler() {}
    void Bind();
    void Initialize(TSharedPtr<SGraphEditor> InGraphEditor, TSharedPtr<IDetailsView> InPropertyEditor, UEdGraph* InThemeGraph, UObject* InEditorProperties);
    TSharedPtr<FUICommandList> GetCommands() const { return GraphEditorCommands; }

    /** Select every node in the graph */
    void SelectAllNodes();
    /** Whether we can select every node */
    bool CanSelectAllNodes() const;

    /** Deletes all the selected nodes */
    void DeleteSelectedNodes();

    bool CanDeleteNode(class UEdGraphNode* Node);

    /** Delete an array of Material Graph Nodes and their corresponding expressions/comments */
    void DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete);

    /** Delete only the currently selected nodes that can be duplicated */
    void DeleteSelectedDuplicatableNodes();

    /** Whether we are able to delete the currently selected nodes */
    bool CanDeleteNodes() const;

    /** Copy the currently selected nodes */
    void CopySelectedNodes();
    /** Whether we are able to copy the currently selected nodes */
    bool CanCopyNodes() const;

    /** Paste the contents of the clipboard */
    void PasteNodes();
    bool CanPasteNodes() const;
    void PasteNodesHere(const FVector2D& Location);

    /** Cut the currently selected nodes */
    void CutSelectedNodes();
    /** Whether we are able to cut the currently selected nodes */
    bool CanCutNodes() const;

    /** Duplicate the currently selected nodes */
    void DuplicateNodes();
    /** Whether we are able to duplicate the currently selected nodes */
    bool CanDuplicateNodes() const;

    SGraphEditor::FGraphEditorEvents& GetGraphEvents() { return GraphEvents; }

    virtual void OnSelectedNodesChanged(const TSet<class UObject*>& NewSelection);
    virtual void OnNodeDoubleClicked(class UEdGraphNode* Node);

    DECLARE_DELEGATE_OneParam(FSingleNodeDelegate, UEdGraphNode*);
    DECLARE_DELEGATE_OneParam(FMultiNodeDelegate, const TSet<UObject*>&);

    FMultiNodeDelegate SelectedNodesChanged;
    FSingleNodeDelegate NodeDoubleClicked;

private:
    TSharedPtr<SGraphEditor> GetGraphEditor() const;
    UEdGraph* GetThemeGraph() const;
    void ShowProperties(UObject* InObject) const;
    void ShowProperties(const TArray<UObject*>& InObjects) const;
    void ShowDefaultEditorProperties() const;

public:
    SGraphEditor::FGraphEditorEvents GraphEvents;

private:
    TSharedPtr<FUICommandList> GraphEditorCommands;
    
    TWeakPtr<SGraphEditor> GraphEditorPtr;
    TWeakPtr<IDetailsView> PropertyEditorPtr;
    TWeakObjectPtr<UEdGraph> ThemeGraphPtr;
    TWeakObjectPtr<UObject> EditorPropertiesPtr;
};

