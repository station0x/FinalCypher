// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "FbAdMobNodes.h"

UShowRewardedVideo* UShowRewardedVideo::ShowRewardedVideo()
{
	return NewObject<UShowRewardedVideo>();
}

void UShowRewardedVideo::Activate()
{
	FRewardedVideo::Show(FAdMobCallback::CreateUObject(this, &UShowRewardedVideo::OnActionOver));
}

void UShowRewardedVideo::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnVideoShown : OnError).Broadcast(Error);
	SetReadyToDestroy();
}

ULoadRewardedVideoAd* ULoadRewardedVideoAd::LoadRewardedVideoAd(const FString& AdUnitId, const FAdMobAdRequest& AdRequest)
{
	ULoadRewardedVideoAd* const Proxy = NewObject<ULoadRewardedVideoAd>();

	Proxy->AdRequest = AdRequest;
	Proxy->AdUnitId  = AdUnitId;

	return Proxy;
}

void ULoadRewardedVideoAd::Activate()
{
	FRewardedVideo::LoadAd(AdUnitId, AdRequest, FAdMobCallback::CreateUObject(this, &ULoadRewardedVideoAd::OnActionOver));
}

void ULoadRewardedVideoAd::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnAdLoaded : OnError).Broadcast(Error);
	SetReadyToDestroy();
}

UPauseRewardedVideo* UPauseRewardedVideo::PauseRewardedVideo()
{
	return NewObject<UPauseRewardedVideo>();
}

void UPauseRewardedVideo::Activate()
{
	FRewardedVideo::Pause(FAdMobCallback::CreateUObject(this, &UPauseRewardedVideo::OnActionOver));
}

void UPauseRewardedVideo::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnVideoPaused : OnError).Broadcast(Error);
	SetReadyToDestroy();
}

UResumeRewardedVideo* UResumeRewardedVideo::ResumeRewardedVideo()
{
	return NewObject<UResumeRewardedVideo>();
}

void UResumeRewardedVideo::Activate()
{
	FRewardedVideo::Resume(FAdMobCallback::CreateUObject(this, &UResumeRewardedVideo::OnActionOver));
}

void UResumeRewardedVideo::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnVideoResumed : OnError).Broadcast(Error);
	SetReadyToDestroy();
}

UCreateBannerView* UCreateBannerView::CreateBannerView(const FString& AdUnitId, const FAdSize& AdSize)
{
	UCreateBannerView* const Proxy = NewObject<UCreateBannerView>();

	Proxy->Size		= AdSize;
	Proxy->AdUnitId = AdUnitId;

	return Proxy;
}

void UCreateBannerView::Activate()
{
	Banner = NewObject<UBannerView>();
	Banner->Initialize(AdUnitId, Size, FAdMobCallback::CreateUObject(this, &UCreateBannerView::OnActionOver));
}

void UCreateBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnBannerViewCreated : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}

ULoadAdBannerView* ULoadAdBannerView::LoadAd(UBannerView* const BannerView, const FAdMobAdRequest& AdRequest)
{
	ULoadAdBannerView* const Proxy = NewObject<ULoadAdBannerView>();

	Proxy->Banner = BannerView;
	Proxy->AdRequest = AdRequest;

	return Proxy;
}

void ULoadAdBannerView::Activate()
{
	if (!Banner)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}

	Banner->LoadAd(AdRequest, FAdMobCallback::CreateUObject(this, &ULoadAdBannerView::OnActionOver));
}

void ULoadAdBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnAdLoaded : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}

UHideAdBannerView* UHideAdBannerView::Hide(UBannerView* const BannerView)
{
	UHideAdBannerView* const Proxy = NewObject<UHideAdBannerView>();

	Proxy->Banner = BannerView;

	return Proxy;
}

void UHideAdBannerView::Activate()
{
	if (!Banner)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}

	Banner->Hide(FAdMobCallback::CreateUObject(this, &UHideAdBannerView::OnActionOver));
}

void UHideAdBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnHidden : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}

UShowBannerView* UShowBannerView::Show(UBannerView* const BannerView)
{
	UShowBannerView* const Proxy = NewObject<UShowBannerView>();

	Proxy->Banner = BannerView;

	return Proxy;
}

void UShowBannerView::Activate()
{
	if (!Banner)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}

	Banner->Show(FAdMobCallback::CreateUObject(this, &UShowBannerView::OnActionOver));
}

void UShowBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnShown : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}

UPauseBannerView* UPauseBannerView::Pause(UBannerView* const BannerView)
{
	UPauseBannerView* const Proxy = NewObject<UPauseBannerView>();

	Proxy->Banner = BannerView;

	return Proxy;
}

void UPauseBannerView::Activate()
{
	if (!Banner)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}

	Banner->Pause(FAdMobCallback::CreateUObject(this, &UPauseBannerView::OnActionOver));
}

void UPauseBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnPaused : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}

UResumeBannerView* UResumeBannerView::Resume(UBannerView* const BannerView)
{
	UResumeBannerView* const Proxy = NewObject<UResumeBannerView>();

	Proxy->Banner = BannerView;

	return Proxy;
}

void UResumeBannerView::Activate()
{
	if (!Banner)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}

	Banner->Resume(FAdMobCallback::CreateUObject(this, &UResumeBannerView::OnActionOver));
}

void UResumeBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnResumed : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}

UMoveToPositionBannerView* UMoveToPositionBannerView::MoveTo(UBannerView* const BannerView, EAdMobBannerViewPosition Position)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Banner = BannerView;
	Proxy->Position = Position;

	return Proxy;
}

void UMoveToPositionBannerView::Activate()
{
	if (!Banner)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}

	Banner->MoveTo(Position, FAdMobCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UMoveToPositionBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnMoved : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}

UMoveToBannerView* UMoveToBannerView::MoveTo(UBannerView* const BannerView, const int32 X, const int32 Y)
{
	UMoveToBannerView* const Proxy = NewObject<UMoveToBannerView>();

	Proxy->Banner = BannerView;
	Proxy->X = X;
	Proxy->Y = Y;

	return Proxy;
}

void UMoveToBannerView::Activate()
{
	if (!Banner)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}

	Banner->MoveTo(X, Y, FAdMobCallback::CreateUObject(this, &UMoveToBannerView::OnActionOver));
}

void UMoveToBannerView::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnMoved : OnError).Broadcast(Banner, Error);
	SetReadyToDestroy();
}


UCreateInterstitialAd* UCreateInterstitialAd::CreateInterstitialAd(const FString& AdUnitId)
{
	UCreateInterstitialAd* const Proxy = NewObject<UCreateInterstitialAd>();

	Proxy->AdUnitId = AdUnitId;

	return Proxy;
}

void UCreateInterstitialAd::Activate()
{
	Interstitial = NewObject<UInterstitialAd>();

	Interstitial->Initialize(AdUnitId, FAdMobCallback::CreateUObject(this, &UCreateInterstitialAd::OnActionOver));
}

void UCreateInterstitialAd::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnInterstitialAdCreated : OnError).Broadcast(Interstitial, Error);
	SetReadyToDestroy();
}

ULoadAdInterstitial* ULoadAdInterstitial::LoadAd(UInterstitialAd* const InterstitialAd, const FAdMobAdRequest& AdRequest)
{
	ULoadAdInterstitial* const Proxy = NewObject<ULoadAdInterstitial>();

	Proxy->AdRequest	= AdRequest;
	Proxy->Interstitial = InterstitialAd;

	return Proxy;
}

void ULoadAdInterstitial::Activate()
{
	if (!Interstitial)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}
	Interstitial->LoadAd(AdRequest, FAdMobCallback::CreateUObject(this, &ULoadAdInterstitial::OnActionOver));
}

void ULoadAdInterstitial::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnAdLoaded : OnError).Broadcast(Interstitial, Error);
	SetReadyToDestroy();
}

UShowInterstitial* UShowInterstitial::Show(UInterstitialAd* const InterstitialAd)
{
	UShowInterstitial* const Proxy = NewObject<UShowInterstitial>();

	Proxy->Interstitial = InterstitialAd;

	return Proxy;
}

void UShowInterstitial::Activate()
{
	if (!Interstitial)
	{
		OnActionOver(EFbAdMobError::Uninitialized);
		return;
	}
	Interstitial->Show(FAdMobCallback::CreateUObject(this, &UShowInterstitial::OnActionOver));
}

void UShowInterstitial::OnActionOver(const EFbAdMobError Error)
{
	(Error == EFbAdMobError::None ? OnShown : OnError).Broadcast(Interstitial, Error);
	SetReadyToDestroy();
}

