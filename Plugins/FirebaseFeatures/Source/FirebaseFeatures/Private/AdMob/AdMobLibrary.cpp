// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "AdMob/FbAdMobLibrary.h"

void UFirebaseAdMobLibrary::Blueprint_RewardedVideoEvents(const FAdMobVideoOnRewarded& OnReward, const FAdMobVideoOnPresentationStateChanged& OnPresentationStateChanged)
{
	FRewardedVideo::OnRewarded().operator=(OnReward);
	FRewardedVideo::OnPresentationStateChanged().operator=(OnPresentationStateChanged);
}
