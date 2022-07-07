// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "AdMob/FbBannerView.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/admob/banner_view.h"
#	include "firebase/app.h"
THIRD_PARTY_INCLUDES_END

#include "FirebaseSdk/FirebaseConfig.h"

#include "AdMob/AdMobHelper.h"
#include "FirebaseFeatures.h"

#include "Async/Async.h"


DECLARE_LOG_CATEGORY_CLASS(LogAdMobBannerView, Log, All);

namespace
{
	void ExecuteCallback(const firebase::Future<void>& Future, const FAdMobCallback& Callback, const FString& MessageOnError)
	{
		const EFbAdMobError Error = (EFbAdMobError)Future.error();
		if (Error != EFbAdMobError::None)
		{									
			UE_LOG(LogAdMobBannerView, Error,
				TEXT("%s Code %d. Message: %s"), *MessageOnError, Error,
				UTF8_TO_TCHAR(Future.error_message()));						
		}		

		if (Callback.IsBound())
		{
			if (IsInGameThread())
			{
				Callback.Execute(Error);
			}

			else AsyncTask(ENamedThreads::GameThread, [Callback, Error]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	}
};

#define CreateAdMobCallback(ErrorMessage)								\
	[Callback](const firebase::Future<void>& Future) -> void			\
	{																	\
		ExecuteCallback(Future, Callback, TEXT(ErrorMessage));			\
	}

UBannerView::UBannerView()
	: BannerView(nullptr)
{	

}

UBannerView::UBannerView(FVTableHelper& Helper) 
	: Super(Helper)
	, BannerView(nullptr)
{

}

UBannerView::~UBannerView()
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{
		BannerView->Destroy();
		delete BannerView;
		BannerView = nullptr;
	}
#endif // WITH_FIREBASE_ADMOB
}

void UBannerView::Initialize(const FString& AdUnitId, const FAdSize& Size, const FAdMobCallback& Callback)
{
	UE_LOG(LogAdMob, Log, TEXT("Initializing AdMob Banner View with Ad Unit %s and size {%dx%d}."), *AdUnitId, Size.Width, Size.Height);

#if WITH_FIREBASE_ADMOB
	if (!BannerView)
	{
		BannerView = new firebase::admob::BannerView();
	}

	if (BannerView->InitializeLastResult().status() == firebase::FutureStatus::kFutureStatusComplete)
	{
		UE_LOG(LogAdMobBannerView, Warning, TEXT("Initialize() called for a Banner View that has already been initialized."));
		Callback.ExecuteIfBound(EFbAdMobError::AlreadyInitialized);
		return;
	}

	if (BannerView->InitializeLastResult().status() == firebase::FutureStatus::kFutureStatusPending)
	{
		UE_LOG(LogAdMobBannerView, Warning, TEXT("Initialize() called for a Banner that is being initialized.."));
		Callback.ExecuteIfBound(EFbAdMobError::AlreadyInitialized);
		return;
	}

	firebase::admob::AdSize NativeSize;
	
	NativeSize.ad_size_type = firebase::admob::AdSizeType::kAdSizeStandard;
	NativeSize.height = Size.Height;
	NativeSize.width  = Size.Width;

	const firebase::admob::AdParent Parent = FAdMobHelper::GetAdParent();

	AdUnit = TCHAR_TO_UTF8(*AdUnitId);

	BannerView->Initialize(Parent, AdUnit.c_str(), NativeSize)
		.OnCompletion(CreateAdMobCallback("Failed to initialize Banner view"));
#else // !WITH_FIREBASE_ADMOB
	UE_LOG(LogAdMobBannerView, Warning, TEXT("UBannerView::Initialize() called but AdMob is disabled."));
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void UBannerView::Hide(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{
		BannerView->Hide().OnCompletion(CreateAdMobCallback("Failed to hide Banner View"));
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void UBannerView::Show(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{
		BannerView->Show().OnCompletion(CreateAdMobCallback("Failed to load ad for Banner View"));
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void UBannerView::LoadAd(const FAdMobAdRequest& AdRequest, const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{	
		RequestData = MakeUnique<FAdMobHelper::FInitAdRequestMemoryHandler>(AdRequest);
		BannerView->LoadAd(RequestData->Get())
			.OnCompletion(CreateAdMobCallback("Failed to load ad for Banner View")); 
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void UBannerView::MoveTo(const int32 X, const int32 Y, const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{
		BannerView->MoveTo(X, Y).OnCompletion(CreateAdMobCallback("Failed to Move Banner View"));
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void UBannerView::MoveTo(const FVector2D Location, const FAdMobCallback& Callback)
{
	MoveTo(Location.X, Location.Y, Callback);
}

void UBannerView::MoveTo(const EAdMobBannerViewPosition Location, const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB 
	if (BannerView)
	{
		BannerView->MoveTo((firebase::admob::BannerView::Position)Location)
			.OnCompletion(CreateAdMobCallback("Failed to move Banner View"));
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void UBannerView::Pause(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{
		BannerView->Pause().OnCompletion(CreateAdMobCallback("Failed to pause Banner View")); 
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

void UBannerView::Resume(const FAdMobCallback& Callback)
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{
		BannerView->Resume().OnCompletion(CreateAdMobCallback("Failed to Show Banner View."));
	}
	else
	{
		Callback.ExecuteIfBound(EFbAdMobError::Uninitialized);
	}
#else // !WITH_FIREBASE_ADMOB
	Callback.ExecuteIfBound(EFbAdMobError::None);
#endif // !WITH_FIREBASE_ADMOB
}

FAdMobBoundingBox UBannerView::GetBoundingBox() const
{
#if WITH_FIREBASE_ADMOB
	if (BannerView)
	{
		auto BoundingBox = BannerView->bounding_box();
		return { BoundingBox.height, BoundingBox.width, BoundingBox.x, BoundingBox.y };
	}
#endif
	return FAdMobBoundingBox();
}

#undef CreateAdMobCallback

