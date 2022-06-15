//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Nodes/PatternGraphNodeRegistry.h"

#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/Nodes/PatternGraphNodeImpl.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphSchema.h"
#include "Core/Editors/ThemeEditor/AppModes/MarkerGenerator/PatternGraph/PatternGraphUtils.h"

// NOTE: This is an auto-generated file.  Do not modify,  update the definitions.py file instead

#define LOCTEXT_NAMESPACE "UMGPatternGraphSchema"

void UMGPatternGraphSchema::GetActionList(TArray<TSharedPtr<FEdGraphSchemaAction>>& OutActions, const UEdGraph* Graph, UEdGraph* OwnerOfTemporaries) const {
	const FText CategoryCondition = LOCTEXT("CategoryConditionLabel", "Pattern Conditions");
	const FText CategoryAction = LOCTEXT("CategoryActionLabel", "Pattern Actions");
	FMGPatternSchemaUtils::AddAction<UMGPatternGraphNode_EmitMarker>( TEXT("EmitMarker"), TEXT("Emit Marker"), CategoryAction, OutActions, OwnerOfTemporaries);
	FMGPatternSchemaUtils::AddAction<UMGPatternGraphNode_RemoveMarker>( TEXT("RemoveMarker"), TEXT("Remove Marker"), CategoryAction, OutActions, OwnerOfTemporaries);
	FMGPatternSchemaUtils::AddAction<UMGPatternGraphNode_LogicalAnd>( TEXT("LogicalAnd"), TEXT("AND"), CategoryCondition, OutActions, OwnerOfTemporaries);
	FMGPatternSchemaUtils::AddAction<UMGPatternGraphNode_LogicalNot>( TEXT("LogicalNot"), TEXT("NOT"), CategoryCondition, OutActions, OwnerOfTemporaries);
	FMGPatternSchemaUtils::AddAction<UMGPatternGraphNode_LogicalOr>( TEXT("LogicalOr"), TEXT("OR"), CategoryCondition, OutActions, OwnerOfTemporaries);
	FMGPatternSchemaUtils::AddAction<UMGPatternGraphNode_MarkerExists>( TEXT("MarkerExists"), TEXT("Marker Exists"), CategoryCondition, OutActions, OwnerOfTemporaries);
}

#undef LOCTEXT_NAMESPACE

