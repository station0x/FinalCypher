// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once


#include "Auth/User.h"
#include "Auth/Auth.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "UserNodes.generated.h"

UCLASS(Abstract)
class UUserNodeBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY()
	UUser* _User;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultStringCallback, const EFirebaseAuthError, Error, const FString&, Token);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FDynMultVoidCallback, const EFirebaseAuthError, Error);

UCLASS()
class UGetTokenProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:

	UPROPERTY(BlueprintAssignable)
	FDynMultStringCallback OnTokenReceived;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultStringCallback OnError;

public:

	/// The Java Web Token (JWT) that can be used to identify the user to
	/// the backend.
	/// If a current ID token is still believed to be valid (i.e. it has not yet
	/// expired), that token will be returned immediately.
	/// A developer may set the optional bForceRefresh flag to get a new ID token,
	/// whether or not the existing token has expired. For example, a developer
	/// may use this when they have discovered that the token is invalid for some
	/// other reason.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Token"))
	static UGetTokenProxy* GetToken(UUser* const User, const bool bForceRefresh);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, const FString& Token);

private:
	bool bForceRefresh;
};

UCLASS()
class UUpdateEmailProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnEmailUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnError;

public:
	/// Sets the email address for the user.
	/// May fail if there is already an email/password-based account for the same
	/// email address.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Update Email"))
	static UUpdateEmailProxy* UpdateEmail(UUser* const User, const FString& Email);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error);

private:
	FString Email;
};

UCLASS()
class UUpdatePasswordProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnPasswordUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnError;

public:
	/// Attempts to change the password for the current user.
	/// For an account linked to an Identity Provider (IDP) with no password,
	/// this will result in the account becoming an email/password-based account
	/// while maintaining the IDP link. May fail if the password is invalid,
	/// if there is a conflicting email/password-based account, or if the token
	/// has expired.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Update Password"))
	static UUpdatePasswordProxy* UpdatePassword(UUser* const User, const FString& Password);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error);

private:
	FString Password;
};

UCLASS()
class UReauthenticateProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnReauthenticated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnError;

public:
	/// Convenience function for ReauthenticateAndRetrieveData that discards
	/// the returned AdditionalUserInfo data.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Reauthenticate"))
	static UReauthenticateProxy* Reauthenticate(UUser* const User, const FCredential& Credential);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error);

private:
	FCredential Credential;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultSignInCallback, const EFirebaseAuthError, Error, const FSignInResult&, Result);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultUserCallback,   const EFirebaseAuthError, Error, UUser*, User);

UCLASS()
class UReauthenticateAndRetrieveDataProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnReauthenticated;

	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnError;

public:
	/// Reauthenticate using a credential.
	/// Data from the Identity Provider used to sign-in is returned in the
	/// AdditionalUserInfo inside the returned SignInResult.
	/// Returns an error if the existing credential is not for this user
	/// or if sign-in with that credential failed. The user should remain
	/// signed in even if this method failed. If the developer had held
	/// a reference to that user, the reference will continue to be valid
	/// after this operation.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Reauthenticate and Retrieve Data"))
	static UReauthenticateAndRetrieveDataProxy* ReauthenticateAndRetrieveData(UUser* const User, const FCredential& Credential);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result);

private:
	FCredential Credential;
};

UCLASS()
class UReauthenticateWithProviderProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnReauthenticated;

	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnError;

public:
	/// @brief Re-authenticates the user with a federated auth provider.
	/// @param[in] Provider Contains information on the auth provider to
	/// authenticate with.
	/// @return A SignInResult via callback with the result of the re-authentication
	/// request.
	/// @note: This operation is supported only on iOS and Android platforms. On
	/// non-mobile platforms this method will return a Future with a preset error
	/// code: kAuthErrorUnimplemented.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Reauthenticate with Provider"))
	static UReauthenticateWithProviderProxy* ReauthenticateWithProvider(UUser* const User, const FFederatedAuthProvider& Provider);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result);

private:
	FFederatedAuthProvider Provider;
};

UCLASS()
class USendEmailVerificationProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnEmailSent;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnError;

public:
	/// Initiates email verification for the user.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Send Email Verification"))
	static USendEmailVerificationProxy* SendEmailVerification(UUser* const User);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error);
};

UCLASS()
class UUpdateUserProfileProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnProfileUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnError;

public:
	/// Updates a subset of user profile information.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Update User Profile"))
	static UUpdateUserProfileProxy* UpdateUserProfile(UUser* const User, const FUserProfile& Profile);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error);

private:
	FUserProfile Profile;
};

UCLASS()
class ULinkWithCredentialProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultUserCallback OnLinked;

	UPROPERTY(BlueprintAssignable)
	FDynMultUserCallback OnError;

public:
	/// Convenience function for ReauthenticateAndRetrieveData that discards
	/// the returned AdditionalUserInfo in SignInResult.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Link With Credential"))
	static ULinkWithCredentialProxy* LinkWithCredential(UUser* const User, const FCredential& Credential);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);

private:
	FCredential Credential;
};


UCLASS()
class ULinkAndRetrieveDataWithCredentialProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnLinked;

	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnError;

public:
	/// Links the user with the given 3rd party credentials.
	/// For example, a Facebook login access token, a Twitter token/token-secret
	/// pair.
	/// Status will be an error if the token is invalid, expired, or otherwise
	/// not accepted by the server as well as if the given 3rd party
	/// user id is already linked with another user account or if the current user
	/// is already linked with another id from the same provider.
	/// Data from the Identity Provider used to sign-in is returned in the
	/// AdditionalUserInfo inside SignInResult.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Link and Retrieve Data with Credential"))
	static ULinkAndRetrieveDataWithCredentialProxy* LinkAndRetrieveDataWithCredential(UUser* const User, const FCredential& Credential);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result);

private:
	FCredential Credential;
};

UCLASS()
class ULinkWithProviderProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnLinked;

	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback OnError;

public:
	/// Links this user with a federated auth provider.
	/// @param[in] Provider Contains information on the auth provider to link
	/// with.
	/// @return A SignInResult via callback with the user data result of the link
	/// request.
	/// @note: This operation is supported only on iOS and Android platforms. On
	/// non-mobile platforms this method will return a Future with a preset error
	/// code: kAuthErrorUnimplemented.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Link with Provider"))
	static ULinkWithProviderProxy* LinkWithProvider(UUser* const User, const FFederatedAuthProvider& Provider);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result);

private:
	FFederatedAuthProvider Provider;
};

UCLASS()
class UUnlinkProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultUserCallback OnUnlinked;

	UPROPERTY(BlueprintAssignable)
	FDynMultUserCallback OnError;

public:
	/// Convenience function for ReauthenticateAndRetrieveData that discards
	/// the returned AdditionalUserInfo in SignInResult.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Unlink"))
	static UUnlinkProxy* Unlink(UUser* const User, const FString& Provider);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);

private:
	FString Provider;
};

UCLASS()
class UUpdatePhoneNumberCredentialProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultUserCallback OnUpdated;

	UPROPERTY(BlueprintAssignable)
	FDynMultUserCallback OnError;

public:
	/// Updates the currently linked phone number on the user.
	/// This is useful when a user wants to change their phone number. It is a
	/// shortcut to calling Unlink(phone_credential.provider()) and then
	/// LinkWithCredential(phone_credential).
	/// `credential` must have been created with PhoneAuthProvider.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Update Phone Number Credential"))
	static UUpdatePhoneNumberCredentialProxy* UpdatePhoneNumberCredential(UUser* const User, const FCredential& Credential);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);

private:
	FCredential Credential;
};

UCLASS()
class UReloadProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnReloaded;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnError;

public:
	/// Refreshes the data for this user.
	/// For example, the attached providers, email address, display name, etc.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Reload"))
	static UReloadProxy* Reload(UUser* const User);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error);
};

UCLASS()
class UDeleteProxy final : public UUserNodeBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnDeleted;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultVoidCallback OnError;

public:
	/// Deletes the user account.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|User", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Delete"))
	static UDeleteProxy* Delete(UUser* const User);

	virtual void Activate();
private:
	void OnActionOver(const EFirebaseAuthError Error);
};
