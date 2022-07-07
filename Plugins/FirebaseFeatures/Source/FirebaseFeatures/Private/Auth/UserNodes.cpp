// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "UserNodes.h"

#include "FirebaseFeatures.h"

#define CHECK_USER(Event, ...) do																\
{																								\
	if (!_User)																					\
	{																							\
		UE_LOG(LogFirebaseAuth, Error, TEXT("Passed a nullptr user object to a method."));		\
		Event.Broadcast(EFirebaseAuthError::UserNotFound, ## __VA_ARGS__);						\
		SetReadyToDestroy();																	\
		return;																					\
	}																							\
} while(0)

#define EXECUTE_CALLBACK(OnSuccess, OnError, ...) do													\
{																										\
	(Error == EFirebaseAuthError::None ? OnSuccess : OnError).Broadcast(Error, ## __VA_ARGS__);			\
	SetReadyToDestroy();																				\
} while(0)


UGetTokenProxy* UGetTokenProxy::GetToken(UUser* const User, const bool bForceRefresh)
{
	UGetTokenProxy* const Proxy = NewObject<UGetTokenProxy>();

	Proxy->bForceRefresh = bForceRefresh;
	Proxy->_User = User;

	return Proxy;
}

void UGetTokenProxy::Activate()
{
	CHECK_USER(OnError, TEXT(""));

	_User->GetToken(bForceRefresh, FGetTokenCallback::CreateUObject(this, &UGetTokenProxy::OnActionOver));
}

void UGetTokenProxy::OnActionOver(const EFirebaseAuthError Error, const FString& Token)
{
	EXECUTE_CALLBACK(OnTokenReceived, OnError, Token);
}

UUpdateEmailProxy* UUpdateEmailProxy::UpdateEmail(UUser* const User, const FString& Email)
{
	UUpdateEmailProxy* const Proxy = NewObject<UUpdateEmailProxy>();

	Proxy->Email = Email;
	Proxy->_User = User;

	return Proxy;
}

void UUpdateEmailProxy::Activate()
{
	CHECK_USER(OnError);

	_User->UpdateEmail(Email, FUserVoidCallback::CreateUObject(this, &UUpdateEmailProxy::OnActionOver));
}

void UUpdateEmailProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnEmailUpdated, OnError);
}

UUpdatePasswordProxy* UUpdatePasswordProxy::UpdatePassword(UUser* const User, const FString& Password)
{
	UUpdatePasswordProxy* const Proxy = NewObject<UUpdatePasswordProxy>();

	Proxy->Password = Password;
	Proxy->_User = User;

	return Proxy;
}

void UUpdatePasswordProxy::Activate()
{
	CHECK_USER(OnError);

	_User->UpdatePassword(Password, FUserVoidCallback::CreateUObject(this, &UUpdatePasswordProxy::OnActionOver));
}

void UUpdatePasswordProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnPasswordUpdated, OnError);
}

UReauthenticateProxy* UReauthenticateProxy::Reauthenticate(UUser* const User, const FCredential& Credential)
{
	UReauthenticateProxy* const Proxy = NewObject<UReauthenticateProxy>();

	Proxy->Credential = Credential;
	Proxy->_User = User;

	return Proxy;
}

void UReauthenticateProxy::Activate()
{
	CHECK_USER(OnError);

	_User->Reauthenticate(Credential, FUserVoidCallback::CreateUObject(this, &UReauthenticateProxy::OnActionOver));
}

void UReauthenticateProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnReauthenticated, OnError);
}

UReauthenticateAndRetrieveDataProxy* UReauthenticateAndRetrieveDataProxy::ReauthenticateAndRetrieveData(UUser* const User, const FCredential& Credential)
{
	UReauthenticateAndRetrieveDataProxy* const Proxy = NewObject<UReauthenticateAndRetrieveDataProxy>();

	Proxy->Credential = Credential;
	Proxy->_User = User;

	return Proxy;
}

void UReauthenticateAndRetrieveDataProxy::Activate()
{
	CHECK_USER(OnError, FSignInResult());

	_User->ReauthenticateAndRetrieveData(Credential, FSignInCallback::CreateUObject(this, &UReauthenticateAndRetrieveDataProxy::OnActionOver));
}

void UReauthenticateAndRetrieveDataProxy::OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result)
{
	EXECUTE_CALLBACK(OnReauthenticated, OnError, Result);
}

UReauthenticateWithProviderProxy* UReauthenticateWithProviderProxy::ReauthenticateWithProvider(UUser* const User, const FFederatedAuthProvider& Provider)
{
	UReauthenticateWithProviderProxy* const Proxy = NewObject<UReauthenticateWithProviderProxy>();

	Proxy->Provider = Provider;
	Proxy->_User = User;

	return Proxy;
}

void UReauthenticateWithProviderProxy::Activate()
{
	CHECK_USER(OnError, FSignInResult());

	_User->ReauthenticateWithProvider(Provider, FSignInCallback::CreateUObject(this, &UReauthenticateWithProviderProxy::OnActionOver));
}

void UReauthenticateWithProviderProxy::OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result)
{
	EXECUTE_CALLBACK(OnReauthenticated, OnError, Result);
}

USendEmailVerificationProxy* USendEmailVerificationProxy::SendEmailVerification(UUser* const User)
{
	USendEmailVerificationProxy* const Proxy = NewObject<USendEmailVerificationProxy>();

	Proxy->_User = User;

	return Proxy;
}

void USendEmailVerificationProxy::Activate()
{
	CHECK_USER(OnError);

	_User->SendEmailVerification(FUserVoidCallback::CreateUObject(this, &USendEmailVerificationProxy::OnActionOver));
}

void USendEmailVerificationProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnEmailSent, OnError);
}

UUpdateUserProfileProxy* UUpdateUserProfileProxy::UpdateUserProfile(UUser* const User, const FUserProfile& Profile)
{
	UUpdateUserProfileProxy* const Proxy = NewObject<UUpdateUserProfileProxy>();

	Proxy->_User = User;
	Proxy->Profile = Profile;

	return Proxy;
}

void UUpdateUserProfileProxy::Activate()
{
	CHECK_USER(OnError);

	_User->UpdateUserProfile(Profile, FUserVoidCallback::CreateUObject(this, &UUpdateUserProfileProxy::OnActionOver));
}

void UUpdateUserProfileProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnProfileUpdated, OnError);
}

ULinkWithCredentialProxy* ULinkWithCredentialProxy::LinkWithCredential(UUser* const User, const FCredential& Credential)
{
	ULinkWithCredentialProxy* const Proxy = NewObject<ULinkWithCredentialProxy>();

	Proxy->Credential = Credential;
	Proxy->_User = User;

	return Proxy;
}

void ULinkWithCredentialProxy::Activate()
{
	CHECK_USER(OnError, nullptr);

	_User->LinkWithCredential(Credential, FSignInUserCallback::CreateUObject(this, &ULinkWithCredentialProxy::OnActionOver));
}

void ULinkWithCredentialProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnLinked, OnError, Result);
}

ULinkAndRetrieveDataWithCredentialProxy* ULinkAndRetrieveDataWithCredentialProxy::LinkAndRetrieveDataWithCredential(UUser* const User, const FCredential& Credential)
{
	ULinkAndRetrieveDataWithCredentialProxy* const Proxy = NewObject<ULinkAndRetrieveDataWithCredentialProxy>();

	Proxy->Credential = Credential;
	Proxy->_User = User;

	return Proxy;
}

void ULinkAndRetrieveDataWithCredentialProxy::Activate()
{
	CHECK_USER(OnError, FSignInResult());

	_User->LinkAndRetrieveDataWithCredential(Credential, FSignInCallback::CreateUObject(this, &ULinkAndRetrieveDataWithCredentialProxy::OnActionOver));
}

void ULinkAndRetrieveDataWithCredentialProxy::OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result)
{
	EXECUTE_CALLBACK(OnLinked, OnError, Result);
}

ULinkWithProviderProxy* ULinkWithProviderProxy::LinkWithProvider(UUser* const User, const FFederatedAuthProvider& Provider)
{
	ULinkWithProviderProxy* const Proxy = NewObject<ULinkWithProviderProxy>();

	Proxy->Provider = Provider ;
	Proxy->_User = User;

	return Proxy;
}

void ULinkWithProviderProxy::Activate()
{
	CHECK_USER(OnError, FSignInResult());

	_User->LinkWithProvider(Provider, FSignInCallback::CreateUObject(this, &ULinkWithProviderProxy::OnActionOver));
}

void ULinkWithProviderProxy::OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result)
{
	EXECUTE_CALLBACK(OnLinked, OnError, Result);
}

UUnlinkProxy* UUnlinkProxy::Unlink(UUser* const User, const FString& Provider)
{
	UUnlinkProxy* const Proxy = NewObject<UUnlinkProxy>();

	Proxy->Provider = Provider;
	Proxy->_User = User;

	return Proxy;
}

void UUnlinkProxy::Activate()
{
	CHECK_USER(OnError, nullptr);

	_User->Unlink(Provider, FSignInUserCallback::CreateUObject(this, &UUnlinkProxy::OnActionOver));
}

void UUnlinkProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnUnlinked, OnError, Result);
}

UUpdatePhoneNumberCredentialProxy* UUpdatePhoneNumberCredentialProxy::UpdatePhoneNumberCredential(UUser* const User, const FCredential& Credential)
{
	UUpdatePhoneNumberCredentialProxy* const Proxy = NewObject<UUpdatePhoneNumberCredentialProxy>();

	Proxy->Credential = Credential;
	Proxy->_User = User;

	return Proxy;
}

void UUpdatePhoneNumberCredentialProxy::Activate()
{
	CHECK_USER(OnError, nullptr);

	_User->UpdatePhoneNumberCredential(Credential, FSignInUserCallback::CreateUObject(this, &UUpdatePhoneNumberCredentialProxy::OnActionOver));
}

void UUpdatePhoneNumberCredentialProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnUpdated, OnError, Result);
}

UReloadProxy* UReloadProxy::Reload(UUser* const User)
{
	UReloadProxy* const Proxy = NewObject<UReloadProxy>();

	Proxy->_User = User;

	return Proxy;
}

void UReloadProxy::Activate()
{
	CHECK_USER(OnError);

	_User->Reload(FAuthCallback::CreateUObject(this, &UReloadProxy::OnActionOver));
}

void UReloadProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnReloaded, OnError);
}

UDeleteProxy* UDeleteProxy::Delete(UUser* const User)
{
	UDeleteProxy* const Proxy = NewObject<UDeleteProxy>();

	Proxy->_User = User;

	return Proxy;
}

void UDeleteProxy::Activate()
{
	CHECK_USER(OnError);

	_User->Delete(FAuthCallback::CreateUObject(this, &UDeleteProxy::OnActionOver));
}

void UDeleteProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnDeleted, OnError);
}




#undef EXECUTE_CALLBACK
#undef CHECK_USER
