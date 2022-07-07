// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "FirebaseSdk/FirebaseErrors.h"
#include "Auth.h"
#include "User.generated.h"

namespace firebase 
{ 
	namespace auth 
	{ 
		class User; 
		class UserInfoInterface;
	}; 
};

/// Parameters to the UpdateUserProfile() function.
///
/// For fields you don't want to update, pass "".
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FUserProfile 
{
	GENERATED_BODY()
public:
	/// Construct a UserProfile with no display name or photo URL.
	FUserProfile() {};

	/// User display name.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString DisplayName;

	/// If we want to reset the display name.
	/// If it is set to true, the content of DisplayName is ignored.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	bool bResetDisplayName = false;

	/// User photo URI.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString PhotoUrl;
	
	/// If we want to reset the photo URL.
	/// If it is set to true, the content of PthotoUrl is ignored.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	bool bResetPhotoUrl = false;
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FUserInfoInterface 
{
public:
	GENERATED_BODY()
private:
	friend class UUser;

#if WITH_FIREBASE_AUTH
	FUserInfoInterface(firebase::auth::UserInfoInterface* const Info);
#endif

public:
	FUserInfoInterface() {};

	/// Gets the unique Firebase user ID for the user.
	///
	/// @note The user's ID, unique to the Firebase project.
	/// Do NOT use this value to authenticate with your backend server, if you
	/// have one.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString Uid;

	/// Gets email associated with the user, if any.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString Email;

	/// Gets the display name associated with the user, if any.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString DisplayName;

	/// Gets the photo url associated with the user, if any.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString PhotoUrl;

	/// Gets the provider ID for the user (For example, "Facebook").
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString ProviderId;

	/// Gets the phone number for the user, in E.164 format.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Auth")
	FString PhoneNumber;
};

DECLARE_DELEGATE_OneParam (FUserVoidCallback, const EFirebaseAuthError /* Error */);
DECLARE_DELEGATE_TwoParams(FGetTokenCallback, const EFirebaseAuthError /* Error */, const FString& /* Token */);

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UUser : public UObject
{
	GENERATED_BODY()
private:
	friend class FAuth;
	friend class FAuthHelper;

public:
	UUser();

	/// The Java Web Token (JWT) that can be used to identify the user to
	/// the backend.
	/// If a current ID token is still believed to be valid (i.e. it has not yet
	/// expired), that token will be returned immediately.
	/// A developer may set the optional bForceRefresh flag to get a new ID token,
	/// whether or not the existing token has expired. For example, a developer
	/// may use this when they have discovered that the token is invalid for some
	/// other reason.
	void GetToken(const bool bForceRefresh, FGetTokenCallback Callback);

	/// Gets the third party profile data associated with this user returned by
	/// the authentication server, if any.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "Provider Data") TArray<FUserInfoInterface> ProviderData();

	/// Sets the email address for the user.
	/// May fail if there is already an email/password-based account for the same
	/// email address.
	void UpdateEmail(const FString& Email, const FUserVoidCallback& Callback = FUserVoidCallback());

	/// Attempts to change the password for the current user.
	/// For an account linked to an Identity Provider (IDP) with no password,
	/// this will result in the account becoming an email/password-based account
	/// while maintaining the IDP link. May fail if the password is invalid,
	/// if there is a conflicting email/password-based account, or if the token
	/// has expired.
	void UpdatePassword(const FString& Password, const FUserVoidCallback& Callback = FUserVoidCallback());

	/// Convenience function for @ref ReauthenticateAndRetrieveData that discards
	/// the returned AdditionalUserInfo data.
	void Reauthenticate(const FCredential& Credential, const FUserVoidCallback& Callback = FUserVoidCallback());

	/// Reauthenticate using a credential.
	/// Data from the Identity Provider used to sign-in is returned in the
	/// AdditionalUserInfo inside the returned SignInResult.
	/// Returns an error if the existing credential is not for this user
	/// or if sign-in with that credential failed. The user should remain
	/// signed in even if this method failed. If the developer had held
	/// a reference to that user, the reference will continue to be valid
	/// after this operation.
	void ReauthenticateAndRetrieveData(const FCredential& Credential, const FSignInCallback& Callback = FSignInCallback());

	/// @brief Re-authenticates the user with a federated auth provider.
	/// @param[in] Provider Contains information on the auth provider to
	/// authenticate with.
	/// @return A SignInResult via callback with the result of the re-authentication
	/// request.
	/// @note: This operation is supported only on iOS and Android platforms. On
	/// non-mobile platforms this method will return a Future with a preset error
	/// code: kAuthErrorUnimplemented.
	void ReauthenticateWithProvider(const FFederatedAuthProvider& Provider, const FSignInCallback& Callback = FSignInCallback());

	/// Initiates email verification for the user.
	void SendEmailVerification(const FUserVoidCallback& Callback = FUserVoidCallback());

	/// Updates a subset of user profile information.
	void UpdateUserProfile(const FUserProfile& Profile, const FUserVoidCallback& Callback = FUserVoidCallback());

	/// Convenience function for ReauthenticateAndRetrieveData that discards
	/// the returned AdditionalUserInfo in SignInResult.
	void LinkWithCredential(const FCredential& Credential, const FSignInUserCallback& Callback);

	/// Links the user with the given 3rd party credentials.
	/// For example, a Facebook login access token, a Twitter token/token-secret
	/// pair.
	/// Status will be an error if the token is invalid, expired, or otherwise
	/// not accepted by the server as well as if the given 3rd party
	/// user id is already linked with another user account or if the current user
	/// is already linked with another id from the same provider.
	/// Data from the Identity Provider used to sign-in is returned in the
	/// AdditionalUserInfo inside SignInResult.
	void LinkAndRetrieveDataWithCredential(const FCredential& Credential, const FSignInCallback& Callback);

	/// Links this user with a federated auth provider.
	/// @param[in] Provider Contains information on the auth provider to link
	/// with.
	/// @return A SignInResult via callback with the user data result of the link
	/// request.
	/// @note: This operation is supported only on iOS and Android platforms. On
	/// non-mobile platforms this method will return a Future with a preset error
	/// code: kAuthErrorUnimplemented.
	void LinkWithProvider(const FFederatedAuthProvider& Provider, const FSignInCallback& Callback);

	/// Unlinks the current user from the provider specified.
	/// Status will be an error if the user is not linked to the given provider.
	void Unlink(const FString& Provider, const FSignInUserCallback& Callback);

	/// Updates the currently linked phone number on the user.
	/// This is useful when a user wants to change their phone number. It is a
	/// shortcut to calling Unlink(phone_credential.provider()) and then
	/// LinkWithCredential(phone_credential).
	/// `credential` must have been created with PhoneAuthProvider.
	void UpdatePhoneNumberCredential(const FCredential& Credential, const FSignInUserCallback& Callback);

	/// Refreshes the data for this user.
	/// For example, the attached providers, email address, display name, etc.
	void Reload(const FAuthCallback& Callback = FAuthCallback());

	/// Deletes the user account.
	void Delete(const FAuthCallback& Callback = FAuthCallback());

	/// Gets the metadata for this user account.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "Metadata") FUserMetadata Metadata() const;

	/// Returns true if the email address associated with this user has been
	/// verified.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "Verified") bool IsEmailVerified() const;

	/// Returns true if user signed in anonymously.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "Anonymous") bool IsAnonymous() const;

	/// Gets the unique Firebase user ID for the user.
	///
	/// @note The user's ID, unique to the Firebase project.
	/// Do NOT use this value to authenticate with your backend server, if you
	/// have one.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "UID") FString Uid() const;

	/// Gets email associated with the user, if any.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "Email") FString Email() const;

	/// Gets the display name associated with the user, if any.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "DisplayName") FString DisplayName() const;

	/// Gets the photo url associated with the user, if any.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "Photo URL") FString PhotoUrl() const;

	/// Gets the provider ID for the user (For example, "Facebook").
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "ID") FString ProviderId() const;

	/// Gets the phone number for the user, in E.164 format.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Auth|User")
	UPARAM(DisplayName = "Phone Number") FString PhoneNumber() const;

private:
#if WITH_FIREBASE_AUTH
	firebase::auth::User* User;
#endif
};
