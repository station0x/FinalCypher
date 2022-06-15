//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "PatternGraphNode.generated.h"

class UMGPatternScriptNode;

UCLASS(Abstract)
class UMGPatternGraphNode : public UEdGraphNode {
	GENERATED_BODY()
public:
	//~Begin UEdGraphNode Interface
	virtual void ReconstructNode() override;
	virtual bool CanCreateUnderSpecifiedSchema(const UEdGraphSchema* Schema) const override;
	virtual void NodeConnectionListChanged() override;
	virtual void PinDefaultValueChanged(UEdGraphPin* Pin) override;
	virtual void AutowireNewNode(UEdGraphPin* FromPin) override;
	//~End UEdGraphNode Interface

public:
	UPROPERTY()
	UMGPatternScriptNode* NodeTemplate;
};

UCLASS(Abstract)
class UMGPatternGraphConditionNode : public UMGPatternGraphNode {
	GENERATED_BODY()
public:
};

UCLASS(Abstract)
class UMGPatternGraphActionNode : public UMGPatternGraphNode {
	GENERATED_BODY()
public:
};

