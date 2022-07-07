// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FbAdMobTypes.h"
#include "FbRewardedVideo.generated.h"

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_TwoParams(FAdMobVideoOnRewarded, const FString&, RewardType, const float, RewardAmount);
UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_OneParam(FAdMobVideoOnPresentationStateChanged, const EAdMobPresentationState, PresentationState);

class FIREBASEFEATURES_API FRewardedVideo final
{
private:
	friend class FRewardedVideoListener;
	friend class FFirebaseFeaturesModule;

private:
	FRewardedVideo() = delete;
	FRewardedVideo(const FRewardedVideo& ) = delete;
	FRewardedVideo(const FRewardedVideo&&) = delete;

public:
	static void LoadAd	(const FString& AdUnitId, const FAdMobAdRequest& AdRequest, const FAdMobCallback& Callback = FAdMobCallback());
	static void Pause	(const FAdMobCallback& Callback = FAdMobCallback());
	static void Resume	(const FAdMobCallback& Callback = FAdMobCallback());
	static void Show	(const FAdMobCallback& Callback = FAdMobCallback());

	FORCEINLINE static FAdMobVideoOnRewarded& OnRewarded()
	{
		return OnRewardedEvent;
	}

	FORCEINLINE static FAdMobVideoOnPresentationStateChanged& OnPresentationStateChanged()
	{
		return OnPresentationStateChangedEvent;
	}

private:
	static void SetListener();

private:
	static FAdMobVideoOnRewarded OnRewardedEvent;
	static FAdMobVideoOnPresentationStateChanged OnPresentationStateChangedEvent;

	static TUniquePtr<class FRewardedVideoListener> Listener;

};

