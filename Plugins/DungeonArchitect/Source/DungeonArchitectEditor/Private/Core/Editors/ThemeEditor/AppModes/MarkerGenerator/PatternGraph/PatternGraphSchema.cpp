//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphSchema.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNodePin.h"

#include "Framework/Commands/GenericCommands.h"
#include "GraphEditorActions.h"
#include "ToolMenu.h"

#define LOCTEXT_NAMESPACE "MGPatternGraphSchema"

namespace MGPatternGraphSchemaLib {
	FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) {
		const UGraphEditorSettings* Settings = GetDefault<UGraphEditorSettings>();
		if (Settings) {
			if (PinType.PinCategory == FMGPatternPinCategory::Bool) {
				return Settings->BooleanPinTypeColor;
			}
			else if (PinType.PinCategory == FMGPatternPinCategory::Exec) {
				return Settings->ExecutionPinTypeColor;
			}
		}

		return FLinearColor::White;
	}
}

//////////////////////////////// Schema ////////////////////////////////
UMGPatternGraphSchema::UMGPatternGraphSchema() {
	
}

FConnectionDrawingPolicy* UMGPatternGraphSchema::CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const {
	return new FMGPatternGraphConnDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements);
}

FLinearColor UMGPatternGraphSchema::GetPinTypeColor(const FEdGraphPinType& PinType) const {
	return MGPatternGraphSchemaLib::GetPinTypeColor(PinType);
}

void UMGPatternGraphSchema::GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const {
	const UEdGraph* Graph = ContextMenuBuilder.CurrentGraph;
	TArray<TSharedPtr<FEdGraphSchemaAction>> Actions;
	GetActionList(Actions, Graph, ContextMenuBuilder.OwnerOfTemporaries);

	for (TSharedPtr<FEdGraphSchemaAction> Action : Actions) {
		ContextMenuBuilder.AddAction(Action);
	}
}

void UMGPatternGraphSchema::GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const {
	if (Context && Context->Node) {
		FToolMenuSection& Section = Menu->AddSection("PatternGraphNodeActions", LOCTEXT("NodeActionsMenuHeader", "Node Actions"));
		if (!Context->bIsDebugging)
		{
			// Node contextual actions
			Section.AddMenuEntry(FGenericCommands::Get().Delete);
			Section.AddMenuEntry(FGenericCommands::Get().Cut);
			Section.AddMenuEntry(FGenericCommands::Get().Copy);
			Section.AddMenuEntry(FGenericCommands::Get().Duplicate);
			Section.AddMenuEntry(FGraphEditorCommands::Get().ReconstructNodes);
			Section.AddMenuEntry(FGraphEditorCommands::Get().BreakNodeLinks);
		}
	}
	Super::GetContextMenuActions(Menu, Context);
}

const FPinConnectionResponse UMGPatternGraphSchema::CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const {
	// Make sure the data types match
	if (A->PinType.PinCategory != B->PinType.PinCategory) {
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));
	}
	// Make sure they are not the same pins
	if (A->GetOwningNode() == B->GetOwningNode()) {
		return FPinConnectionResponse(CONNECT_RESPONSE_DISALLOW, TEXT("Not allowed"));
	}

	ECanCreateConnectionResponse Response = CONNECT_RESPONSE_BREAK_OTHERS_AB;
	if (A->PinType.PinCategory == FMGPatternPinCategory::Exec) {
		// We can have only one outgoing link
		Response = (A->Direction == EGPD_Output) ? CONNECT_RESPONSE_BREAK_OTHERS_A : CONNECT_RESPONSE_BREAK_OTHERS_B;
	}
	else if (A->PinType.PinCategory == FMGPatternPinCategory::Bool) {
		// We can have only one incoming link
		Response = (A->Direction == EGPD_Input) ? CONNECT_RESPONSE_BREAK_OTHERS_A : CONNECT_RESPONSE_BREAK_OTHERS_B;
	}

	return FPinConnectionResponse(Response, TEXT(""));
}

//////////////////////////////// Connection Drawing Policy ////////////////////////////////
FMGPatternGraphConnDrawingPolicy::FMGPatternGraphConnDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements)
	: FConnectionDrawingPolicy(InBackLayerID, InFrontLayerID, InZoomFactor, InClippingRect, InDrawElements)
{
}

void FMGPatternGraphConnDrawingPolicy::DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) {
	FConnectionDrawingPolicy::DetermineWiringStyle(OutputPin, InputPin, Params);

	FLinearColor WireColor;
	if (InputPin) {
		WireColor = MGPatternGraphSchemaLib::GetPinTypeColor(InputPin->PinType);
	}
	else if (OutputPin) {
		WireColor = MGPatternGraphSchemaLib::GetPinTypeColor(OutputPin->PinType);
	}
	else {
		WireColor = FLinearColor::White;
	}
		
	Params.WireColor = WireColor;
}

#undef LOCTEXT_NAMESPACE

