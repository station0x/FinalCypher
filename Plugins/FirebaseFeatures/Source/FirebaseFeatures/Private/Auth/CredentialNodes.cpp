// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "CredentialNodes.h"

UGetCredentialFromGameCenterProxy* UGetCredentialFromGameCenterProxy::GetCredentialFromGameCenter()
{
	return NewObject<UGetCredentialFromGameCenterProxy>();
}

void UGetCredentialFromGameCenterProxy::Activate()
{
	UCredentialLibrary::GetCredentialFromGameCenter(FGetCredentialCallback::CreateUObject(this, &UGetCredentialFromGameCenterProxy::OnActionOver));
}

void UGetCredentialFromGameCenterProxy::OnActionOver(const EFirebaseAuthError Error, const FCredential& Credential)
{
	(Error == EFirebaseAuthError::None ? OnSuccess : OnError).Broadcast(Error, Credential);
	SetReadyToDestroy();
}

UVerifyPhoneNumberProxy* UVerifyPhoneNumberProxy::VerifyPhoneNumber(const FString& PhoneNumber, const int64 AutoVerifyTimeoutInMilliseconds, const FForceResendingToken& ForceResendingToken)
{
	UVerifyPhoneNumberProxy* const Proxy = NewObject<UVerifyPhoneNumberProxy>();

	Proxy->PhoneNumber						= PhoneNumber;
	Proxy->AutoVerifyTimeoutInMilliseconds	= AutoVerifyTimeoutInMilliseconds;
	Proxy->ForceResendingToken				= ForceResendingToken;

	return Proxy;
}

void UVerifyPhoneNumberProxy::Activate()
{
	UCredentialLibrary::VerifyPhoneNumber
	(
		PhoneNumber,
		AutoVerifyTimeoutInMilliseconds,
		FCredentialCallback				 ::CreateUObject(this, &UVerifyPhoneNumberProxy::Credential),
		FVerificationFailedCallback		 ::CreateUObject(this, &UVerifyPhoneNumberProxy::Failed),
		FCodeSentCallback				 ::CreateUObject(this, &UVerifyPhoneNumberProxy::Sent),
		FCodeAutoRetrievalTimeoutCallback::CreateUObject(this, &UVerifyPhoneNumberProxy::Timeout),
		ForceResendingToken
	);
}

void UVerifyPhoneNumberProxy::Credential(const FCredential& Credential)
{
	OnVerificationCompleted.Broadcast(Credential, TEXT(""), FForceResendingToken(), TEXT(""));
	SetReadyToDestroy();
}

void UVerifyPhoneNumberProxy::Timeout(const FString& VerifId)
{
	OnCodeRetrievalTimeout.Broadcast(FCredential(), VerifId, FForceResendingToken(), TEXT("Code retrieval timeout."));
}

void UVerifyPhoneNumberProxy::Failed(const FString& Error)
{
	OnVerificationCompleted.Broadcast(FCredential(), TEXT(""), FForceResendingToken(), Error);
	SetReadyToDestroy();
}

void UVerifyPhoneNumberProxy::Sent(const FString& VerifId, const FForceResendingToken& Token)
{
	OnCodeSent.Broadcast(FCredential(), VerifId, Token, TEXT(""));
}
