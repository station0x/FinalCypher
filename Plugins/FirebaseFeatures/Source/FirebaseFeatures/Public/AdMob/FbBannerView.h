// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "FbAdMobTypes.h"
#include "CoreMinimal.h"
#include <string>
#include "FbBannerView.generated.h"

namespace firebase { namespace admob { class BannerView; } };

UENUM(BlueprintType)
enum class EAdMobBannerViewPosition : uint8
{
	/// Top of the screen, horizontally centered.
	Top = 0,
	/// Bottom of the screen, horizontally centered.
	Bottom,
	/// Top-left corner of the screen.
	TopLeft,
	/// Top-right corner of the screen.
	TopRight,
	/// Bottom-left corner of the screen.
	BottomLeft,
	/// Bottom-right corner of the screen.
	BottomRight,
};

/// Loads and displays AdMob banner ads.
///
/// Each BannerView object corresponds to a single AdMob banner placement. There
/// are methods to load an ad, move it, show it and hide it, and retrieve the
/// bounds of the ad onscreen.
///
/// BannerView objects maintain a presentation state that indicates whether
/// or not they're currently onscreen, as well as a set of bounds (stored in a
/// BoundingBox struct), but otherwise provide information about
/// their current state through Futures. Methods like @ref Initialize,
/// LoadAd, and Hide each have a corresponding @ref Future from which
/// the result of the last call can be determined. The two variants of
/// @ref MoveTo share a single result @ref Future, since they're essentially the
/// same action.
UCLASS(BlueprintType)
class FIREBASEFEATURES_API UBannerView final : public UObject
{
	GENERATED_BODY()

public:
	UBannerView();
	UBannerView(FVTableHelper& Helper);
	~UBannerView();

	/// Initializes the @ref BannerView object.
	/// @param[in] ad_unit_id The ad unit ID to use when requesting ads.
	/// @param[in] size The desired ad size for the banner.
	void Initialize(const FString& AdUnitId, const FAdSize& Size,	const FAdMobCallback& Callback = FAdMobCallback());

	/// Begins an asynchronous request for an ad. If successful, the ad will
	/// automatically be displayed in the BannerView.
	/// @param[in] request An AdRequest struct with information about the request
	///                    to be made (such as targeting info).
	void LoadAd(const FAdMobAdRequest& AdRequest,				const FAdMobCallback& Callback = FAdMobCallback());
	
	/// Moves the @ref BannerView so that its top-left corner is located at
	/// (x, y). Coordinates are in pixels from the top-left corner of the screen.
	///
	/// When built for Android, the library will not display an ad on top of or
	/// beneath an Activity's status bar. If a call to MoveTo would result in an
	/// overlap, the @ref BannerView is placed just below the status bar, so no
	/// overlap occurs.
	/// @param[in] x The desired horizontal coordinate.
	/// @param[in] y The desired vertical coordinate.
	void MoveTo(const int32 X, const int32 Y,					const FAdMobCallback& Callback = FAdMobCallback());
	void MoveTo(const FVector2D Location,						const FAdMobCallback& Callback = FAdMobCallback());
	
	/// Moves the @ref BannerView so that it's located at the given pre-defined
	/// position.
	/// @param[in] position The pre-defined position to which to move the
	///                     @ref BannerView.
	void MoveTo(const EAdMobBannerViewPosition Location,		const FAdMobCallback& Callback = FAdMobCallback());
	
	/// Hides the BannerView.
	void Hide(const FAdMobCallback& Callback = FAdMobCallback());

	/// Shows the BannerView.
	void Show(const FAdMobCallback& Callback = FAdMobCallback());
	
	/// Pauses the BannerView. Should be called whenever the C++ engine
	/// pauses or the application loses focus.
	void Pause(const FAdMobCallback& Callback = FAdMobCallback());

	/// Resumes the BannerView after pausing.
	void Resume(const FAdMobCallback& Callback = FAdMobCallback());

	/// Retrieves the BannerView's current onscreen size and location.
	/// @return The current size and location. Values are in pixels, and location
	///         coordinates originate from the top-left corner of the screen.
	UFUNCTION(BlueprintPure, Category = "AdMob|BannerView")
	UPARAM(DisplayName = "Bounding Box") FAdMobBoundingBox GetBoundingBox() const;

private:
	std::string AdUnit;

	firebase::admob::BannerView* BannerView;

	FAdMobAdRequestDataPtr RequestData;
};

