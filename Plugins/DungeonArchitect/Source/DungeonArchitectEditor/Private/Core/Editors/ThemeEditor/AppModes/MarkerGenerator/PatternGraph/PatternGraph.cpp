//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraph.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Nodes/PatternGraphNodeImpl.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphSchema.h"

UMGPatternGraph::UMGPatternGraph(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	Schema = UMGPatternGraphSchema::StaticClass();
}

void UMGPatternGraph::Initialize() {
	// Create the result node
	{
		FGraphNodeCreator<UMGPatternGraphNode_Result> ResultNodeCreator(*this);
		ResultNode = ResultNodeCreator.CreateNode(true);
		ResultNode->NodePosX = 0;
		ResultNode->NodePosY = 0;
		ResultNodeCreator.Finalize();
		if (UEdGraphPin* InputPin = ResultNode->GetPinAt(0)) {
			InputPin->DefaultValue = TEXT("true");
		}
	}

	// Create the action pass node
	{
		FGraphNodeCreator<UMGPatternGraphNode_OnPass> ActionNodeCreator(*this);
		ActionPassNode = ActionNodeCreator.CreateNode(false);
		ActionPassNode->NodePosX = 176;
		ActionPassNode->NodePosY = 0;
		ActionNodeCreator.Finalize();
	}
}

