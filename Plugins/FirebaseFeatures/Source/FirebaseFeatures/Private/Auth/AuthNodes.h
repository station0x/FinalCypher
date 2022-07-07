// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "FirebaseFeatures.h"
#include "Auth/User.h"
#include "Auth/Auth.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "AuthNodes.generated.h"

/**
 * Contains methods for Blueprints.
 * Not exported because C++ modules don't need it.
 **/
UCLASS()
class UAuthMiscLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// Gets the name of the Identification Provider (IDP) for the credential.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential")
	static UPARAM(DisplayName = "Provider") FString Provider(UPARAM(ref) const FCredential& Credential);

	/// Get whether this credential is valid. A credential can be
	/// invalid in an error condition, e.g. empty username/password.
	/// @returns True if the credential is valid, false otherwise.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|Credential")
	static UPARAM(DisplayName = "Is Valid") bool IsValid(UPARAM(ref) const FCredential& Credential);

	/// Synchronously gets the cached current user, or nullptr if there is none.
	/// @note This function may block and wait until the Auth instance finishes
	/// loading the saved user's state. This should only happen for a short
	/// period of time after the Auth instance is created.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth")
	static UPARAM(DisplayName = "User") UUser* CurrentUser();

	/// Removes any existing authentication credentials from this client.
	/// This function always succeeds.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth")
	static void SignOut();

	/// @brief Configures the provider with OAuth provider information.
	/// @param[in] ProviderData Contains the federated provider id and OAuth
	/// scopes and OAuth custom parameters required for user authentication and
	/// user linking.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth")
	static FFederatedAuthProvider CreateOAuthProvider(const FFederatedOAuthProviderData& ProviderData);
};


USTRUCT(BlueprintType)
struct FProviderList
{
	GENERATED_BODY()
public:
	FProviderList() {};
	FProviderList(const TArray<FString>& InProviders) : Providers(InProviders) {};

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth|Providers")
	TArray<FString> Providers;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam (FDynMultiVoidCallback,		 const EFirebaseAuthError, Error);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultProvidersCallback,  const EFirebaseAuthError, Error, const FProviderList&, Providers);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultSignInUserCallback, const EFirebaseAuthError, Error, UUser* const, User);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultSignInCallback_,	 const EFirebaseAuthError, Error, const FSignInResult&, Result);

UCLASS()
class UFetchProvidersForEmailProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultProvidersCallback OnProvidersFetched;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultProvidersCallback OnError;

public:
	/// Asynchronously requests the IDPs (identity providers) that can be used
	/// for the given email address.
	/// Useful for an "identifier-first" login flow.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Fetch Providers for Email"))
	static UFetchProvidersForEmailProxy* FetchProvidersForEmail(const FString& Email);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, const TArray<FString>& Providers);

private:
	FString Email;
};

UCLASS()
class USignInWithCustomTokenProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnSignedIn;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnError;

public:
	/// Asynchronously logs into Firebase with the given Auth token.
	/// An error is returned, if the token is invalid, expired or otherwise
	/// not accepted by the server.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Sign In with Custom Token"))
	static USignInWithCustomTokenProxy* SignInWithCustomToken(const FString& Token);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);

private:
	FString Token;
};

UCLASS()
class USignInWithCredentialProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnSignedIn;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnError;

public:
	/// Convenience method for SignInAndRetrieveDataWithCredential that
	/// doesn't return additional identity provider data.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Sign In with Credential"))
	static USignInWithCredentialProxy* SignInWithCredential(const FCredential& Credential);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);

private:
	FCredential Credential;
};

UCLASS()
class USignInWithProviderProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback_ OnSignedIn;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback_ OnError;

public:
	/// Sign-in a user authenticated via a federated auth provider.
	/// @param[in] Provider Contains information on the provider to authenticate
	/// with.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Sign In with Provider"))
	static USignInWithProviderProxy* SignInWithProvider(const FFederatedAuthProvider& AuthProvider);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result);

private:
	FFederatedAuthProvider AuthProvider;
};

UCLASS()
class USignInAndRetrieveDataWithCredentialProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback_ OnSignedIn;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInCallback_ OnError;

public:
	/// Asynchronously logs into Firebase with the given credentials.
	/// For example, the credential could wrap a Facebook login access token or
	/// a Twitter token/token-secret pair.
	/// The SignInResult contains both a reference to the User (which can be null
	/// if the sign in failed), and AdditionalUserInfo, which holds details
	/// specific to the Identity Provider used to sign in.
	/// An error is returned if the token is invalid, expired, or otherwise not
	/// accepted by the server.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Sign In and retrieve Data with Credential"))
	static USignInAndRetrieveDataWithCredentialProxy* SignInAndRetrieveDataWithCredential(const FCredential& Credential);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result);

private:
	FCredential Credential;
};

UCLASS()
class USignInAnonymouslyProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnSignedIn;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnError;

public:
	/// Asynchronously creates and becomes an anonymous user.
	/// If there is already an anonymous user signed in, that user will be
	/// returned instead.
	/// If there is any other existing user, that user will be signed out.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Sign In Anonymously"))
	static USignInAnonymouslyProxy* SignInAnonymously();

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);
};

UCLASS()
class USignInWithEmailAndPasswordProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnSignedIn;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnError;

public:
	/// Signs in using provided email address and password.
	/// An error is returned if the password is wrong or otherwise not accepted
	/// by the server.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Sign In with Email and Password"))
	static USignInWithEmailAndPasswordProxy* SignInWithEmailAndPassword(const FString& Email, const FString& Password);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);

private:
	FString Email;
	FString Password;
};

UCLASS()
class UCreateUserWithEmailAndPasswordProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnUserCreated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSignInUserCallback OnError;

public:
	/// Creates, and on success, logs in a user with the given email address
	/// and password.
	/// An error is returned when account creation is unsuccessful
	/// (due to another existing account, invalid password, etc.).
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Create User with Email and Password"))
	static UCreateUserWithEmailAndPasswordProxy* CreateUserWithEmailAndPassword(const FString& Email, const FString& Password);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error, UUser* const Result);

private:
	FString Email;
	FString Password;
};

UCLASS()
class USendPasswordResetEmailProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultiVoidCallback OnEmailSent;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultiVoidCallback OnError;

public:
	/// Initiates a password reset for the given email address.
	/// If the email address is not registered, then the callback has a
	/// status of IsFaulted.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Send Password Reset Email"))
	static USendPasswordResetEmailProxy* SendPasswordResetEmail(const FString& Email);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseAuthError Error);

private:
	FString Email;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDynMultOnEvent);

UCLASS()
class UListenAuthEventsProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultOnEvent OnAuthChanged;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultOnEvent OnIdTokenChanged;

public:
	/**
	 * Listens to the Auth events.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Listen Auth Events"))
	static UListenAuthEventsProxy* ListenAuthEvents();

	virtual void Activate();

private:
	void OnAuthChangedCallback();
	void OnIdTokenChangedCallback();

	static TWeakObjectPtr<UListenAuthEventsProxy> Singleton;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynMulAppleSignInCred, const FCredential&, Credential, int32, Error, const FString&, ErrorMessage);

UCLASS()
class UAppleSignInGetCred final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMulAppleSignInCred OnSuccess;
	UPROPERTY(BlueprintAssignable)
	FDynMulAppleSignInCred OnError;

public:

	/// Generates a credential from Apple Sign-In
	/// @param Nonce The nonce used during the sign-in process.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Auth|Apple", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Credential from Apple Sign-In"))
	static UAppleSignInGetCred* GetCredential(FString Nonce);

	virtual void Activate() override;

private:
	void OnTaskOver(bool bSuccess, FCredential Credential, int32 ErrorCode, FString ErrorMessage);

	FString Nonce;
};