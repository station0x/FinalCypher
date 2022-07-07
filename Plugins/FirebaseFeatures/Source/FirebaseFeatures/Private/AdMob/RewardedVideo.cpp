// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "AdMob/FbRewardedVideo.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/admob/rewarded_video.h"
THIRD_PARTY_INCLUDES_END

#include "FirebaseFeatures.h"
#include "FirebaseSdk/FirebaseConfig.h"
#include "AdMob/AdMobHelper.h"

#include "Async/Async.h"

#include "FirebaseFeatures.h"

class FRewardedVideoListener final 
#if WITH_FIREBASE_ADMOB
	: public firebase::admob::rewarded_video::Listener
#endif
{
public:
#if WITH_FIREBASE_ADMOB
	virtual void OnRewarded(firebase::admob::rewarded_video::RewardItem reward) final
	{
		FAdMobRewardItem Item;
		Item.Amount		= reward.amount;
		Item.RewardType = UTF8_TO_TCHAR(reward.reward_type.c_str());

		UE_LOG(LogAdMob, Log, TEXT("Received Rewarded Video's reward. Type: %s. Amount: %f."), *Item.RewardType, Item.Amount);

		if (FRewardedVideo::OnRewardedEvent.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Item]() -> void 
			{
				FRewardedVideo::OnRewardedEvent.ExecuteIfBound(Item.RewardType, Item.Amount);
			});
		}
	}

	virtual void OnPresentationStateChanged(firebase::admob::rewarded_video::PresentationState state) final
	{
		UE_LOG(LogAdMob, Log, TEXT("Rewarded video state has changed."));

		if (FRewardedVideo::OnPresentationStateChangedEvent.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [state]() -> void
			{
				FRewardedVideo::OnPresentationStateChangedEvent.ExecuteIfBound(static_cast<EAdMobPresentationState>(state));
			});
		}
	}
#endif // WITH_FIREBASE_ADMOB

	virtual ~FRewardedVideoListener() final
	{
	}
};

FAdMobVideoOnRewarded					FRewardedVideo::OnRewardedEvent;
FAdMobVideoOnPresentationStateChanged	FRewardedVideo::OnPresentationStateChangedEvent;
TUniquePtr<FRewardedVideoListener>		FRewardedVideo::Listener = MakeUnique<FRewardedVideoListener>();

void FRewardedVideo::LoadAd(const FString& AdUnitId, const FAdMobAdRequest& AdRequest, const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	
	// Firebase functions have to be copiable.
	auto Request = MakeShared<FAdMobHelper::FInitAdRequestMemoryHandler>(AdRequest);

	firebase::admob::rewarded_video::LoadAd(TCHAR_TO_UTF8(*AdUnitId), Request->Get()).
	
	OnCompletion([Callback, Request = MoveTemp(Request)](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to load ad for rewarded video: Code %d, Message: %s."), _Future.error(), UTF8_TO_TCHAR(_Future.error_message()));
		}

		AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void FRewardedVideo::Pause(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	firebase::admob::rewarded_video::Pause().
	
	OnCompletion([Callback](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to pause ad for rewarded video: Code %d, Message: %s."), Error, UTF8_TO_TCHAR(_Future.error_message()));
		}
		if (Callback.IsBound()) AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void FRewardedVideo::Resume(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	firebase::admob::rewarded_video::Resume().
	
	OnCompletion([Callback](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to resume ad for rewarded video: Code %d, Message: %s"), Error, UTF8_TO_TCHAR(_Future.error_message()));
		}
		if (Callback.IsBound()) AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void FRewardedVideo::SetListener()
{
#if WITH_FIREBASE_ADMOB
	firebase::admob::rewarded_video::SetListener(Listener.Get());
#endif // WITH_FIREBASE_ADMOB
}

void FRewardedVideo::Show(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	firebase::admob::rewarded_video::Show(FAdMobHelper::GetAdParent()).
	
	OnCompletion([Callback](const firebase::Future<void>& _Future) -> void
	{
		const EFbAdMobError Error = (EFbAdMobError)_Future.error();
		if (Error != EFbAdMobError::None)
		{
			UE_LOG(LogAdMob, Error, TEXT("Failed to show rewarded video: Code %d, Message: %s"), Error, UTF8_TO_TCHAR(_Future.error_message()));
		}

		if (Callback.IsBound()) AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
		{
			Callback.ExecuteIfBound(Error);
		});
	});
#else  // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}
