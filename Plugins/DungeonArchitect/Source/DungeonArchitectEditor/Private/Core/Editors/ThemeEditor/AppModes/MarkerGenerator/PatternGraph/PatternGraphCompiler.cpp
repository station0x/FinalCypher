//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphCompiler.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraph.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphNode.h"
#include "Frameworks/MarkerGenerator/MarkerGenPattern.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"

void FMGPatternGraphCompiler::Compile(UMarkerGenPattern* InPattern) {
	// TODO: Return error messages
	if (!InPattern) return;

	for (UMarkerGenPatternRule* Rule : InPattern->Rules) {
		Compile(Rule);
	}
}

void FMGPatternGraphCompiler::Compile(const UMarkerGenPatternRule* InRule) {
	if (!InRule) return;
	
	UMGPatternScript* Script = InRule->RuleScript;
	UMGPatternGraph* Graph = Cast<UMGPatternGraph>(InRule->RuleEdGraph);

	if (!Script || !Graph) {
		return;
	}

	Script->Clear();
	
	TMap<UMGPatternGraphNode*, UMGPatternScriptNode*> NodeMap;
	TMap<UEdGraphPin*, UMGPatternScriptNodePin*> PinMap;
	for (UEdGraphNode* EdNode : Graph->Nodes) {
		if (UMGPatternGraphNode* RuleEdNode = Cast<UMGPatternGraphNode>(EdNode)) {
			if (!RuleEdNode || !RuleEdNode->NodeTemplate) continue;
			
			UMGPatternScriptNode* Template = RuleEdNode->NodeTemplate;
			UMGPatternScriptNode* ScriptNode = NewObject<UMGPatternScriptNode>(Script, Template->GetClass(), NAME_None, RF_NoFlags, Template);
			ScriptNode->NodeGuid = RuleEdNode->NodeGuid;
			ScriptNode->AllocateDefaultPins();
			Script->Nodes.Add(ScriptNode);
			NodeMap.Add(RuleEdNode, ScriptNode);

			{
				if (UMGPatternScriptNode_Result* ResultNode = Cast<UMGPatternScriptNode_Result>(ScriptNode)) {
					Script->ResultNode = ResultNode;
				}
				else if (UMGPatternScriptNode_OnPass* PassEventNode = Cast<UMGPatternScriptNode_OnPass>(ScriptNode)) {
					Script->PassEventNode = PassEventNode;
				}
			}

			// Map out the pins
			for (UEdGraphPin* EdPin : RuleEdNode->Pins) {
				//Pin->PinName
				UMGPatternScriptNodePin* ScriptPin = nullptr;
				if (EdPin->Direction == EGPD_Input) {
					FName PinName = EdPin->PinName; 
					if (PinName.IsNone()) {
						PinName = "DefaultInput";
					}
					ScriptPin = ScriptNode->GetInputPin(PinName);
				}
				else if (EdPin->Direction == EGPD_Output) {
					FName PinName = EdPin->PinName; 
					if (PinName.IsNone()) {
						PinName = "DefaultOutput";
					}
					ScriptPin = ScriptNode->GetOutputPin(PinName);
				}

				if (ScriptPin) {
					ScriptPin->SetDefaultValue(EdPin->DefaultValue);
					PinMap.Add(EdPin, ScriptPin);
				}
			}
		}
	}

	
	for (UEdGraphNode* EdNode : Graph->Nodes) {
		for (UEdGraphPin* EdPin : EdNode->Pins) {
			if (EdPin->Direction == EGPD_Output) {
				for (const UEdGraphPin* LinkedTo : EdPin->LinkedTo) {
					UMGPatternScriptNodePin** SrcPinPtr = PinMap.Find(EdPin);
					UMGPatternScriptNodePin** DstPinPtr = PinMap.Find(LinkedTo);
					if (!SrcPinPtr || !DstPinPtr) continue;

					UMGPatternScriptNodePin* SrcPin = *SrcPinPtr;
					UMGPatternScriptNodePin* DstPin = *DstPinPtr;

					if (SrcPin && DstPin) {
						SrcPin->SetLinkedTo(DstPin);
						DstPin->SetLinkedTo(SrcPin);
					}
				}
			}
		} 
	}

	Script->Modify();
}

