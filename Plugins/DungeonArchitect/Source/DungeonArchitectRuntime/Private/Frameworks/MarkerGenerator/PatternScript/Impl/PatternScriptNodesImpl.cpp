//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/PatternScript/Impl/PatternScriptNodesImpl.h"


// NOTE: This is an auto-generated file.  Do not modify,  update the definitions.py file instead

#define LOCTEXT_NAMESPACE "PatternScriptNodesImpl"

/////////////////////// Node: EmitMarker ///////////////////////
const FName UMGPatternScriptNode_EmitMarker::InPinID_Default = "DefaultInput";
const FName UMGPatternScriptNode_EmitMarker::OutPinID_Default = "DefaultOutput";
void UMGPatternScriptNode_EmitMarker::AllocateDefaultPins() {
	AddInputPin<UMGPatternScriptNodePinExec>(InPinID_Default, true);
	AddOutputPin<UMGPatternScriptNodePinExec>(OutPinID_Default);
}

FText UMGPatternScriptNode_EmitMarker::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_EmitMarker", "Emit Marker");
	return NodeTitle;
}

/////////////////////// Node: RemoveMarker ///////////////////////
const FName UMGPatternScriptNode_RemoveMarker::InPinID_Default = "DefaultInput";
const FName UMGPatternScriptNode_RemoveMarker::OutPinID_Default = "DefaultOutput";
void UMGPatternScriptNode_RemoveMarker::AllocateDefaultPins() {
	AddInputPin<UMGPatternScriptNodePinExec>(InPinID_Default, true);
	AddOutputPin<UMGPatternScriptNodePinExec>(OutPinID_Default);
}

FText UMGPatternScriptNode_RemoveMarker::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_RemoveMarker", "Remove Marker");
	return NodeTitle;
}

/////////////////////// Node: LogicalAnd ///////////////////////
const FName UMGPatternScriptNode_LogicalAnd::InPinID_A = "A";
const FName UMGPatternScriptNode_LogicalAnd::InPinID_B = "B";
const FName UMGPatternScriptNode_LogicalAnd::OutPinID_Default = "DefaultOutput";
void UMGPatternScriptNode_LogicalAnd::AllocateDefaultPins() {
	AddInputPin<UMGPatternScriptNodePinBool>(InPinID_A, true);
	AddInputPin<UMGPatternScriptNodePinBool>(InPinID_B, true);
	AddOutputPin<UMGPatternScriptNodePinBool>(OutPinID_Default);
}

FText UMGPatternScriptNode_LogicalAnd::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_LogicalAnd", "AND");
	return NodeTitle;
}

/////////////////////// Node: LogicalNot ///////////////////////
const FName UMGPatternScriptNode_LogicalNot::InPinID_Default = "DefaultInput";
const FName UMGPatternScriptNode_LogicalNot::OutPinID_Default = "DefaultOutput";
void UMGPatternScriptNode_LogicalNot::AllocateDefaultPins() {
	AddInputPin<UMGPatternScriptNodePinBool>(InPinID_Default, true);
	AddOutputPin<UMGPatternScriptNodePinBool>(OutPinID_Default);
}

FText UMGPatternScriptNode_LogicalNot::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_LogicalNot", "NOT");
	return NodeTitle;
}

/////////////////////// Node: LogicalOr ///////////////////////
const FName UMGPatternScriptNode_LogicalOr::InPinID_A = "A";
const FName UMGPatternScriptNode_LogicalOr::InPinID_B = "B";
const FName UMGPatternScriptNode_LogicalOr::OutPinID_Default = "DefaultOutput";
void UMGPatternScriptNode_LogicalOr::AllocateDefaultPins() {
	AddInputPin<UMGPatternScriptNodePinBool>(InPinID_A, true);
	AddInputPin<UMGPatternScriptNodePinBool>(InPinID_B, true);
	AddOutputPin<UMGPatternScriptNodePinBool>(OutPinID_Default);
}

FText UMGPatternScriptNode_LogicalOr::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_LogicalOr", "OR");
	return NodeTitle;
}

/////////////////////// Node: MarkerExists ///////////////////////
const FName UMGPatternScriptNode_MarkerExists::OutPinID_Default = "DefaultOutput";
void UMGPatternScriptNode_MarkerExists::AllocateDefaultPins() {
	AddOutputPin<UMGPatternScriptNodePinBool>(OutPinID_Default);
}

FText UMGPatternScriptNode_MarkerExists::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_MarkerExists", "Marker Exists");
	return NodeTitle;
}

/////////////////////// Node: OnPass ///////////////////////
const FName UMGPatternScriptNode_OnPass::OutPinID_Default = "DefaultOutput";
void UMGPatternScriptNode_OnPass::AllocateDefaultPins() {
	AddOutputPin<UMGPatternScriptNodePinExec>(OutPinID_Default);
}

FText UMGPatternScriptNode_OnPass::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_OnPass", "On Pattern Selected");
	return NodeTitle;
}

/////////////////////// Node: Result ///////////////////////
const FName UMGPatternScriptNode_Result::InPinID_Select = "Select";
void UMGPatternScriptNode_Result::AllocateDefaultPins() {
	AddInputPin<UMGPatternScriptNodePinBool>(InPinID_Select, true);
}

FText UMGPatternScriptNode_Result::GetNodeTitle() const {
	static const FText NodeTitle = LOCTEXT("NodeTitle_Result", "Selection Condition");
	return NodeTitle;
}


#undef LOCTEXT_NAMESPACE

