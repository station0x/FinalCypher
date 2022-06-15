//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNodePin.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptVariable.h"
#include "PatternScriptNode.generated.h"

struct FMGActionExecContext;
struct FMGConditionExecContext;
UCLASS(Abstract)
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNode : public UObject {
	GENERATED_BODY()

public:
	virtual void AllocateDefaultPins();
	virtual FText GetNodeTitle() const { return FText(); }

	UMGPatternScriptNodePin* GetOutputPin(const FName& PinId) const;
	UMGPatternScriptNodePin* GetInputPin(const FName& PinId) const;

	UMGPatternScriptNodePin* GetOutputExecPin() const;
	EMGPatternScriptVarDataType GetOutPinType(const FName& OutputPinID) const;
	
protected:
	template<typename TGraphNodePin>
	TGraphNodePin* AddInputPin(const FName& InName, bool bMandatory) {
		return AddPinImpl<TGraphNodePin>(InputPins, EMGPatternScriptPinDirection::Input, InName, bMandatory);
	}
	
	template<typename TGraphNodePin>
	TGraphNodePin* AddOutputPin(const FName& InName) {
		return AddPinImpl<TGraphNodePin>(OutputPins, EMGPatternScriptPinDirection::Output, InName, true);
	}

private:
	template<typename TGraphNodePin>
	TGraphNodePin* AddPinImpl(TMap<FName, UMGPatternScriptNodePin*>& InPinMap, EMGPatternScriptPinDirection InDirection,
				const FName& InName, bool bMandatory) {
		TGraphNodePin* Pin = NewObject<TGraphNodePin>(this, InName);
		Pin->Name = InName;
		Pin->Direction = InDirection;
		Pin->ConnectivityConstraint = bMandatory
				? EMGPatternScriptPinConnectivityConstraint::Mandatory
				: EMGPatternScriptPinConnectivityConstraint::Optional;
	
		InPinMap.Add(InName, Pin);
		return Pin;
	}

public:
	UPROPERTY()
	FGuid NodeGuid;
	
	UPROPERTY()
	TMap<FName, UMGPatternScriptNodePin*> InputPins;

	UPROPERTY()
	TMap<FName, UMGPatternScriptNodePin*> OutputPins;
};

UCLASS(Abstract)
class DUNGEONARCHITECTRUNTIME_API UMGPatternConditionalScriptNodeBase : public UMGPatternScriptNode {
	GENERATED_BODY()
public:
	virtual bool Execute(const FMGConditionExecContext& InContext) { return false; }
	virtual FString GenerateRuleText() const { return ""; }
	virtual bool IsBinaryOperator() const;

protected:
	bool GetInputPinValue(const FMGConditionExecContext& InContext, const FName& InPinID) const;
	static bool GetPinDefaultValue(UMGPatternScriptNodePin* InPin);
};


UCLASS(Abstract)
class DUNGEONARCHITECTRUNTIME_API UMGPatternActionScriptNodeBase : public UMGPatternScriptNode {
	GENERATED_BODY()
public:
	virtual void Execute(const FMGActionExecContext& InContext) {}
	virtual FName GetInputExecPinId() const { return NAME_None; }
	virtual FName GetOutputExecPinId() const { return NAME_None; }
	virtual FString GetActionText() const { return ""; }
};

