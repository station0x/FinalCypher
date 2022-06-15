//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "ConnectionDrawingPolicy.h"
#include "PatternGraphSchema.generated.h"

UCLASS()
class DUNGEONARCHITECTEDITOR_API UMGPatternGraphSchema : public UEdGraphSchema {
	GENERATED_BODY()
public:
	UMGPatternGraphSchema();
	
	// Begin EdGraphSchema interface
	virtual FConnectionDrawingPolicy* CreateConnectionDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor,
		const FSlateRect& InClippingRect, FSlateWindowElementList& InDrawElements, UEdGraph* InGraphObj) const override;

	virtual FLinearColor GetPinTypeColor(const FEdGraphPinType& PinType) const override;
	virtual void GetGraphContextActions(FGraphContextMenuBuilder& ContextMenuBuilder) const override;
	virtual void GetContextMenuActions(UToolMenu* Menu, UGraphNodeContextMenuContext* Context) const override;
	virtual const FPinConnectionResponse CanCreateConnection(const UEdGraphPin* A, const UEdGraphPin* B) const override;
	// End EdGraphSchema interface

	void GetActionList(TArray<TSharedPtr<FEdGraphSchemaAction>>& OutActions, const UEdGraph* Graph, UEdGraph* OwnerOfTemporaries) const;
};


class DUNGEONARCHITECTEDITOR_API FMGPatternGraphConnDrawingPolicy : public FConnectionDrawingPolicy {
public:
	FMGPatternGraphConnDrawingPolicy(int32 InBackLayerID, int32 InFrontLayerID, float InZoomFactor, const FSlateRect& InClippingRect,
		FSlateWindowElementList& InDrawElements);

	virtual void DetermineWiringStyle(UEdGraphPin* OutputPin, UEdGraphPin* InputPin, FConnectionParams& Params) override;
};

