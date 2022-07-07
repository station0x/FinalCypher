// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "FirebaseSdk/FirebaseErrors.h"
#include "Auth/Auth.h"
#include "Auth/Credential.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "CredentialNodes.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGetCredentialEvent, const EFirebaseAuthError, Error, const FCredential&, Credential);

UCLASS()
class UGetCredentialFromGameCenterProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FGetCredentialEvent OnSuccess;

	UPROPERTY(BlueprintAssignable)
	FGetCredentialEvent OnError;

public:
	/// Generate a credential from GameCenter for the current user.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|Credential|Game Center", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Credential from Game Center"))
	static UGetCredentialFromGameCenterProxy* GetCredentialFromGameCenter();

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, const FCredential& Credential);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FVerifyPhoneNumberCallback, const FCredential&, Credential, const FString&, VerificationId, const FForceResendingToken&, ResendingToken, const FString&, Error);

UCLASS()
class UVerifyPhoneNumberProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FVerifyPhoneNumberCallback OnVerificationCompleted;
	
	UPROPERTY(BlueprintAssignable)
	FVerifyPhoneNumberCallback OnVerificationFailed;

	UPROPERTY(BlueprintAssignable)
	FVerifyPhoneNumberCallback OnCodeSent;

	UPROPERTY(BlueprintAssignable)
	FVerifyPhoneNumberCallback OnCodeRetrievalTimeout;

public:
	/// Start the phone number authentication operation.
	/// @param[in] PhoneNumber The phone number identifier supplied by the user.
	///    Its format is normalized on the server, so it can be in any format
	///    here.
	/// @param[in] AutoVerifyTimeoutMilliseconds The time out for SMS auto retrieval, in
	///    miliseconds. Currently SMS auto retrieval is only supported on Android.
	///    If 0, do not do SMS auto retrieval.
	///    If positive, try to auto-retrieve the SMS verification code.
	///    If larger than kMaxTimeoutMs, clamped to kMaxTimeoutMs.
	///    When the time out is exceeded, OnCodeAutoRetrievalTimeOut
	///    is called.
	/// @param[in] ForceResendingToken If default, assume this is a new phone
	///    number to verify. If not-default, bypass the verification session deduping
	///    and force resending a new SMS.
	///    This token is received in OnCodeSent callback.
	///    This should only be used when the user presses a Resend SMS button.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|Credential|Phone", meta = (AutoCreateRefTerm = "ForceResendingToken", BlueprintInternalUseOnly = "true", DisplayName = "Verify Phone Number"))
	static UVerifyPhoneNumberProxy* VerifyPhoneNumber(const FString& PhoneNumber, const int64 AutoVerifyTimeoutInMilliseconds, const FForceResendingToken& ForceResendingToken);

	virtual void Activate();

private:
	void Credential	(const FCredential& Credential);
	void Timeout	(const FString& VerifId);
	void Failed		(const FString& Error);
	void Sent		(const FString& VerifId, const FForceResendingToken& Token);

private:
	FString				 PhoneNumber;
	int64				 AutoVerifyTimeoutInMilliseconds;
	FForceResendingToken ForceResendingToken;
};