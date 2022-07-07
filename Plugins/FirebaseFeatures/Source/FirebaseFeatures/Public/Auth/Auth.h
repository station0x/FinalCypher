// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FirebaseFeatures.h"
#include "FirebaseSdk/FirebaseErrors.h"
#include "FirebaseSdk/FirebaseVariant.h"
#include "Auth.generated.h"

namespace firebase 
{ 
	namespace auth 
	{ 
		class Auth; 
		class Credential;
		class FederatedAuthProvider;
		class FederatedOAuthProvider;
	};
};

class UUser;

/// Metadata corresponding to a Firebase user.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FUserMetadata 
{
	GENERATED_BODY()
public:
	FUserMetadata() : LastSignInTimestamp(0), CreationTtimestamp(0) {}

	/// The last sign in UTC timestamp in milliseconds.
	/// See https://en.wikipedia.org/wiki/Unix_time for details of UTC.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	int64 LastSignInTimestamp;

	/// The Firebase user creation UTC timestamp in milliseconds.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	int64 CreationTtimestamp;
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FCredential
{
	GENERATED_BODY()
private:
	friend class FPhoneProviderListener;
	friend class UCredentialLibrary;
	friend class UUser;
	friend class FAuth;
	friend class FAuthHelper;

public:
#if WITH_FIREBASE_AUTH
	FCredential(const firebase::auth::Credential* InCredential);
	FCredential(firebase::auth::Credential InCredential);
#endif

public:
	FCredential();
	FCredential(FCredential&& Other);
	FCredential(const FCredential& Other);

	FCredential& operator=(const FCredential& Other);
	FCredential& operator=(FCredential&& Other);

	~FCredential();
	
	/// Gets the name of the Identification Provider (IDP) for the credential.
	FString Provider() const;

	/// Get whether this credential is valid. A credential can be
	/// invalid in an error condition, e.g. empty username/password.
	/// @returns True if the credential is valid, false otherwise.
	bool IsValid() const;

private:
#if WITH_FIREBASE_AUTH
	TUniquePtr<firebase::auth::Credential> Credential;
#endif
};


/// Additional user data returned from an identity provider.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FAdditionalUserInfo 
{
	GENERATED_BODY()
public:
	/// The provider identifier.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString ProviderId;

	/// The name of the user.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString UserName;

	/// Additional identity-provider specific information.
	/// Most likely a hierarchical key-value mapping, like a parsed JSON file.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	TMap<FFirebaseVariant, FFirebaseVariant> Profile;

	/// On a nonce-based credential link failure where the user has already linked
	/// to the provider, the Firebase auth service may provide an updated
	/// Credential. If is_valid returns true on this credential, then it may be
	/// passed to a new SignInWithCredential request to sign
	/// the user in with the provider.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FCredential UpdatedCredential;
};

/// @brief Result of operations that can affect authentication state.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FSignInResult
{
	GENERATED_BODY()
public:
	FSignInResult() : User(nullptr) {}

	/// The currently signed-in @ref User, or NULL if there isn't any (i.e. the
	/// user is signed out).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	UUser* User;

	/// Identity-provider specific information for the user, if the provider is
	/// one of Facebook, GitHub, Google, or Twitter.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FAdditionalUserInfo Info;

	/// Metadata associated with the Firebase user.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FUserMetadata Meta;
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFederatedOAuthProviderData
{
	GENERATED_BODY()
public:
	/// Contains the id of the provider to be used during sign-in, link, or
	/// reauthentication requests.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString ProviderId;

	/// OAuth parmeters which specify which rights of access are being requested.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	TArray<FString> Scopes;

	/// OAuth parameters which are provided to the federated provider service.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	TMap<FString, FString> CustomParameters;
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFederatedAuthProvider
{
	GENERATED_BODY()
private:
	friend class UUser;
	friend class FAuth;
	friend class FAuthHelper;

#if WITH_FIREBASE_AUTH
	FFederatedAuthProvider(TSharedPtr<firebase::auth::FederatedOAuthProvider> Provider);
#endif
public:
	FFederatedAuthProvider();
	FFederatedAuthProvider(const FFederatedAuthProvider& Other);
	FFederatedAuthProvider(FFederatedAuthProvider&& Other);

	FFederatedAuthProvider& operator=(const FFederatedAuthProvider& Other);
	FFederatedAuthProvider& operator=(FFederatedAuthProvider&& Other);

	~FFederatedAuthProvider();

private:
#if WITH_FIREBASE_AUTH
	TSharedPtr<firebase::auth::FederatedAuthProvider> AuthProvider;
#endif
};

DECLARE_DELEGATE_OneParam (FAuthCallback,					const EFirebaseAuthError /* Error */);
DECLARE_DELEGATE_TwoParams(FFetchProvidersForEmailCallback, const EFirebaseAuthError /* Error */, const TArray<FString>& /* Providers */);
DECLARE_DELEGATE_TwoParams(FSignInUserCallback,				const EFirebaseAuthError /* Error */, UUser* /* User */);
DECLARE_DELEGATE_TwoParams(FSignInCallback,					const EFirebaseAuthError /* Error */, const FSignInResult& /* Result */);

/// FAuth is the gateway to the Firebase authentication API.
/// With it, you can reference UUser objects to manage user
/// accounts and credentials.
class FIREBASEFEATURES_API FAuth final
{
private:
	FAuth() = delete;
	FAuth(const FAuth& ) = delete;
	FAuth(const FAuth&&) = delete;

public:
	/// Synchronously gets the cached current user, or nullptr if there is none.
	/// @note This function may block and wait until the Auth instance finishes
	/// loading the saved user's state. This should only happen for a short
	/// period of time after the Auth instance is created.
	static UUser* CurrentUser();

	/// Asynchronously requests the IDPs (identity providers) that can be used
	/// for the given email address.
	/// Useful for an "identifier-first" login flow.
	static void FetchProvidersForEmail(const FString& Email, const FFetchProvidersForEmailCallback& Callback);
	
	/// Asynchronously logs into Firebase with the given Auth token.
	/// An error is returned, if the token is invalid, expired or otherwise
	/// not accepted by the server.
	static void SignInWithCustomToken(const FString& Token, const FSignInUserCallback& Callback);

	/// Convenience method for SignInAndRetrieveDataWithCredential that
	/// doesn't return additional identity provider data.
	static void SignInWithCredential(const FCredential& Credential, const FSignInUserCallback& Callback);
	
	/// Sign-in a user authenticated via a federated auth provider.
	/// @param[in] Provider Contains information on the provider to authenticate
	/// with.
	static void SignInWithProvider(const FFederatedAuthProvider& AuthProvider, const FSignInCallback& Callback);
	
	/// Asynchronously logs into Firebase with the given credentials.
	/// For example, the credential could wrap a Facebook login access token or
	/// a Twitter token/token-secret pair.
	/// The SignInResult contains both a reference to the User (which can be null
	/// if the sign in failed), and AdditionalUserInfo, which holds details
	/// specific to the Identity Provider used to sign in.
	/// An error is returned if the token is invalid, expired, or otherwise not
	/// accepted by the server.
	static void SignInAndRetrieveDataWithCredential(const FCredential& Credential, const FSignInCallback& Callback);

	/// Asynchronously creates and becomes an anonymous user.
	/// If there is already an anonymous user signed in, that user will be
	/// returned instead.
	/// If there is any other existing user, that user will be signed out.
	static void SignInAnonymously(const FSignInUserCallback& Callback);

	/// Signs in using provided email address and password.
	/// An error is returned if the password is wrong or otherwise not accepted
	/// by the server.
	static void SignInWithEmailAndPassword(const FString& Email, const FString& Password, const FSignInUserCallback& Callback);

	/// Creates, and on success, logs in a user with the given email address
	/// and password.
	/// An error is returned when account creation is unsuccessful
	/// (due to another existing account, invalid password, etc.).
	static void CreateUserWithEmailAndPassword(const FString& Email, const FString& Password, const FSignInUserCallback& Callback);

	/// Removes any existing authentication credentials from this client.
	/// This function always succeeds.
	static void SignOut();

	/// Initiates a password reset for the given email address.
	///
	/// If the email address is not registered, then the returned task has a
	/// status of IsFaulted.
	static void SendPasswordResetEmail(const FString& Email, const FAuthCallback& Callback = FAuthCallback());

	/// @brief Create and configures the provider with OAuth provider information.
	/// @param[in] ProviderData Contains the federated provider id and OAuth
	/// scopes and OAuth custom parameters required for user authentication and
	/// user linking.
	static FFederatedAuthProvider CreateFederatedOAuthProvider(const FFederatedOAuthProviderData& ProviderData);

private:
#if WITH_FIREBASE_AUTH
	static firebase::auth::Auth* GetAuth();
#endif
};
