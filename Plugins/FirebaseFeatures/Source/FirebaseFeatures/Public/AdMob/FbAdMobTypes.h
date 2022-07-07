// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FbAdMobTypes.generated.h"

UENUM(BlueprintType)
enum class EFbAdMobError : uint8
{
	/// Call completed successfully.
	None,
	/// The ad has not been fully initialized.
	Uninitialized,
	/// The ad is already initialized (repeat call).
	AlreadyInitialized,
	/// A call has failed because an ad is currently loading.
	LoadInProgress,
	/// A call to load an ad has failed due to an internal SDK error.
	InternalError,
	/// A call to load an ad has failed due to an invalid request.
	InvalidRequest,
	/// A call to load an ad has failed due to a network error.
	NetworkError,
	/// A call to load an ad has failed because no ad was available to serve.
	NoFill,
	/// An attempt has been made to show an ad on an Android Activity that has
	/// no window token (such as one that's not done initializing).
	NoWindowToken,
};

/// The possible presentation states for rewarded video.
UENUM(BlueprintType)
enum class EAdMobPresentationState : uint8
{
	/// No ad is currently being shown.
	Hidden = 0,
	/// A rewarded video ad is completely covering the screen or has caused
	/// focus to leave the application (for example, when opening an external
	/// browser during a clickthrough), but the video associated with the ad has
	/// yet to begin playing.
	CoveringUI,
	/// All of the above conditions are true *except* that the video associated
	/// with the ad began playing at some point in the past.
	VideoHasStarted,
	/// The rewarded video has played and completed.
	VideoHasCompleted,
};

UENUM(BlueprintType)
enum class EAdMobGender : uint8
{
	/// The gender of the current user is unknown or unspecified by the publisher.
	Unknown = 0,
	/// The current user is known to be male.
	Male,
	/// The current user is known to be female.
	Female
};

/// @brief Indicates whether an ad request is considered tagged for
/// child-directed treatment.
UENUM(BlueprintType)
enum class EAdMobChildDirectedTreatmentState : uint8
{
	/// The child-directed status for the request is not indicated.
	Unknown = 0,
	/// The request is tagged for child-directed treatment.
	Tagged,
	/// The request is not tagged for child-directed treatment.
	NotTagged
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FAdSize
{
	GENERATED_BODY()
public:
	/// Height of the ad (in points or dp).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 Height = 80;

	/// Width of the ad (in points or dp).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 Width = 300;
};

// The information needed to request an ad.
USTRUCT(BlueprintType)
struct FAdMobBoundingBox 
{
	GENERATED_BODY()
public:
	FAdMobBoundingBox() : Height(0), Width(0), X(0), Y(0) {}
	FAdMobBoundingBox(int32 InHeight, int32 InWidth, int32 InX, int32 InY) 
		: Height(InHeight), Width(InWidth), X(InX), Y(InY) {}

	/// Height of the ad in pixels.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 Height;
	
	/// Width of the ad in pixels.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 Width;

	/// Horizontal position of the ad in pixels from the left.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 X;

	/// Vertical position of the ad in pixels from the top.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 Y;
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FAdMobAdRequest
{
	GENERATED_BODY()
public:
	/// An array of test device IDs specifying devices that test ads will be
	/// returned for.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	TArray<FString> TestDeviceIds;

	/// An array of keywords or phrases describing the current user activity, such
	/// as "Sports Scores" or "Football."
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	TArray<FString> Keywords;
	
	/// A @ref KeyValuePair specifying additional parameters accepted by an ad
	/// network.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	TMap<FString, FString> Extras;
	
	/// The day the user was born. Specify the user's birthday to increase ad
	/// relevancy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 BirthdayDay = 0;
	
	/// The month the user was born. Specify the user's birthday to increase ad
	/// relevancy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 BirthdayMonth = 0;
	
	/// The year the user was born. Specify the user's birthday to increase ad
	/// relevancy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	int32 BirthdayYear = 0;
	
	/// The user's @ref Gender. Specify the user's gender to increase ad
	/// relevancy.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	EAdMobGender Gender = EAdMobGender::Unknown;
	
	/// Specifies whether the request should be considered as child-directed for
	/// purposes of the Children's Online Privacy Protection Act (COPPA).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	EAdMobChildDirectedTreatmentState TaggedForChildDirectedTreatment = EAdMobChildDirectedTreatmentState::Unknown;
};


USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FAdMobRewardItem
{
	GENERATED_BODY()
public:
	/// The reward amount.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	float Amount = 0.f;
	
	/// A string description of the type of reward (such as "coins" or "points").
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AdMob")
	FString RewardType;
};

DECLARE_DELEGATE_OneParam(FAdMobCallback, const EFbAdMobError /* Error */);


namespace FAdMobHelper
{
	class FInitAdRequestMemoryHandler;
}

using FAdMobAdRequestDataPtr = TUniquePtr<FAdMobHelper::FInitAdRequestMemoryHandler>;
