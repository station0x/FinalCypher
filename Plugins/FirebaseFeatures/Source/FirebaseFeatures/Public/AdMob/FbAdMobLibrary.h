// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FbRewardedVideo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FbAdMobLibrary.generated.h"

UCLASS()
class FIREBASEFEATURES_API UFirebaseAdMobLibrary final : public UBlueprintFunctionLibrary{
	GENERATED_BODY()
public:

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Rewarded Video", meta = (DisplayName = "Events - Rewarded Video", AutoCreateRefTerm="OnReward, OnPresentationStateChanged"))
	static void Blueprint_RewardedVideoEvents(const FAdMobVideoOnRewarded& OnReward, const FAdMobVideoOnPresentationStateChanged& OnPresentationStateChanged);

};


