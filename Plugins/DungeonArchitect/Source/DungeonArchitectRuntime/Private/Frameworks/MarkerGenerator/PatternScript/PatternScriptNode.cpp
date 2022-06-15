//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"

#include "Frameworks/MarkerGenerator/PatternScript/PatternScript.h"

void UMGPatternScriptNode::AllocateDefaultPins() {
}

UMGPatternScriptNodePin* UMGPatternScriptNode::GetOutputPin(const FName& PinId) const {
	UMGPatternScriptNodePin* const* PinPtr = OutputPins.Find(PinId);
	return PinPtr ? *PinPtr : nullptr;
}

UMGPatternScriptNodePin* UMGPatternScriptNode::GetInputPin(const FName& PinId) const {
	UMGPatternScriptNodePin* const* PinPtr = InputPins.Find(PinId);
	return PinPtr ? *PinPtr : nullptr;
}

UMGPatternScriptNodePin* UMGPatternScriptNode::GetOutputExecPin() const {
	for (auto& Entry : OutputPins) {
		UMGPatternScriptNodePin* OutputPin = Entry.Value;
		if (OutputPin->GetDataType() == EMGPatternScriptVarDataType::Exec) {
			return OutputPin;
		}
	}
	return nullptr;
}
EMGPatternScriptVarDataType UMGPatternScriptNode::GetOutPinType(const FName& OutputPinID) const {
	UMGPatternScriptNodePin* OutputPin = GetOutputPin(OutputPinID);
	return OutputPin ? OutputPin->GetDataType() : EMGPatternScriptVarDataType::Unknown;
}

bool UMGPatternConditionalScriptNodeBase::IsBinaryOperator() const {
	return InputPins.Num() > 1;
}

bool UMGPatternConditionalScriptNodeBase::GetInputPinValue(const FMGConditionExecContext& InContext, const FName& InPinID) const {
	UMGPatternScriptNodePin* InputPin = GetInputPin(InPinID);
	if (!InputPin) return false;

	UMGPatternScriptNodePin* LinkedPin = InputPin->GetLinkedTo();
	if (!LinkedPin) {
		// Nothing is linked to this node. Get the default value of the pin
		return GetPinDefaultValue(InputPin);
	}

	UMGPatternConditionalScriptNodeBase* IncomingNode = Cast<UMGPatternConditionalScriptNodeBase>(LinkedPin->GetOuter());
	if (IncomingNode) {
		return IncomingNode->Execute(InContext);		
	}
	return false;
}

bool UMGPatternConditionalScriptNodeBase::GetPinDefaultValue(UMGPatternScriptNodePin* InPin) {
	if (!InPin) return false;
	return InPin->GetDefaultValue() == TEXT("true");
}


