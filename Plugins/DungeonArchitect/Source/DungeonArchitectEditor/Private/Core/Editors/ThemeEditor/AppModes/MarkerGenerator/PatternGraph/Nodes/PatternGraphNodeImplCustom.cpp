//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Nodes/PatternGraphNodeImpl.h"

#include "Frameworks/MarkerGenerator/PatternScript/Impl/PatternScriptNodesImpl.h"

#define LOCTEXT_NAMESPACE "MGPatternCustomNodeImpl"

//////////////////////// UMGPatternGraphNode_Result ////////////////////////
FLinearColor UMGPatternGraphNode_Result::GetNodeTitleColor() const {
	return FLinearColor(1.0f, 0.25f, 0.25f);
}

FLinearColor UMGPatternGraphNode_Result::GetNodeBodyTintColor() const {
	return FLinearColor(1.0f, 0.5f, 0.5f);
}

//////////////////////// UMGPatternGraphNode_OnPass ////////////////////////
FLinearColor UMGPatternGraphNode_OnPass::GetNodeTitleColor() const {
	return FLinearColor(0.25f, 1.0f, 0.25f);
}

FLinearColor UMGPatternGraphNode_OnPass::GetNodeBodyTintColor() const {
	return FLinearColor(0.5f, 1.0f, 0.5f);
}

//////////////////////// UMGPatternGraphNode_MarkerExists ////////////////////////
FText UMGPatternGraphNode_MarkerExists::GetNodeTitle(ENodeTitleType::Type Title) const {
	FString MarkerName;
	
	if (UMGPatternScriptNode_MarkerExists* ScriptNode = Cast<UMGPatternScriptNode_MarkerExists>(NodeTemplate)) {
		MarkerName = ScriptNode->MarkerName;
	}

	if (MarkerName.IsEmpty()) {
		MarkerName = "<NONE>";
	}
	
	FFormatNamedArguments Args;
	Args.Add(TEXT("MarkerName"), FText::FromString(MarkerName));

	return FText::Format(LOCTEXT("MarkerExists_TitleFormat", "Marker Exists: {MarkerName}"), Args);
}

//////////////////////// UMGPatternGraphNode_EmitMarker ////////////////////////
FText UMGPatternGraphNode_EmitMarker::GetNodeTitle(ENodeTitleType::Type Title) const {
	FString MarkerName;
	
	if (UMGPatternScriptNode_EmitMarker* ScriptNode = Cast<UMGPatternScriptNode_EmitMarker>(NodeTemplate)) {
		MarkerName = ScriptNode->MarkerName;
	}

	if (MarkerName.IsEmpty()) {
		MarkerName = "<NONE>";
	}
	
	FFormatNamedArguments Args;
	Args.Add(TEXT("MarkerName"), FText::FromString(MarkerName));

	return FText::Format(LOCTEXT("EmitMarker_TitleFormat", "Emit Marker: {MarkerName}"), Args);
}

//////////////////////// UMGPatternGraphNode_RemoveMarker ////////////////////////
FText UMGPatternGraphNode_RemoveMarker::GetNodeTitle(ENodeTitleType::Type Title) const {
	FString MarkerName;
	
	if (UMGPatternScriptNode_RemoveMarker* ScriptNode = Cast<UMGPatternScriptNode_RemoveMarker>(NodeTemplate)) {
		MarkerName = ScriptNode->MarkerName;
	}

	if (MarkerName.IsEmpty()) {
		MarkerName = "<NONE>";
	}
	
	FFormatNamedArguments Args;
	Args.Add(TEXT("MarkerName"), FText::FromString(MarkerName));

	return FText::Format(LOCTEXT("RemoveMarker_TitleFormat", "Remove Marker: {MarkerName}"), Args);
}

#undef LOCTEXT_NAMESPACE

