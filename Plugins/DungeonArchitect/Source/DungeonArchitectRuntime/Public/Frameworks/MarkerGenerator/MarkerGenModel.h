//$ Copyright 2015-22, Code Respawn Technologies Pvt Ltd - All Rights Reserved $//

#pragma once
#include "CoreMinimal.h"
#include "MarkerGenModel.generated.h"

class UMarkerGenLayer;

UCLASS()
class DUNGEONARCHITECTRUNTIME_API UMarkerGenModel : public UObject {
	GENERATED_BODY()
public:

	UPROPERTY()
	TArray<UMarkerGenLayer*> Layers;
};

