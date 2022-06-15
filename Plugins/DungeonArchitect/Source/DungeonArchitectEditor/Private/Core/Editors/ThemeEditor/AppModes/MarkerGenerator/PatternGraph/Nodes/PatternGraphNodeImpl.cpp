//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Nodes/PatternGraphNodeImpl.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNodePin.h"
#include "Frameworks/MarkerGenerator/PatternScript/Impl/PatternScriptNodesImpl.h"

// NOTE: This is an auto-generated file.  Do not modify,  update the definitions.py file instead

#define LOCTEXT_NAMESPACE "MGPatternGraphNodeImpl"

/////////////////////// UMGPatternGraphNode_EmitMarker ///////////////////////
UMGPatternGraphNode_EmitMarker::UMGPatternGraphNode_EmitMarker(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_EmitMarker>(this, "Template");
}

void UMGPatternGraphNode_EmitMarker::AllocateDefaultPins() {
	CreatePin(EGPD_Input, FMGPatternPinCategory::Exec, "");
	CreatePin(EGPD_Output, FMGPatternPinCategory::Exec, "");
}

/////////////////////// UMGPatternGraphNode_RemoveMarker ///////////////////////
UMGPatternGraphNode_RemoveMarker::UMGPatternGraphNode_RemoveMarker(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_RemoveMarker>(this, "Template");
}

void UMGPatternGraphNode_RemoveMarker::AllocateDefaultPins() {
	CreatePin(EGPD_Input, FMGPatternPinCategory::Exec, "");
	CreatePin(EGPD_Output, FMGPatternPinCategory::Exec, "");
}

/////////////////////// UMGPatternGraphNode_LogicalAnd ///////////////////////
UMGPatternGraphNode_LogicalAnd::UMGPatternGraphNode_LogicalAnd(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_LogicalAnd>(this, "Template");
}

FText UMGPatternGraphNode_LogicalAnd::GetNodeTitle(ENodeTitleType::Type Title) const {
	return LOCTEXT("NodeTitle_LogicalAnd", "AND");
}

void UMGPatternGraphNode_LogicalAnd::AllocateDefaultPins() {
	CreatePin(EGPD_Input, FMGPatternPinCategory::Bool, "A");
	CreatePin(EGPD_Input, FMGPatternPinCategory::Bool, "B");
	CreatePin(EGPD_Output, FMGPatternPinCategory::Bool, "");
}

/////////////////////// UMGPatternGraphNode_LogicalNot ///////////////////////
UMGPatternGraphNode_LogicalNot::UMGPatternGraphNode_LogicalNot(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_LogicalNot>(this, "Template");
}

FText UMGPatternGraphNode_LogicalNot::GetNodeTitle(ENodeTitleType::Type Title) const {
	return LOCTEXT("NodeTitle_LogicalNot", "NOT");
}

void UMGPatternGraphNode_LogicalNot::AllocateDefaultPins() {
	CreatePin(EGPD_Input, FMGPatternPinCategory::Bool, "");
	CreatePin(EGPD_Output, FMGPatternPinCategory::Bool, "");
}

/////////////////////// UMGPatternGraphNode_LogicalOr ///////////////////////
UMGPatternGraphNode_LogicalOr::UMGPatternGraphNode_LogicalOr(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_LogicalOr>(this, "Template");
}

FText UMGPatternGraphNode_LogicalOr::GetNodeTitle(ENodeTitleType::Type Title) const {
	return LOCTEXT("NodeTitle_LogicalOr", "OR");
}

void UMGPatternGraphNode_LogicalOr::AllocateDefaultPins() {
	CreatePin(EGPD_Input, FMGPatternPinCategory::Bool, "A");
	CreatePin(EGPD_Input, FMGPatternPinCategory::Bool, "B");
	CreatePin(EGPD_Output, FMGPatternPinCategory::Bool, "");
}

/////////////////////// UMGPatternGraphNode_MarkerExists ///////////////////////
UMGPatternGraphNode_MarkerExists::UMGPatternGraphNode_MarkerExists(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_MarkerExists>(this, "Template");
}

void UMGPatternGraphNode_MarkerExists::AllocateDefaultPins() {
	CreatePin(EGPD_Output, FMGPatternPinCategory::Bool, "");
}

/////////////////////// UMGPatternGraphNode_OnPass ///////////////////////
UMGPatternGraphNode_OnPass::UMGPatternGraphNode_OnPass(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_OnPass>(this, "Template");
}

FText UMGPatternGraphNode_OnPass::GetNodeTitle(ENodeTitleType::Type Title) const {
	return LOCTEXT("NodeTitle_OnPass", "On Pattern Selected");
}

void UMGPatternGraphNode_OnPass::AllocateDefaultPins() {
	CreatePin(EGPD_Output, FMGPatternPinCategory::Exec, "");
}

/////////////////////// UMGPatternGraphNode_Result ///////////////////////
UMGPatternGraphNode_Result::UMGPatternGraphNode_Result(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer) {
	NodeTemplate = ObjectInitializer.CreateDefaultSubobject<UMGPatternScriptNode_Result>(this, "Template");
}

FText UMGPatternGraphNode_Result::GetNodeTitle(ENodeTitleType::Type Title) const {
	return LOCTEXT("NodeTitle_Result", "Selection Condition");
}

void UMGPatternGraphNode_Result::AllocateDefaultPins() {
	CreatePin(EGPD_Input, FMGPatternPinCategory::Bool, "Select");
}


#undef LOCTEXT_NAMESPACE

