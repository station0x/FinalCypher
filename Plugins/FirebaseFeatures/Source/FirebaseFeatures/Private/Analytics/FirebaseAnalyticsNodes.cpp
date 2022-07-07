// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Analytics/FirebaseAnalyticsNodes.h"
#include "Analytics/FirebaseAnalyticsLibrary.h"

UGetAnalyticsInstanceIdProxy::UGetAnalyticsInstanceIdProxy() 
	: Super()
{
}

UGetAnalyticsInstanceIdProxy* UGetAnalyticsInstanceIdProxy::GetAnalyticsInstanceId()
{
	UGetAnalyticsInstanceIdProxy* const Proxy = NewObject<UGetAnalyticsInstanceIdProxy>();

	return Proxy;
}

void UGetAnalyticsInstanceIdProxy::Activate()
{
	UFirebaseAnalyticsLibrary::GetAnalyticsInstanceId(FFirebaseAnalyticsStringCallback::CreateUObject(this, &UGetAnalyticsInstanceIdProxy::OnActionOver));
}

void UGetAnalyticsInstanceIdProxy::OnActionOver(const int32 Error, const FString& InstanceId)
{
	OnInstanceIdReceived.Broadcast(InstanceId);
	SetReadyToDestroy();
}
