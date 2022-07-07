// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdMob/FbBannerView.h"
#include "AdMob/FbAdMobTypes.h"
#include "AdMob/FbRewardedVideo.h"
#include "AdMob/FbInterstitialAd.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "FbAdMobNodes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynMultAdMobCallback, const EFbAdMobError, Error);

UCLASS()
class UShowRewardedVideo final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnVideoShown;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnError;

public:
	UShowRewardedVideo() {};

	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Rewarded Video", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Show - Rewarded Video"))
	static UShowRewardedVideo* ShowRewardedVideo();

private:
	void OnActionOver(const EFbAdMobError Error);
};

UCLASS()
class ULoadRewardedVideoAd final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnAdLoaded;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnError;

public:
	ULoadRewardedVideoAd() {};

	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Rewarded Video", meta = (AutoCreateRefTerms="AdRequest", BlueprintInternalUseOnly = "true", DisplayName = "Load Ad - Rewarded Video"))
	static ULoadRewardedVideoAd* LoadRewardedVideoAd(const FString& AdUnitId, const FAdMobAdRequest& AdRequest);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	FString AdUnitId;
	FAdMobAdRequest AdRequest;
};

UCLASS()
class UPauseRewardedVideo final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnVideoPaused;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnError;

public:
	UPauseRewardedVideo() {};

	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Rewarded Video", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Pause - Rewarded Video"))
	static UPauseRewardedVideo* PauseRewardedVideo();

private:
	void OnActionOver(const EFbAdMobError Error);
};

UCLASS()
class UResumeRewardedVideo final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnVideoResumed;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultAdMobCallback OnError;

public:
	UResumeRewardedVideo() {};

	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Rewarded Video", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Resume - Rewarded Video"))
	static UResumeRewardedVideo* ResumeRewardedVideo();

private:
	void OnActionOver(const EFbAdMobError Error);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultCreateBanner, UBannerView*, BannerView, const EFbAdMobError, Error);

UCLASS()
class UCreateBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY() 
public:
	/**
	 * Called when the banner view has been created and initialized.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnBannerViewCreated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Create Banner View"))
	static UCreateBannerView* CreateBannerView(const FString& AdUnitId, const FAdSize& AdSize);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;

	FString AdUnitId;
	FAdSize Size;
};

UCLASS()
class ULoadAdBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnAdLoaded;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Load Ad"))
	static ULoadAdBannerView* LoadAd(UBannerView* const BannerView, const FAdMobAdRequest& AdRequest);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;

	FAdMobAdRequest AdRequest;
};

UCLASS()
class UHideAdBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnHidden;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Hide"))
	static UHideAdBannerView* Hide(UBannerView* const BannerView);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;
};

UCLASS()
class UShowBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnShown;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Show"))
	static UShowBannerView* Show(UBannerView* const BannerView);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;
};


UCLASS()
class UPauseBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnPaused;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Pause"))
	static UPauseBannerView* Pause(UBannerView* const BannerView);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;
};

UCLASS()
class UResumeBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnResumed;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Resume"))
	static UResumeBannerView* Resume(UBannerView* const BannerView);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;
};


UCLASS()
class UMoveToPositionBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnMoved;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Move to Position"))
	static UMoveToPositionBannerView* MoveTo(UBannerView* const BannerView, EAdMobBannerViewPosition Position);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;

	EAdMobBannerViewPosition Position;
};

UCLASS()
class UMoveToBannerView final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnMoved;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateBanner OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Banner View", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Move To"))
	static UMoveToBannerView* MoveTo(UBannerView* const BannerView, const int32 X, const int32 Y);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UBannerView* Banner;

	int32 X, Y;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultCreateInterstitial, UInterstitialAd*, InterstitialAd, const EFbAdMobError, Error);

UCLASS()
class UCreateInterstitialAd final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY() 
public:
	/**
	 * Called when the interstitial ad has been created and initialized.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateInterstitial OnInterstitialAdCreated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateInterstitial OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Interstitial Ad", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Create Interstitial Ad"))
	static UCreateInterstitialAd* CreateInterstitialAd(const FString& AdUnitId);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UInterstitialAd* Interstitial;

	FString AdUnitId;
};

UCLASS()
class ULoadAdInterstitial final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateInterstitial OnAdLoaded;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateInterstitial OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Interstitial Ad", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Load Ad"))
	static ULoadAdInterstitial* LoadAd(UInterstitialAd* const InterstitialAd, const FAdMobAdRequest& AdRequest);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UInterstitialAd* Interstitial;

	FAdMobAdRequest AdRequest;
};

UCLASS()
class UShowInterstitial final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateInterstitial OnShown;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultCreateInterstitial OnError;

public:
	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|AdMob|Interstitial Ad", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Show"))
	static UShowInterstitial* Show(UInterstitialAd* const InterstitialAd);

private:
	void OnActionOver(const EFbAdMobError Error);

private:
	UPROPERTY()
	UInterstitialAd* Interstitial;
};

