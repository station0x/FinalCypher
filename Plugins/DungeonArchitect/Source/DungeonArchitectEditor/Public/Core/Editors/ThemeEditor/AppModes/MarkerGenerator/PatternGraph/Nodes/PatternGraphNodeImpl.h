//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNode.h"
#include "PatternGraphNodeImpl.generated.h"

// NOTE: This is an auto-generated file.  Do not modify,  update the definitions.py file instead

UCLASS()
class UMGPatternGraphNode_EmitMarker : public UMGPatternGraphActionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
};

UCLASS()
class UMGPatternGraphNode_RemoveMarker : public UMGPatternGraphActionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
};

UCLASS()
class UMGPatternGraphNode_LogicalAnd : public UMGPatternGraphConditionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
};

UCLASS()
class UMGPatternGraphNode_LogicalNot : public UMGPatternGraphConditionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
};

UCLASS()
class UMGPatternGraphNode_LogicalOr : public UMGPatternGraphConditionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
};

UCLASS()
class UMGPatternGraphNode_MarkerExists : public UMGPatternGraphConditionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
};

UCLASS()
class UMGPatternGraphNode_OnPass : public UMGPatternGraphActionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
    virtual FLinearColor GetNodeBodyTintColor() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
    virtual bool CanDuplicateNode() const override { return false; }
};

UCLASS()
class UMGPatternGraphNode_Result : public UMGPatternGraphConditionNode {
	GENERATED_UCLASS_BODY()
public:
	virtual FText GetNodeTitle(ENodeTitleType::Type TitleType) const override;
	virtual void AllocateDefaultPins() override;
    virtual FLinearColor GetNodeBodyTintColor() const override;
	virtual FLinearColor GetNodeTitleColor() const override;
    virtual bool CanDuplicateNode() const override { return false; }
};


