//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "EdGraph/EdGraph.h"
#include "PatternGraph.generated.h"

class UMGPatternGraphNode_OnPass;
class UMGPatternGraphNode_Result;
UCLASS()
class UMGPatternGraph : public UEdGraph {
	GENERATED_UCLASS_BODY()
public:
	void Initialize();

	UPROPERTY()
	UMGPatternGraphNode_Result* ResultNode = nullptr;

	UPROPERTY()
	UMGPatternGraphNode_OnPass* ActionPassNode = nullptr;
};

