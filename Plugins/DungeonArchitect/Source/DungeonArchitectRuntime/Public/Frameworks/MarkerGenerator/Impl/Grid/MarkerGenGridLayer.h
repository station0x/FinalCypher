//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "Frameworks/MarkerGenerator/MarkerGenLayer.h"
#include "MarkerGenGridLayer.generated.h"

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMarkerGenGridLayer : public UMarkerGenLayer {
	GENERATED_BODY()
public:

	/**
	 * List of marker names that should be on the same level while testing for the pattern
	 * E.g. 1: If you add "Ground" to the list, and if that marker exist at the location of any of the pattern rule blocks,
	 * they'll have to be on the same height, or the pattern won't match. This might be useful if you're inserting
	 * a larger ground tile (2x2) and don't want it to match with a adjacent ground tile of a different height
	 * E.g. 2: You're trying to turn a (Door-Wall) edge into a larger 2x Door. We don't want to match the pattern
	 * if the Wall and Door adjacent edges are on different heights, so you'd specify both "Wall" and "Door" in this list
	 */
	UPROPERTY(EditAnywhere, Category="Constraints")
	TArray<FString> SameHeightMarkers;

	UPROPERTY(EditAnywhere, Category = "Advanced")
	bool bExpandMarkerDomain = false;
    	
	UPROPERTY(EditAnywhere, Category = "Advanced", Meta=(EditCondition="bExpandMarkerDomain", UIMin = 0, UIMax = 10, ClampMin = 0, ClampMax = 20))
	int32 ExpandMarkerDomainAmount = 0;
};

