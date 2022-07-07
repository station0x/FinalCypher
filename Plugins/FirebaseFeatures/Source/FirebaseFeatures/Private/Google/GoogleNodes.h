// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Auth/Credential.h"
#include "GoogleNodes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDMSignIn, const FString&, Message);

UCLASS()
class UFbGoogleSignInProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDMSignIn SignedIn;
	UPROPERTY(BlueprintAssignable)
	FDMSignIn Failed;

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Google", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Show External Login UI (Android Updated API)", AutoCreateRefTerm = ""))
	static UFbGoogleSignInProxy* SignIn(FString ServerClientID, bool bSkipIfSigned, bool bUseSilentSignIn);

private:
	void OnTaskOver(const bool bSuccess, FString Message);

	FString Token;
	bool bSkipSigned;
	bool bSilentSignIn;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDMReadLdb, const FString&, Message, int32, Score);

UCLASS()
class UGetLeaderboardScoreProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDMReadLdb ScoreRead;
	UPROPERTY(BlueprintAssignable)
	FDMReadLdb Failed;

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Google", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Read Leaderboard Integer (Android Update API)", AutoCreateRefTerm = ""))
	static UGetLeaderboardScoreProxy* GetScore(FString LeaderboardID);

private:
	void OnTaskOver(const bool bSuccess, FString Message, int32 Score);
	
	FString LeaderboardID;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDMSICred, const FCredential&, Credential, const FString&, ErrorMessage);

UCLASS()
class UGoogleSignInCredentialProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDMSICred Success;

	UPROPERTY(BlueprintAssignable)
	FDMSICred Failed;

public:
	virtual void Activate() override;

	UFUNCTION(BlueprintCallable, Category = "Google", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Credential from Google Play Games", AutoCreateRefTerm = ""))
	static UGoogleSignInCredentialProxy* GetCredentialFromPlayGames(FString ServerClientID, bool bUseSilentSignIn = true, bool bSkipIfSigned = false);

	UFUNCTION(BlueprintCallable, Category = "Google", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Credential from Google Sign-In", AutoCreateRefTerm = ""))
	static UGoogleSignInCredentialProxy* GetCredential(FString ServerClientID, bool bUseSilentSignIn = true, bool bSkipIfSigned = false);

private:
	void OnTaskOver(const bool bSuccess, FString Message);
	
	bool bUsePlayGames;
	bool bUseSilentSignIn;
	bool bSkipIfSigned;
	FString Token;
};

