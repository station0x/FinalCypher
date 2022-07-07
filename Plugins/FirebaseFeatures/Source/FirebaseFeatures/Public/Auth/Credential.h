// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Auth.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Credential.generated.h"

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FForceResendingToken
{
	GENERATED_BODY()
private:
	friend class UCredentialLibrary;
	friend class FPhoneProviderListener;

	FForceResendingToken(void* const InToken);

public:
	FForceResendingToken();
	FForceResendingToken(const FForceResendingToken&  Other);
	FForceResendingToken(FForceResendingToken&& Other);
	FForceResendingToken& operator=(const FForceResendingToken& Other);
	~FForceResendingToken();

private:
	void* Token;
};

DECLARE_DELEGATE_TwoParams(FGetCredentialCallback, const EFirebaseAuthError /* Error */, const FCredential& /* Credential */);
DECLARE_DELEGATE_FourParams(FGetAppleSignInCredentialCallback, bool /* bSuccess */, FCredential /* Credential */, int32 /* Error */, FString /* ErrorMessage */);

/**
 * Delegates for VerifyPhoneNumber.
 */
DECLARE_DELEGATE_OneParam(FCredentialCallback, const FCredential& /* Credential */);
DECLARE_DELEGATE_OneParam(FCodeAutoRetrievalTimeoutCallback, const FString& /* VerificationId */);
DECLARE_DELEGATE_OneParam(FVerificationFailedCallback, const FString& /* Error */);
DECLARE_DELEGATE_TwoParams(FCodeSentCallback, const FString& /* VerificationId */, const FForceResendingToken& /* ForceResendingToken */);

UCLASS()
class FIREBASEFEATURES_API UCredentialLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// Generate a credential from the given email and password.
	/// @param email E-mail to generate the credential from.
	/// @param password Password to use for the new credential.
	/// @returns New Credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|Email")
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromEmail(const FString& Email, const FString& Password);

	/// Generate a credential from the given Facebook token.
	/// @param access_token Facebook token to generate the credential from.
	/// @returns New Credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|Facebook", meta=(Keywords = "Facebook Provider Auth Login"))
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromFacebook(const FString& AccessToken);

	/// Generate a credential from GameCenter for the current user.
	/// @return a Callback that will be called with the resulting credential.
	static void GetCredentialFromGameCenter(FGetCredentialCallback Callback);

	/// Tests to see if the current user is signed in to GameCenter.
	/// @return true if the user is signed in, false otherwise.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|Game Center", meta = (Keywords = "Game Center Provider Auth"))
	static UPARAM(DisplayName = "Is Authenticated") bool IsAuthenticatedWithGameCenter();

	/// Generate a credential from the given GitHub token.
	/// @param Token The GitHub OAuth access token.
	/// @returns New Credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|GitHub", meta = (Keywords = "Github Provider Auth"))
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromGitHub(const FString& Token);

	/// Generate a credential from the given Google ID token and/or access token.
	/// @param IdToken Google Sign-In ID token.
	/// @param AccessToken Google Sign-In access token.
	/// @returns New Credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|Google", meta = (Keywords = "Google Provider Auth"))
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromGoogle(const FString& Token, const FString& AccessToken);

	/// Generate a credential for an OAuth2 provider.
	/// @param ProviderId Name of the OAuth2 provider
	/// @param id_token The authentication token (OIDC only).
	/// @param access_token The authentication access token.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|OAuth")
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromOAuth(const FString& ProviderId, const FString& IdToken, const FString& AccessToken);

	/// Generate a credential for an OAuth2 provider.
	/// @param ProviderId Name of the OAuth2 provider.
	/// @param IdToken The authentication token (OIDC only).
	/// @param Nonce The raw nonce associated with the Auth credential being
	/// created.
	/// @param AccessToken The access token associated with the Auth credential
	/// to be created, if available. 
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|OAuth")
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromOAuthWithOnce(const FString& ProviderId, const FString& IdToken, const FString& Nonce, const FString& AccessToken);

	/// Generate a credential from the given Server Auth Code.
	/// @param ServerAuth_code Play Games Sign in Server Auth Code.
	/// @return New Credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|PlayGames", meta = (Keywords = "Play Games Provider Auth"))
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromPlayGames(const FString& ServerAuthCode);


	/// Generate a credential from the given Twitter token and password.
	/// @param Token The Twitter OAuth token.
	/// @param Secret The Twitter OAuth secret.
	/// @return New Credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|Twitter", meta = (Keywords = "Twitter Provider Auth"))
	static UPARAM(DisplayName = "Credential") FCredential GetCredentialFromTwitter(const FString& Token, const FString& Secret);

	/// Generates a credential from Apple Sign-In
	/// @param Nonce The nonce used during the sign-in process.
	/// @param Callback Callback called when the operation is over.
	static void GetCredentialFromAppleSignIn(FString Nonce, FGetAppleSignInCredentialCallback Callback);

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
	static void VerifyPhoneNumber
	(
		const FString&							PhoneNumber, 
		const int64								AutoVerifyTimeoutMilliseconds,
		const FCredentialCallback&				OnVerificationCompleted,
		const FVerificationFailedCallback&		OnVerificationFailed       = FVerificationFailedCallback(),
		const FCodeSentCallback&				OnCodeSent				   = FCodeSentCallback(),
		const FCodeAutoRetrievalTimeoutCallback OnCodeAutoRetrievalTimeout = FCodeAutoRetrievalTimeoutCallback(),
		const FForceResendingToken&				ForceResendingToken		   = FForceResendingToken()
	);


	/// Generate a credential for the given phone number.
	///
	/// @param[in] VerificationId The id returned when sending the verification
	///    code. Sent to the caller via VerifyPhoneNumber::OnCodeSent.
	/// @param[in] VerificationCode The verification code supplied by the user,
	///    most likely by a GUI where the user manually enters the code
	///    received in the SMS sent by @ref VerifyPhoneNumber.
	/// @returns New Credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential|Phone")
	static UPARAM(DisplayName = "Credential") FCredential GetCredential(const FString& VerificationId, const FString& VerificationCode);
};


