//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNode.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphSchema.h"

void UMGPatternGraphNode::ReconstructNode() {
	// Break any links to 'orphan' pins
	for (int32 PinIndex = 0; PinIndex < Pins.Num(); ++PinIndex)
	{
		UEdGraphPin* Pin = Pins[PinIndex];
		TArray<class UEdGraphPin*>& LinkedToRef = Pin->LinkedTo;
		for (int32 LinkIdx=0; LinkIdx < LinkedToRef.Num(); LinkIdx++)
		{
			UEdGraphPin* OtherPin = LinkedToRef[LinkIdx];
			if (!OtherPin) continue;
			// If we are linked to a pin that its owner doesn't know about, break that link
			if (!OtherPin->GetOwningNode()->Pins.Contains(OtherPin))
			{
				Pin->LinkedTo.Remove(OtherPin);
			}
		}
	}

	// Store the old Input and Output pins 
	TArray<UEdGraphPin*> OldInputPins;
	TArray<UEdGraphPin*> OldOutputPins;
	for (int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++) {
		Pins[PinIndex]->LinkedTo = Pins[PinIndex]->LinkedTo.FilterByPredicate([](UEdGraphPin* InPin) { return !!InPin; });
		if (Pins[PinIndex]->Direction == EGPD_Input) {
			OldInputPins.Add(Pins[PinIndex]);
		}
		else if (Pins[PinIndex]->Direction == EGPD_Output) {
			OldOutputPins.Add(Pins[PinIndex]);
		}
	}

	// Move the existing pins to a saved array
	TArray<UEdGraphPin*> OldPins(Pins);
	Pins.Reset();

	// Recreate the new pins
	AllocateDefaultPins();

	// Get new Input and Output pins
	TArray<UEdGraphPin*> NewInputPins;
	TArray<UEdGraphPin*> NewOutputPins;
	for (int32 PinIndex = 0; PinIndex < Pins.Num(); PinIndex++) {
		if (Pins[PinIndex]->Direction == EGPD_Input) {
			NewInputPins.Add(Pins[PinIndex]);
		}
		else if (Pins[PinIndex]->Direction == EGPD_Output) {
			NewOutputPins.Add(Pins[PinIndex]);
		}
	}

	for (int32 PinIndex = 0; PinIndex < OldInputPins.Num(); PinIndex++) {
		if (PinIndex < NewInputPins.Num()) {
			NewInputPins[PinIndex]->MovePersistentDataFromOldPin(*OldInputPins[PinIndex]);
		}
	}

	for (int32 PinIndex = 0; PinIndex < OldOutputPins.Num(); PinIndex++) {
		if (PinIndex < NewOutputPins.Num()) {
			NewOutputPins[PinIndex]->MovePersistentDataFromOldPin(*OldOutputPins[PinIndex]);
		}
	}

	// Throw away the original pins
	for (UEdGraphPin* OldPin : OldPins)
	{
		OldPin->Modify();
		UEdGraphNode::DestroyPin(OldPin);
	}
}

bool UMGPatternGraphNode::CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const {
	return Schema->IsA(UMGPatternGraphSchema::StaticClass());
}

void UMGPatternGraphNode::NodeConnectionListChanged() {
	Super::NodeConnectionListChanged();
	GetGraph()->NotifyGraphChanged();
}

void UMGPatternGraphNode::PinDefaultValueChanged(UEdGraphPin* Pin) {
	Super::PinDefaultValueChanged(Pin);

	GetGraph()->NotifyGraphChanged();
}

void UMGPatternGraphNode::AutowireNewNode(UEdGraphPin* FromPin) {
	if (!FromPin) {
		return;
	}

	// Get the first pin that has the opposite direction
	for (UEdGraphPin* Pin : Pins) {
		if (Pin->Direction != FromPin->Direction && Pin->PinType.PinCategory == FromPin->PinType.PinCategory) {
			UEdGraphPin* OutputPin = nullptr;
			UEdGraphPin* InputPin = nullptr;
			if (FromPin->Direction == EGPD_Output) {
				OutputPin = FromPin;
				InputPin = Pin;
			}
			else {
				OutputPin = Pin;
				InputPin = FromPin;
			}
			OutputPin->MakeLinkTo(InputPin);
			
			OutputPin->Modify();
			InputPin->Modify();
			GetGraph()->NotifyGraphChanged();
			
			break;
		}
	}

}


