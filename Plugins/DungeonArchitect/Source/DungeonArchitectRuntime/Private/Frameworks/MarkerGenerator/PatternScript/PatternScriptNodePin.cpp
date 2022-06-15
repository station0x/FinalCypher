//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNodePin.h"

#include "Frameworks/MarkerGenerator/PatternScript/PatternScriptNode.h"

UMGPatternScriptNode* UMGPatternScriptNodePin::GetOwner() const {
	return Cast<UMGPatternScriptNode>(GetOuter());
}

void UMGPatternScriptNodePin::SetLinkedTo(UMGPatternScriptNodePin* InLinkedTo) {
	check(InLinkedTo && InLinkedTo->Direction != Direction);
	LinkedTo = InLinkedTo;
}

