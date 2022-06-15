//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/PatternScript/Impl/PatternScriptNodesImpl.h"
#include "PatternScript.generated.h"

class UMGPatternScriptNodePin;
class UMGPatternScriptNode;
UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMGPatternScript : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UMGPatternScriptNode*> Nodes;

	UPROPERTY()
	TWeakObjectPtr<UMGPatternScriptNode_Result> ResultNode;

	UPROPERTY()
	TWeakObjectPtr<UMGPatternScriptNode_OnPass> PassEventNode;
	
public:
	template<typename TNode>
	TNode* CreateNode() {
		TNode* Node = NewObject<TNode>(this);
		Node->AllocateDefaultPins();
		Nodes.Add(Node);
		return Node;
	}
	

	template<typename TNode>
	TNode* FindNode() const {
		for (UMGPatternScriptNode* Node : Nodes) {
			if (TNode* CastNode = Cast<TNode>(Node)) {
				return CastNode;
			}
		}
		return nullptr;
	}

	template<typename TNode>
	void FindNodes(TArray<TNode*>& OutNodes) const {
		for (UMGPatternScriptNode* Node : Nodes) {
			if (TNode* CastNode = Cast<TNode>(Node)) {
				OutNodes.Add(CastNode);
			}
		}
	}

	UMGPatternScriptNode* CreateNode(const FGuid& InNodeId, const UClass* InNodeClass, UMGPatternScriptNode* InTemplate = nullptr);
	
public:
	void Clear();
	void GenerateNodeLookup(TMap<FGuid, UMGPatternScriptNode*>& OutNodeLookup);
};


class DUNGEONARCHITECTRUNTIME_API FMGPatternScriptUtils {
public:
	static UMGPatternActionScriptNodeBase* GetOutgoingExecNode(UMGPatternActionScriptNodeBase* InExecNode);
	static FString GetPinRuleText(UMGPatternScriptNodePin* InputPin, bool bApplyParenthesis = true);
	static UMGPatternConditionalScriptNodeBase* GetIncomingConditionNode(const UMGPatternConditionalScriptNodeBase* InNode, const FName& InPinName);
};

