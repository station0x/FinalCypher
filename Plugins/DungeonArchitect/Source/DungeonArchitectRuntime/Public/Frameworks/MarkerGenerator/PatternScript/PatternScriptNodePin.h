//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptVariable.h"
#include "PatternScriptNodePin.generated.h"

class UMGPatternScriptNode;

UENUM()
enum class EMGPatternScriptPinDirection : uint8 {
	Input,
	Output
};

UENUM()
enum class EMGPatternScriptPinConnectivityConstraint : uint8 {
	Mandatory,
	Optional
};

UCLASS(Abstract)
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNodePin : public UObject {
	GENERATED_BODY()
public:
	EMGPatternScriptVarDataType GetDataType() const { return DataType; }
	EMGPatternScriptPinDirection GetDirection() const { return Direction; }
	EMGPatternScriptPinConnectivityConstraint GetConnectivityConstraint() const { return ConnectivityConstraint; }
	FName GetPinName() const { return Name; }
	FString GetDefaultValue() const { return DefaultValue; }
	void SetDefaultValue(const FString& InValue) { DefaultValue = InValue; }

	UMGPatternScriptNode* GetOwner() const;
	UMGPatternScriptNodePin* GetLinkedTo() const { return LinkedTo; }
	void SetLinkedTo(UMGPatternScriptNodePin* InLinkedTo);
	
protected:
	UPROPERTY()
	FName Name;
	
	UPROPERTY()
	EMGPatternScriptVarDataType DataType;
	
	UPROPERTY()
	EMGPatternScriptPinDirection Direction;

	UPROPERTY()
	EMGPatternScriptPinConnectivityConstraint ConnectivityConstraint;
	
	UPROPERTY()
	UMGPatternScriptNodePin* LinkedTo;
	
	UPROPERTY()
	FString DefaultValue;

	friend class UMGPatternScriptNode;
};

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNodePinExec : public UMGPatternScriptNodePin {
	GENERATED_BODY()
public:
	UMGPatternScriptNodePinExec() { DataType = EMGPatternScriptVarDataType::Exec; };
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScriptNodePinBool : public UMGPatternScriptNodePin {
	GENERATED_BODY()
public:
	UMGPatternScriptNodePinBool() { DataType = EMGPatternScriptVarDataType::Bool; };
};

