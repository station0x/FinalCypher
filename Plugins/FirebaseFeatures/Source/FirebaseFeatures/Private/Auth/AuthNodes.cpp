// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "AuthNodes.h"
#include "Auth/Credential.h"

#define EXECUTE_CALLBACK(OnSuccess, OnError, ...) do											\
{																								\
	(Error == EFirebaseAuthError::None ? OnSuccess : OnError).Broadcast(Error, ## __VA_ARGS__);	\
	SetReadyToDestroy();																		\
} while(0)

TWeakObjectPtr<UListenAuthEventsProxy> UListenAuthEventsProxy::Singleton;

FFederatedAuthProvider UAuthMiscLibrary::CreateOAuthProvider(const FFederatedOAuthProviderData & ProviderData)
{
	return FAuth::CreateFederatedOAuthProvider(ProviderData);
}

FString UAuthMiscLibrary::Provider(const FCredential& Credential)
{
	return Credential.Provider();
}

bool UAuthMiscLibrary::IsValid(const FCredential& Credential)
{
	return Credential.IsValid();
}

UUser* UAuthMiscLibrary::CurrentUser()
{
	return FAuth::CurrentUser();
}

void UAuthMiscLibrary::SignOut()
{
	FAuth::SignOut();
}


UFetchProvidersForEmailProxy* UFetchProvidersForEmailProxy::FetchProvidersForEmail(const FString& Email)
{
	UFetchProvidersForEmailProxy* const Proxy = NewObject<UFetchProvidersForEmailProxy>();

	Proxy->Email = Email;

	return Proxy;
}

void UFetchProvidersForEmailProxy::Activate()
{
	FAuth::FetchProvidersForEmail(Email, FFetchProvidersForEmailCallback::CreateUObject(this, &UFetchProvidersForEmailProxy::OnActionOver));
}

void UFetchProvidersForEmailProxy::OnActionOver(const EFirebaseAuthError Error, const TArray<FString>& Providers)
{
	EXECUTE_CALLBACK(OnProvidersFetched, OnError, Providers);
}

USignInWithCustomTokenProxy* USignInWithCustomTokenProxy::SignInWithCustomToken(const FString& Token)
{
	USignInWithCustomTokenProxy* const Proxy = NewObject<USignInWithCustomTokenProxy>();

	Proxy->Token = Token;

	return Proxy;
}

void USignInWithCustomTokenProxy::Activate()
{
	FAuth::SignInWithCustomToken(Token, FSignInUserCallback::CreateUObject(this, &USignInWithCustomTokenProxy::OnActionOver));
}

void USignInWithCustomTokenProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnSignedIn, OnError, Result);
}

USignInWithCredentialProxy* USignInWithCredentialProxy::SignInWithCredential(const FCredential& Credential)
{
	USignInWithCredentialProxy* const Proxy = NewObject<USignInWithCredentialProxy>();

	Proxy->Credential = Credential;

	return Proxy;
}

void USignInWithCredentialProxy::Activate()
{
	FAuth::SignInWithCredential(Credential, FSignInUserCallback::CreateUObject(this, &USignInWithCredentialProxy::OnActionOver));
}

void USignInWithCredentialProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnSignedIn, OnError, Result);
}

USignInWithProviderProxy* USignInWithProviderProxy::SignInWithProvider(const FFederatedAuthProvider& AuthProvider)
{
	USignInWithProviderProxy* const Proxy = NewObject<USignInWithProviderProxy>();

	Proxy->AuthProvider = AuthProvider;

	return Proxy;
}

void USignInWithProviderProxy::Activate()
{
	FAuth::SignInWithProvider(AuthProvider, FSignInCallback::CreateUObject(this, &USignInWithProviderProxy::OnActionOver));
}

void USignInWithProviderProxy::OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result)
{
	EXECUTE_CALLBACK(OnSignedIn, OnError, Result);
}

USignInAndRetrieveDataWithCredentialProxy* USignInAndRetrieveDataWithCredentialProxy::SignInAndRetrieveDataWithCredential(const FCredential& Credential)
{
	USignInAndRetrieveDataWithCredentialProxy* const Proxy = NewObject<USignInAndRetrieveDataWithCredentialProxy>();

	Proxy->Credential = Credential;

	return Proxy;
}

void USignInAndRetrieveDataWithCredentialProxy::Activate()
{
	FAuth::SignInAndRetrieveDataWithCredential(Credential, FSignInCallback::CreateUObject(this, &USignInAndRetrieveDataWithCredentialProxy::OnActionOver));
}

void USignInAndRetrieveDataWithCredentialProxy::OnActionOver(const EFirebaseAuthError Error, const FSignInResult& Result)
{
	EXECUTE_CALLBACK(OnSignedIn, OnError, Result);
}

USignInAnonymouslyProxy* USignInAnonymouslyProxy::SignInAnonymously()
{
	USignInAnonymouslyProxy* const Proxy = NewObject<USignInAnonymouslyProxy>();

	return Proxy;
}

void USignInAnonymouslyProxy::Activate()
{
	FAuth::SignInAnonymously(FSignInUserCallback::CreateUObject(this, &USignInAnonymouslyProxy::OnActionOver));
}

void USignInAnonymouslyProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnSignedIn, OnError, Result);
}

USignInWithEmailAndPasswordProxy* USignInWithEmailAndPasswordProxy::SignInWithEmailAndPassword(const FString& Email, const FString& Password)
{
	USignInWithEmailAndPasswordProxy* const Proxy = NewObject<USignInWithEmailAndPasswordProxy>();

	Proxy->Email    = Email;
	Proxy->Password = Password;

	return Proxy;
}

void USignInWithEmailAndPasswordProxy::Activate()
{
	FAuth::SignInWithEmailAndPassword(Email, Password, FSignInUserCallback::CreateUObject(this, &USignInWithEmailAndPasswordProxy::OnActionOver));
}

void USignInWithEmailAndPasswordProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnSignedIn, OnError, Result);
}

UCreateUserWithEmailAndPasswordProxy* UCreateUserWithEmailAndPasswordProxy::CreateUserWithEmailAndPassword(const FString& Email, const FString& Password)
{
	UCreateUserWithEmailAndPasswordProxy* const Proxy = NewObject<UCreateUserWithEmailAndPasswordProxy>();

	Proxy->Email = Email;
	Proxy->Password = Password;

	return Proxy;
}

void UCreateUserWithEmailAndPasswordProxy::Activate()
{
	FAuth::CreateUserWithEmailAndPassword(Email, Password, FSignInUserCallback::CreateUObject(this, &UCreateUserWithEmailAndPasswordProxy::OnActionOver));
}

void UCreateUserWithEmailAndPasswordProxy::OnActionOver(const EFirebaseAuthError Error, UUser* const Result)
{
	EXECUTE_CALLBACK(OnUserCreated, OnError, Result);
}

USendPasswordResetEmailProxy* USendPasswordResetEmailProxy::SendPasswordResetEmail(const FString& Email)
{
	USendPasswordResetEmailProxy* const Proxy = NewObject<USendPasswordResetEmailProxy>();

	Proxy->Email = Email;

	return Proxy;
}

void USendPasswordResetEmailProxy::Activate()
{
	FAuth::SendPasswordResetEmail(Email, FAuthCallback::CreateUObject(this, &USendPasswordResetEmailProxy::OnActionOver));
}

void USendPasswordResetEmailProxy::OnActionOver(const EFirebaseAuthError Error)
{
	EXECUTE_CALLBACK(OnEmailSent, OnError);
}

UListenAuthEventsProxy* UListenAuthEventsProxy::ListenAuthEvents()
{
	if (!Singleton.IsValid())
	{
		Singleton = NewObject<ThisClass>();
	}

	return Singleton.Get();
}

void UListenAuthEventsProxy::Activate()
{
	FFirebaseFeaturesModule::OnAuthStateChanged().RemoveAll(this);
	FFirebaseFeaturesModule::OnIdTokenChanged()	 .RemoveAll(this);

	FFirebaseFeaturesModule::OnAuthStateChanged().AddUObject(this, &ThisClass::OnAuthChangedCallback);
	FFirebaseFeaturesModule::OnIdTokenChanged()	 .AddUObject(this, &ThisClass::OnIdTokenChangedCallback);
}

void UListenAuthEventsProxy::OnAuthChangedCallback()
{
	OnAuthChanged.Broadcast();
}

void UListenAuthEventsProxy::OnIdTokenChangedCallback()
{
	OnIdTokenChanged.Broadcast();
}

UAppleSignInGetCred* UAppleSignInGetCred::GetCredential(FString Nonce)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Nonce = MoveTemp(Nonce);

	return Proxy;
}

void UAppleSignInGetCred::Activate()
{
	UCredentialLibrary::GetCredentialFromAppleSignIn(MoveTemp(Nonce), 
		FGetAppleSignInCredentialCallback::CreateUObject(this, &ThisClass::OnTaskOver));
}

void UAppleSignInGetCred::OnTaskOver(bool bSuccess, FCredential Credential, int32 ErrorCode, FString ErrorMessage)
{
	(bSuccess ? OnSuccess : OnError).Broadcast(Credential, ErrorCode, ErrorMessage);
	SetReadyToDestroy();
}




#undef EXECUTE_CALLBACK
