// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GoogleServices.generated.h"

DECLARE_DELEGATE_TwoParams(FGoogleSignInCallback, bool /* bSuccess */, FString /* Message */);

DECLARE_DELEGATE_ThreeParams(FGetLeaderboardScoreCallback, bool /* bSuccess */, FString /* Message */, int32 /* Score */);

UCLASS()
class FIREBASEFEATURES_API UGoogleServicesLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/**
	 * Signs-In with Google Sign-In, using the new API.
	 * @param bSkipIfSigned If we should avoid signing in again if the user is already signed in.
	 * @param bUseSilentSignIn If we should try to use silent sign in.
	 * @param bUsePlayGames (Android only) If Google Play Games must be used instead of Google Sign-In.
	*/
	static void SignIn(const FString& ServerClientID, const bool bSkipIfSigned, const bool bUseSilentSignIn, const bool bUsePlayGames, FGoogleSignInCallback Callback);
	static void SignIn(const FString& ServerClientID, const bool bSkipIfSigned, const bool bUseSilentSignIn, FGoogleSignInCallback Callback);
	static void SignIn(FGoogleSignInCallback Callback);

	/**
	 * Checks if the user is currently signed in.
	 * @return True if the user is currently signed in.
	*/
	UFUNCTION(BlueprintCallable, Category = "Google", Meta = (DisplayName = "Is Signed (Android Updated API)"))
	static UPARAM(DisplayName = "Is Signed") bool IsSignedIn();

	/**
	 * Signs out from the Google Sign-In SDK.
	*/
	UFUNCTION(BlueprintCallable, Category = "Google", Meta = (DisplayName = "Sign Out of Google"))
	static void SignOut();

	/**
	 * Submits a new score.
	 * @param LeaderboardID The leaderboard where the score should be submitted.
	 * @param Score The new score submitted.
	*/
	UFUNCTION(BlueprintCallable, Category = "Google", Meta = (DisplayName = "Write Leaderboard Integer (Android Updated API)"))
	static void SubmitScore(const FString& LeaderboardID, int64 Score);

	/**
	 * Shows the leaderboard for the specified ID. The user has to be signed first.
	 * @param LeaderboardID The ID of the leaderboard to show.
	*/
	UFUNCTION(BlueprintCallable, Category = "Google", Meta = (DisplayName = "Show Leaderboard (Android Updated API)"))
	static void ShowLeaderboard(const FString& LeaderboardID);

	/**
	 * Gets the leaderboard's score.
	 * @param LeaderboardID The ID of the leaderboard to get the score from.
	*/
	static void GetLeaderboardScore(const FString& LeaderboardID, FGetLeaderboardScoreCallback Callback);

	/**
	 * Returns an ID token that you can send to your server.
	*/
	UFUNCTION(BlueprintCallable, Category = "Google")
	static UPARAM(DisplayName = "ID Token") FString GetIdToken();

	UFUNCTION(BlueprintCallable, Category = "Google")
	static UPARAM(DisplayName = "Access Token") FString GetAccessToken();

	/**
	 * Returns a one-time server auth code to send to your web server which can be exchanged for access token
	*/
	UFUNCTION(BlueprintCallable, Category = "Google")
	static UPARAM(DisplayName = "Server Auth Code") FString GetServerAuthCode();

	UFUNCTION(BlueprintCallable, Category = "Google", Meta = (DisplayName = "Show Platform Specific Achievements Screen (Android Updated API)"))
	static void ShowAchievements();
};


