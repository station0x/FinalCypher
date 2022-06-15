//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/Impl/Grid/MarkerGenGridPattern.h"

#include "Frameworks/MarkerGenerator/PatternScript/Impl/PatternScriptNodesImpl.h"

//////////////////////////// UMarkerGenGridPattern ////////////////////////////
UMarkerGenGridPatternRule* UMarkerGenGridPattern::AddNewRule(const FIntPoint& InCoord, EMarkerGenGridPatternRuleType InRuleType) {
	// Make sure a rule doesn't already exist in this location
	{
		UMarkerGenGridPatternRule* ExistingRule = GetRule(InCoord, InRuleType);
		if (ExistingRule) {
			return ExistingRule;
		}
	}
	
	UMarkerGenGridPatternRule* Rule = NewObject<UMarkerGenGridPatternRule>(this, UMarkerGenGridPatternRule::StaticClass(), NAME_None, RF_NoFlags);
	Rule->RuleType = InRuleType;
	Rule->Coord = InCoord;
	Rules.Add(Rule);
	return Rule;
}

UMarkerGenGridPatternRule* UMarkerGenGridPattern::GetRule(const FIntPoint& InCoord, EMarkerGenGridPatternRuleType InRuleType) {
	for (UMarkerGenPatternRule* Rule : Rules) {
		if (UMarkerGenGridPatternRule* GridRule = Cast<UMarkerGenGridPatternRule>(Rule)) {
			if (GridRule->Coord == InCoord && GridRule->RuleType == InRuleType) {
				return GridRule;
			}
		}
	}
	return nullptr;
}

//////////////////////////// UMarkerGenGridPatternRule ////////////////////////////
UMarkerGenGridPatternRule::UMarkerGenGridPatternRule() {
	
}

//////////////////////////// Assembly ////////////////////////////
void FMGGridPatternAssemblyBuilder::GenerateAssembly(const UMarkerGenPattern* InPattern, FMGGridPatternAssembly& OutPatternAssembly) {
	OutPatternAssembly = FMGGridPatternAssembly();
	OutPatternAssembly.RotationAngleRad = 0;
	for (const UMarkerGenPatternRule* Rule : InPattern->Rules) {
		if (const UMarkerGenGridPatternRule* GridRule = Cast<UMarkerGenGridPatternRule>(Rule)) {
			FMGGridPatternAssemblyRule& AsmRule = OutPatternAssembly.Rules.AddDefaulted_GetRef();
			AsmRule.bHintWillInsertAssetHere = GridRule->bHintWillInsertAssetHere;
			AsmRule.Coord = GridRule->Coord;
			AsmRule.RuleType = GridRule->RuleType;
			AsmRule.RuleScript = GridRule->RuleScript;
			
#if WITH_EDITORONLY_DATA
			AsmRule.bVisuallyDominant = GridRule->bVisuallyDominant;
			AsmRule.Color = GridRule->Color;
			AsmRule.RuleEdGraph = GridRule->RuleEdGraph;	
#endif

			if (!AsmRule.bHintWillInsertAssetHere) {
				for (const UMGPatternScriptNode* ScriptNode : GridRule->RuleScript->Nodes) {
					// Check if we have a emit marker node, so we can set the hint
					if (ScriptNode->IsA<UMGPatternScriptNode_EmitMarker>()) {
						AsmRule.bHintWillInsertAssetHere = true;
						break;
					}
				}
			}
		}
	}
	UpdateBounds(OutPatternAssembly);
}

void FMGGridPatternAssemblyBuilder::RotateAssembly90(const FMGGridPatternAssembly& InAssembly, FMGGridPatternAssembly& OutRotatedAssembly) {
	OutRotatedAssembly = InAssembly;
	OutRotatedAssembly.RotationAngleRad += PI * 0.5f;
	OutRotatedAssembly.Rotation90Index++;
	
	const FQuat Rotation(FVector::UpVector, PI * 0.5f);
	for (FMGGridPatternAssemblyRule& Rule : OutRotatedAssembly.Rules) {
		if (Rule.RuleType == EMarkerGenGridPatternRuleType::Corner) {
			const FVector Offset(0.5f, 0.5f, 0.0f);
			const FVector OldLocation = FVector(Rule.Coord.X, Rule.Coord.Y, 0) - Offset;
			const FVector NewLocation = Rotation * OldLocation + Offset;
			Rule.Coord = FIntPoint(
				FMath::RoundToInt(NewLocation.X),
				FMath::RoundToInt(NewLocation.Y));
		}
		else if (Rule.RuleType == EMarkerGenGridPatternRuleType::Ground) {
			const FVector OldLocation = FVector(Rule.Coord.X, Rule.Coord.Y, 0);
			const FVector NewLocation = Rotation * OldLocation;
			Rule.Coord = FIntPoint(
				FMath::RoundToInt(NewLocation.X),
				FMath::RoundToInt(NewLocation.Y));
		}
		else if (Rule.RuleType == EMarkerGenGridPatternRuleType::EdgeX) {
			const FVector OldLocation = FVector(Rule.Coord.X, Rule.Coord.Y - 0.5f, 0);
			const FVector NewLocation = Rotation * OldLocation + FVector(0.5f, 0, 0);;
			Rule.RuleType = EMarkerGenGridPatternRuleType::EdgeY;
			Rule.Coord = FIntPoint(
				FMath::RoundToInt(NewLocation.X),
				FMath::RoundToInt(NewLocation.Y));
		}
		else if (Rule.RuleType == EMarkerGenGridPatternRuleType::EdgeY) {
			const FVector OldLocation = FVector(Rule.Coord.X - 0.5f, Rule.Coord.Y, 0);
			const FVector NewLocation = Rotation * OldLocation + FVector(0, 0.5f, 0);;
			Rule.RuleType = EMarkerGenGridPatternRuleType::EdgeX;
			Rule.Coord = FIntPoint(
				FMath::RoundToInt(NewLocation.X),
				FMath::RoundToInt(NewLocation.Y));
		}
		else {
			// Not supported
			check(0);
		}
	}
	UpdateBounds(OutRotatedAssembly);
}

void FMGGridPatternAssemblyBuilder::UpdateBounds(FMGGridPatternAssembly& Pattern) {
	if (Pattern.Rules.Num() == 0) {
		Pattern.BoundMin = FIntPoint::ZeroValue;
		Pattern.BoundMax = FIntPoint::ZeroValue;
		return;
	}
	
	Pattern.BoundMin = Pattern.Rules[0].Coord;
	Pattern.BoundMax = Pattern.Rules[0].Coord;
	for (const FMGGridPatternAssemblyRule& Rule : Pattern.Rules) {
		Pattern.BoundMin.X = FMath::Min(Pattern.BoundMin.X, Rule.Coord.X);
		Pattern.BoundMin.Y = FMath::Min(Pattern.BoundMin.Y, Rule.Coord.Y);
		Pattern.BoundMax.X = FMath::Max(Pattern.BoundMax.X, Rule.Coord.X);
		Pattern.BoundMax.Y = FMath::Max(Pattern.BoundMax.Y, Rule.Coord.Y);
	}
}

