//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#include "Frameworks/MarkerGenerator/MarkerGenPattern.h"


////////////////////////////// UMarkerGenPatternRule //////////////////////////////
UMarkerGenPatternRule::UMarkerGenPatternRule() {
	RuleScript = CreateDefaultSubobject<UMGPatternScript>("RuleScript");
	
	RuleId = FGuid::NewGuid();
	AssignRandomColor();
}

void UMarkerGenPatternRule::AssignRandomColor() {
#if WITH_EDITORONLY_DATA
	const uint8 Hue = FMath::RoundToInt(FMath::FRand() * 255);
	constexpr uint8 Sat = 200;
	constexpr uint8 Val = 255;
	Color = FLinearColor::MakeFromHSV8(Hue, Sat, Val);
#endif
}

void UMarkerGenPatternRule::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector) {
	UMarkerGenPatternRule* This = CastChecked<UMarkerGenPatternRule>(InThis);
	
#if WITH_EDITORONLY_DATA
	Collector.AddReferencedObject(This->RuleEdGraph, This);
#endif

	Super::AddReferencedObjects(InThis, Collector);
}

