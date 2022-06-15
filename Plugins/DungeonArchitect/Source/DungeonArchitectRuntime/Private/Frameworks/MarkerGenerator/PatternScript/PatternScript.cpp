//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/PatternScript/PatternScript.h"

#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"

UMGPatternScriptNode* UMGPatternScript::CreateNode(const FGuid& InNodeId, const UClass* InNodeClass, UMGPatternScriptNode* InTemplate) {
	UMGPatternScriptNode* Node = NewObject<UMGPatternScriptNode>(this, InNodeClass, NAME_None, RF_NoFlags, InTemplate);
	Node->NodeGuid = InNodeId;
	Node->AllocateDefaultPins();
	Nodes.Add(Node);
	return Node;
}


void UMGPatternScript::Clear() {
	Nodes.Reset();
}

void UMGPatternScript::GenerateNodeLookup(TMap<FGuid, UMGPatternScriptNode*>& OutNodeLookup) {
	for (UMGPatternScriptNode* Node : Nodes) {
		if (!Node || !Node->NodeGuid.IsValid()) continue;
		UMGPatternScriptNode*& NodeRef = OutNodeLookup.FindOrAdd(Node->NodeGuid);
		NodeRef = Node;
	}
}

UMGPatternActionScriptNodeBase* FMGPatternScriptUtils::GetOutgoingExecNode(UMGPatternActionScriptNodeBase* InExecNode) {
	if (InExecNode) {
		UMGPatternScriptNodePin* OutputPin = InExecNode->GetOutputPin(InExecNode->GetOutputExecPinId());
		if (OutputPin) {
			if (const UMGPatternScriptNodePin* LinkedPin = OutputPin->GetLinkedTo()) {
				return Cast<UMGPatternActionScriptNodeBase>(LinkedPin->GetOuter());
			}
		}
	}
	return nullptr;
}

FString FMGPatternScriptUtils::GetPinRuleText(UMGPatternScriptNodePin* InputPin, bool bApplyParenthesis) {
	if (InputPin) {
		UMGPatternScriptNodePin* IncomingPin = InputPin->GetLinkedTo();
		if (!IncomingPin) {
			const FString DefaultValue = InputPin->GetDefaultValue();
			return DefaultValue.ToLower() == "true" ? "True" : "False";
		}

		if (const UMGPatternConditionalScriptNodeBase* IncomingNode = Cast<UMGPatternConditionalScriptNodeBase>(IncomingPin->GetOwner())) {
			FString RuleText = IncomingNode->GenerateRuleText();
			if (bApplyParenthesis && IncomingNode->IsBinaryOperator()) {
				RuleText = "(" + RuleText + ")";
			}
			return RuleText;
		}
	}

	return "";
}

UMGPatternConditionalScriptNodeBase* FMGPatternScriptUtils::GetIncomingConditionNode(const UMGPatternConditionalScriptNodeBase* InNode, const FName& InPinName) {
	if (InNode) {
		if (const UMGPatternScriptNodePin* InputPin = InNode->GetInputPin(InPinName)) {
			if (const UMGPatternScriptNodePin* LinkedPin = InputPin->GetLinkedTo()) {
				return Cast<UMGPatternConditionalScriptNodeBase>(LinkedPin->GetOwner());
			}
		}
	}
	return nullptr;
}


