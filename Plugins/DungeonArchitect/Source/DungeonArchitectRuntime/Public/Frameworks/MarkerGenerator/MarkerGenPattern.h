//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/PatternScript/PatternScript.h"
#include "MarkerGenPattern.generated.h"

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMarkerGenPatternRule : public UObject {
	GENERATED_BODY()
public:
	UMarkerGenPatternRule();
	void AssignRandomColor();
	static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

public:
	UPROPERTY()
	FGuid RuleId;

	/**
	 * Tell the system that you'll be inserting an art asset at this location
	 * By default, if the rule graph emits a marker (EmitMarker action node), it would know that this position would be occupied by an art asset.
	 * In cases where you'd insert a larger asset, e.g. a 2x2 tile, you'd use the EmitMarker node in one of the 2x2 position and the system
	 * needs to know that the nearby 3 tiles would also be occupied. Go to each one and set this hint so your final result does not have overlaps
	 */
	UPROPERTY(EditAnywhere, Category = "Pattern Rule")
	bool bHintWillInsertAssetHere = false;
	
	UPROPERTY()
	UMGPatternScript* RuleScript = nullptr;
	
#if WITH_EDITORONLY_DATA
	// Make the editor rule block pop out in the scene.   This is purely visual, disable it for blocks whose visuals get in the way and don't really contribute to the pattern you're looking for
	UPROPERTY(EditAnywhere, Category = "Visuals")
	bool bVisuallyDominant = true;
	
	UPROPERTY(EditAnywhere, Category = "Visuals")
	FLinearColor Color = FLinearColor::White;

	/** EdGraph based representation */
	UPROPERTY()
	class UEdGraph* RuleEdGraph = nullptr;
#endif // WITH_EDITORONLY_DATA

};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMarkerGenPattern : public UObject {
	GENERATED_BODY()
public:
	UPROPERTY()
	TArray<UMarkerGenPatternRule*> Rules;
};

