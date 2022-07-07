// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "FirebaseInstallationsNode.h"

UFirebaseGetInstallationIDProxy* UFirebaseGetInstallationIDProxy::GetInstallationID()
{
	return NewObject<ThisClass>();
}

void UFirebaseGetInstallationIDProxy::Activate()
{
	UFirebaseInstallations::GetInstallationID(FFirebaseInstallationIDCallback::CreateUObject(this, &ThisClass::OnTaskOver));
}

void UFirebaseGetInstallationIDProxy::OnTaskOver(FString ID, int32 Error, FString ErrorMessage)
{
	(Error == 0 ? Done : Failed).Broadcast(ID, Error, ErrorMessage);
	SetReadyToDestroy();
}

UFirebaseGetInstallationAuthTokenProxy* UFirebaseGetInstallationAuthTokenProxy::GetInstallationAuthToken(const bool bForceRefresh)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->bForceRefresh = bForceRefresh;

	return Proxy;
}

void UFirebaseGetInstallationAuthTokenProxy::Activate()
{
	UFirebaseInstallations::GetAuthToken(bForceRefresh, FFirebaseInstallationAuthTokenCallback::CreateUObject(this, &ThisClass::OnTaskOver));
}

void UFirebaseGetInstallationAuthTokenProxy::OnTaskOver(FFirebaseInstallationsAuthToken Token, int32 Error, FString ErrorMessage)
{
	(Error == 0 ? Done : Failed).Broadcast(Token, Error, ErrorMessage);
	SetReadyToDestroy();
}

UFirebaseDeleteInstallationProxy* UFirebaseDeleteInstallationProxy::DeleteInstallation()
{
	return NewObject<ThisClass>();
}

void UFirebaseDeleteInstallationProxy::Activate()
{
	UFirebaseInstallations::Delete(FFirebaseInstallationDeleteCallback::CreateUObject(this, &ThisClass::OnTaskOver));
}

void UFirebaseDeleteInstallationProxy::OnTaskOver(int32 Error, FString ErrorMessage)
{
	(Error == 0 ? Done : Failed).Broadcast(Error, ErrorMessage);
	SetReadyToDestroy();
}
