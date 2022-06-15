//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/MarkerGenPattern.h"
#include "MarkerGenGridPattern.generated.h"

UENUM()
enum class EMarkerGenGridPatternRuleType : uint8 {
	Ground,
	EdgeX,
	EdgeY,
	Corner
};

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMarkerGenGridPatternRule : public UMarkerGenPatternRule {
	GENERATED_BODY()
public:
	UMarkerGenGridPatternRule();
	
public:
	UPROPERTY()
	FIntPoint Coord;

	UPROPERTY()
	EMarkerGenGridPatternRuleType RuleType;
};


UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMarkerGenGridPattern : public UMarkerGenPattern {
	GENERATED_BODY()
public:
	UMarkerGenGridPatternRule* AddNewRule(const FIntPoint& InCoord, EMarkerGenGridPatternRuleType InRuleType);
	UMarkerGenGridPatternRule* GetRule(const FIntPoint& InCoord, EMarkerGenGridPatternRuleType InRuleType);
	
private:
	FORCEINLINE static UMarkerGenGridPatternRule* GetItemImpl(const FIntPoint& InCoord, const TMap<FIntPoint, UMarkerGenGridPatternRule*>& InMap) {
		UMarkerGenGridPatternRule* const* SearchResult = InMap.Find(InCoord);
		return SearchResult ? *SearchResult : nullptr;
	}
};

struct DUNGEONARCHITECTRUNTIME_API FMGGridPatternAssemblyRule {
	bool bHintWillInsertAssetHere = false;
	FIntPoint Coord;
	EMarkerGenGridPatternRuleType RuleType;
	TWeakObjectPtr<UMGPatternScript> RuleScript;

#if WITH_EDITORONLY_DATA
	bool bVisuallyDominant = true;
	FLinearColor Color;
	class UEdGraph* RuleEdGraph = nullptr;
#endif
};

struct DUNGEONARCHITECTRUNTIME_API FMGGridPatternAssembly {
	TArray<FMGGridPatternAssemblyRule> Rules;
	FIntPoint BoundMin = FIntPoint::ZeroValue;
	FIntPoint BoundMax = FIntPoint::ZeroValue;
	float RotationAngleRad = 0;
	int32 Rotation90Index = 0;
};

struct DUNGEONARCHITECTRUNTIME_API FMGGridPatternAssemblyBuilder {
	static void GenerateAssembly(const UMarkerGenPattern* InPattern, FMGGridPatternAssembly& OutPatternAssembly);
	static void RotateAssembly90(const FMGGridPatternAssembly& InAssembly, FMGGridPatternAssembly& OutRotatedAssembly);

	static void UpdateBounds(FMGGridPatternAssembly& Pattern);
};

