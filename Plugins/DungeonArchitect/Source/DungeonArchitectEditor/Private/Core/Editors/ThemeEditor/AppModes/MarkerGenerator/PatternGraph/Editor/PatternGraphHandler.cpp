//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Editor/PatternGraphHandler.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Nodes/PatternGraphNodeImpl.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNode.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"

#include "EdGraphUtilities.h"
#include "Framework/Commands/GenericCommands.h"
#include "HAL/PlatformApplicationMisc.h"
#include "Kismet2/BlueprintEditorUtils.h"
#include "SNodePanel.h"

FMGPatternGraphHandler::~FMGPatternGraphHandler() {
}

void FMGPatternGraphHandler::Bind() {
    GraphEditorCommands = MakeShareable(new FUICommandList);
    GraphEditorCommands->MapAction(FGenericCommands::Get().SelectAll,
                                   FExecuteAction::CreateSP(this, &FMGPatternGraphHandler::SelectAllNodes),
                                   FCanExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CanSelectAllNodes)
    );

    GraphEditorCommands->MapAction(FGenericCommands::Get().Delete,
                                   FExecuteAction::CreateSP(this, &FMGPatternGraphHandler::DeleteSelectedNodes),
                                   FCanExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CanDeleteNodes)
    );

    GraphEditorCommands->MapAction(FGenericCommands::Get().Copy,
                                   FExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CopySelectedNodes),
                                   FCanExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CanCopyNodes)
    );

    GraphEditorCommands->MapAction(FGenericCommands::Get().Paste,
                                   FExecuteAction::CreateSP(this, &FMGPatternGraphHandler::PasteNodes),
                                   FCanExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CanPasteNodes)
    );

    GraphEditorCommands->MapAction(FGenericCommands::Get().Cut,
                                   FExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CutSelectedNodes),
                                   FCanExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CanCutNodes)
    );

    GraphEditorCommands->MapAction(FGenericCommands::Get().Duplicate,
                                   FExecuteAction::CreateSP(this, &FMGPatternGraphHandler::DuplicateNodes),
                                   FCanExecuteAction::CreateSP(this, &FMGPatternGraphHandler::CanDuplicateNodes)
    );

    GraphEvents.OnSelectionChanged = SGraphEditor::FOnSelectionChanged::CreateSP(
        this, &FMGPatternGraphHandler::HandleSelectedNodesChanged);
    GraphEvents.OnNodeDoubleClicked = FSingleNodeEvent::CreateSP(
        this, &FMGPatternGraphHandler::HandleNodeDoubleClicked);
}

void FMGPatternGraphHandler::SetGraphEditor(TSharedPtr<SGraphEditor> InGraphEditor) {
    this->GraphEditor = InGraphEditor;
}

void FMGPatternGraphHandler::SetPropertyEditor(TWeakPtr<IDetailsView> InPropertyEditor) {
    PropertyEditor = InPropertyEditor;
}

void FMGPatternGraphHandler::SelectAllNodes() {
    if (!GraphEditor.IsValid()) return;
    GraphEditor->SelectAllNodes();
}

bool FMGPatternGraphHandler::CanSelectAllNodes() const {
    return GraphEditor.IsValid();
}

void FMGPatternGraphHandler::DeleteSelectedNodes() {
    if (!GraphEditor.IsValid()) return;
    TArray<UEdGraphNode*> NodesToDelete;
    const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

    for (FGraphPanelSelectionSet::TConstIterator NodeIt(SelectedNodes); NodeIt; ++NodeIt) {
        NodesToDelete.Add(CastChecked<UEdGraphNode>(*NodeIt));
    }

    DeleteNodes(NodesToDelete);

    if (NodesToDelete.Num() > 0) {
        GraphEditor->GetCurrentGraph()->NotifyGraphChanged();
    }
}

bool FMGPatternGraphHandler::CanDeleteNode(class UEdGraphNode* Node) {
    if (!GraphEditor.IsValid()) return false;
    if (Node->IsA<UMGPatternGraphNode_Result>() || Node->IsA<UMGPatternGraphNode_OnPass>()) {
        return false;
    }
    return true;
}

void FMGPatternGraphHandler::DeleteNodes(const TArray<class UEdGraphNode*>& NodesToDelete) {
    if (!GraphEditor.IsValid()) return;
    if (NodesToDelete.Num() > 0) {

        for (int32 Index = 0; Index < NodesToDelete.Num(); ++Index) {
            if (!CanDeleteNode(NodesToDelete[Index])) {
                continue;
            }

            // Break all node links first so that we don't update the material before deleting
            NodesToDelete[Index]->BreakAllNodeLinks();

            FBlueprintEditorUtils::RemoveNode(nullptr, NodesToDelete[Index], true);

            // TODO: Process deletion in the data model
        }
    }
}

void FMGPatternGraphHandler::DeleteSelectedDuplicatableNodes() {
    if (!GraphEditor.IsValid()) return;
    // Cache off the old selection
    const FGraphPanelSelectionSet OldSelectedNodes = GraphEditor->GetSelectedNodes();

    // Clear the selection and only select the nodes that can be duplicated
    FGraphPanelSelectionSet RemainingNodes;
    GraphEditor->ClearSelectionSet();

    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(OldSelectedNodes); SelectedIter; ++SelectedIter) {
        UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
        if ((Node != nullptr) && Node->CanDuplicateNode()) {
            GraphEditor->SetNodeSelection(Node, true);
        }
        else {
            RemainingNodes.Add(Node);
        }
    }

    // Delete the duplicatable nodes
    DeleteSelectedNodes();

    // Reselect whatever is left from the original selection after the deletion
    GraphEditor->ClearSelectionSet();

    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(RemainingNodes); SelectedIter; ++SelectedIter) {
        if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter)) {
            GraphEditor->SetNodeSelection(Node, true);
        }
    }
}

bool FMGPatternGraphHandler::CanDeleteNodes() const {
    if (!GraphEditor.IsValid()) return false;
    return true;
}

void FMGPatternGraphHandler::CopySelectedNodes() {
    if (!GraphEditor.IsValid()) return;

    // Export the selected nodes and place the text on the clipboard
    const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();

    FString ExportedText;

    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter) {
        if (UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter)) {
            Node->PrepareForCopying();
        }
    }

    FEdGraphUtilities::ExportNodesToText(SelectedNodes, /*out*/ ExportedText);
    FPlatformApplicationMisc::ClipboardCopy(*ExportedText);
}

bool FMGPatternGraphHandler::CanCopyNodes() const {
    if (!GraphEditor.IsValid()) return false;

    // If any of the nodes can be duplicated then we should allow copying
    const FGraphPanelSelectionSet SelectedNodes = GraphEditor->GetSelectedNodes();
    for (FGraphPanelSelectionSet::TConstIterator SelectedIter(SelectedNodes); SelectedIter; ++SelectedIter) {
        UEdGraphNode* Node = Cast<UEdGraphNode>(*SelectedIter);
        if ((Node != nullptr) && Node->CanDuplicateNode()) {
            return true;
        }
    }
    return false;
}

void FMGPatternGraphHandler::PasteNodes() {
    if (!GraphEditor.IsValid()) return;

    PasteNodesHere(GraphEditor->GetPasteLocation());
}

bool FMGPatternGraphHandler::CanPasteNodes() const {
    if (!GraphEditor.IsValid()) return false;

    FString ClipboardContent;
    FPlatformApplicationMisc::ClipboardPaste(ClipboardContent);

    return FEdGraphUtilities::CanImportNodesFromText(GraphEditor->GetCurrentGraph(), ClipboardContent);
}

void FMGPatternGraphHandler::PasteNodesHere(const FVector2D& Location) {
    if (!GraphEditor.IsValid()) return;

    // Undo/Redo support
    const FScopedTransaction Transaction(NSLOCTEXT("DungeonArchitect", "DungeonEditorPaste", "Dungeon Editor: Paste"));
    // TODO: Notify the data model of modification
    //Material->MaterialGraph->Modify();
    //Material->Modify();

    // Clear the selection set (newly pasted stuff will be selected)
    GraphEditor->ClearSelectionSet();

    // Grab the text to paste from the clipboard.
    FString TextToImport;
    FPlatformApplicationMisc::ClipboardPaste(TextToImport);

    // Import the nodes
    TSet<UEdGraphNode*> PastedNodes;
    FEdGraphUtilities::ImportNodesFromText(GraphEditor->GetCurrentGraph(), TextToImport, /*out*/ PastedNodes);

    //Average position of nodes so we can move them while still maintaining relative distances to each other
    FVector2D AvgNodePosition(0.0f, 0.0f);

    for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It) {
        UEdGraphNode* Node = *It;
        AvgNodePosition.X += Node->NodePosX;
        AvgNodePosition.Y += Node->NodePosY;
    }

    if (PastedNodes.Num() > 0) {
        float InvNumNodes = 1.0f / static_cast<float>(PastedNodes.Num());
        AvgNodePosition.X *= InvNumNodes;
        AvgNodePosition.Y *= InvNumNodes;
    }

    for (TSet<UEdGraphNode*>::TIterator It(PastedNodes); It; ++It) {
        UEdGraphNode* Node = *It;

        // Select the newly pasted stuff
        GraphEditor->SetNodeSelection(Node, true);

        Node->NodePosX = (Node->NodePosX - AvgNodePosition.X) + Location.X;
        Node->NodePosY = (Node->NodePosY - AvgNodePosition.Y) + Location.Y;

        Node->SnapToGrid(SNodePanel::GetSnapGridSize());

        // Give new node a different Guid from the old one
        Node->CreateNewGuid();
        Node->PostPlacedNewNode();
        //Node->AllocateDefaultPins();
    }

    // TODO: Implement
    //UpdatePropAfterGraphChange();

    // Update UI
    GraphEditor->NotifyGraphChanged();
}

void FMGPatternGraphHandler::CutSelectedNodes() {
    if (!GraphEditor.IsValid()) return;

    CopySelectedNodes();
    // Cut should only delete nodes that can be duplicated
    DeleteSelectedDuplicatableNodes();
}

bool FMGPatternGraphHandler::CanCutNodes() const {
    if (!GraphEditor.IsValid()) return false;

    return CanCopyNodes() && CanDeleteNodes();
}

void FMGPatternGraphHandler::DuplicateNodes() {
    if (!GraphEditor.IsValid()) return;

    // Copy and paste current selection
    CopySelectedNodes();
    PasteNodes();
}

bool FMGPatternGraphHandler::CanDuplicateNodes() const {
    if (!GraphEditor.IsValid()) return false;

    return CanCopyNodes();
}

void FMGPatternGraphHandler::HandleSelectedNodesChanged(const TSet<class UObject*>& NewSelection) const {
    TArray<UObject*> ObjectsToShow;

    for (UObject* Obj : NewSelection) {
        if (const UMGPatternGraphNode* RuleNode = Cast<UMGPatternGraphNode>(Obj)) {
            if (RuleNode->NodeTemplate) {
                ObjectsToShow.Add(RuleNode->NodeTemplate);
            }
        }
    }

    if (PropertyEditor.IsValid()) {
        const TSharedPtr<IDetailsView> DetailsView = PropertyEditor.Pin();
        DetailsView->SetObjects(ObjectsToShow);
    }

    if (OnNodeSelectionChanged.IsBound()) {
        OnNodeSelectionChanged.Execute(NewSelection);
    }
}

void FMGPatternGraphHandler::HandleNodeDoubleClicked(class UEdGraphNode* Node) const {
    OnNodeDoubleClicked.ExecuteIfBound(Node);
}


