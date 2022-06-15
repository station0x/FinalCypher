//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"
#include "PatternScriptNodesImpl.generated.h"

// NOTE: This is an auto-generated file.  Do not modify,  update the definitions.py file instead

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_EmitMarker : public UMGPatternActionScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FName GetOutputExecPinId() const override { return OutPinID_Default; }
	virtual FString GetActionText() const override;
	virtual void Execute(const FMGActionExecContext& InContext) override;

public:
	UPROPERTY(EditAnywhere, Category="Emit Marker")
	FString MarkerName;

	/**  Copy the rotation from one of the markers found in this list */
	UPROPERTY(EditAnywhere, Category="Emit Marker")
	TArray<FString> CopyRotationFromMarkers;
	
	/**  Copy the height from one of the markers found in this list */
	UPROPERTY(EditAnywhere, Category="Emit Marker")
	TArray<FString> CopyHeightFromMarkers;

	static const FName InPinID_Default;
	static const FName OutPinID_Default;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_RemoveMarker : public UMGPatternActionScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FName GetOutputExecPinId() const override { return OutPinID_Default; }
	virtual FString GetActionText() const override;
	virtual void Execute(const FMGActionExecContext& InContext) override;

public:
	UPROPERTY(EditAnywhere, Category="Remove Marker")
	FString MarkerName;
	
	static const FName InPinID_Default;
	static const FName OutPinID_Default;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_LogicalAnd : public UMGPatternConditionalScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FString GenerateRuleText() const override;
	virtual bool Execute(const FMGConditionExecContext& InContext) override;

public:
	static const FName InPinID_A;
	static const FName InPinID_B;
	static const FName OutPinID_Default;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_LogicalNot : public UMGPatternConditionalScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FString GenerateRuleText() const override;
	virtual bool Execute(const FMGConditionExecContext& InContext) override;

public:
	static const FName InPinID_Default;
	static const FName OutPinID_Default;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_LogicalOr : public UMGPatternConditionalScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FString GenerateRuleText() const override;
	virtual bool Execute(const FMGConditionExecContext& InContext) override;

public:
	static const FName InPinID_A;
	static const FName InPinID_B;
	static const FName OutPinID_Default;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_MarkerExists : public UMGPatternConditionalScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FString GenerateRuleText() const override;
	virtual bool Execute(const FMGConditionExecContext& InContext) override;

public:
	UPROPERTY(EditAnywhere, Category="Markers")
	FString MarkerName;
	
	static const FName OutPinID_Default;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_OnPass : public UMGPatternActionScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FName GetOutputExecPinId() const override { return OutPinID_Default; }
	virtual FString GetActionText() const override;
	virtual void Execute(const FMGActionExecContext& InContext) override;

public:
	static const FName OutPinID_Default;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode_Result : public UMGPatternConditionalScriptNodeBase {
	GENERATED_BODY()
public:
	virtual void AllocateDefaultPins() override;
	virtual FText GetNodeTitle() const override;
	virtual FString GenerateRuleText() const override;
	virtual bool Execute(const FMGConditionExecContext& InContext) override;

public:
	static const FName InPinID_Select;
};


