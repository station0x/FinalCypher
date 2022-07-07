// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Messaging/MessagingNodes.h"

void UFirebaseMesssagingBlueprintLibrary::Terminate()
{
	FFirebaseMessagingLibrary::Terminate();
}

bool UFirebaseMesssagingBlueprintLibrary::IsTokenRegistrationOnInitEnabled()
{
	return FFirebaseMessagingLibrary::IsTokenRegistrationOnInitEnabled();
}

void UFirebaseMesssagingBlueprintLibrary::SetTokenRegistrationOnInitEnabled(const bool bEnable)
{
	FFirebaseMessagingLibrary::SetTokenRegistrationOnInitEnabled(bEnable);
}

void UFirebaseMesssagingBlueprintLibrary::Send(const FFirebaseMessage& Message)
{
	PRAGMA_DISABLE_DEPRECATION_WARNINGS;
	FFirebaseMessagingLibrary::Send(Message);
	PRAGMA_ENABLE_DEPRECATION_WARNINGS;
}

bool UFirebaseMesssagingBlueprintLibrary::IsDeliveryMetricsExportToBigQueryEnabled()
{
	return FFirebaseMessagingLibrary::IsDeliveryMetricsExportToBigQueryEnabled();
}

void UFirebaseMesssagingBlueprintLibrary::SetDeliveryMetricsExportToBigQuery(const bool bEnable)
{
	FFirebaseMessagingLibrary::SetDeliveryMetricsExportToBigQuery(bEnable);
}

FString UFirebaseMesssagingBlueprintLibrary::GetCachedCloudMessagingToken()
{
	return FFirebaseMessagingLibrary::GetCachedCloudMessagingToken();
}

FFirebaseMessage UFirebaseMesssagingBlueprintLibrary::GetLastMessage()
{
	return FFirebaseMessagingLibrary::GetCachedLastMessage();
}

#define CALL_CALLBACK(OnSuccess) do															\
	{																						\
		(Error == EFirebaseMessagingError::None ? OnSuccess : OnError).Broadcast(Error);	\
		SetReadyToDestroy();																\
	} while (0)																				


URequestPermissionProxy* URequestPermissionProxy::RequestPermission()
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	return Proxy;
}

void URequestPermissionProxy::Activate()
{
	FFirebaseMessagingLibrary::RequestPermission(FFirebaseMessagingCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void URequestPermissionProxy::OnActionOver(const EFirebaseMessagingError Error)
{
	CALL_CALLBACK(OnRequestOver);
}

USubscribeProxy* USubscribeProxy::Subscribe(const FString& Topic)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Topic = Topic;

	return Proxy;
}

void USubscribeProxy::Activate()
{
	FFirebaseMessagingLibrary::Subscribe(_Topic, FFirebaseMessagingCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void USubscribeProxy::OnActionOver(const EFirebaseMessagingError Error)
{
	CALL_CALLBACK(OnSubscribed);
}

UUnsubscribeProxy* UUnsubscribeProxy::Unsubscribe(const FString& Topic)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_Topic = Topic;

	return Proxy;
}

void UUnsubscribeProxy::Activate()
{
	FFirebaseMessagingLibrary::Unsubscribe(_Topic, FFirebaseMessagingCallback::CreateUObject(this, &ThisClass::OnActionOver));
}

void UUnsubscribeProxy::OnActionOver(const EFirebaseMessagingError Error)
{
	CALL_CALLBACK(OnUnsubscribed);
}


UMessagingEventsProxy* UMessagingEventsProxy::ListenForMessagingEvents()
{
	static TSoftObjectPtr<ThisClass> Singleton;

	if (!Singleton.IsValid())
	{
		Singleton = NewObject<ThisClass>();
	}

	return Singleton.Get();
}

void UMessagingEventsProxy::Activate()
{
	FFirebaseMessagingLibrary::OnMessage()      .AddUObject(this, &ThisClass::OnMessageInternal);
	FFirebaseMessagingLibrary::OnTokenReceived().AddUObject(this, &ThisClass::OnTokenInternal);
}

void UMessagingEventsProxy::OnMessageInternal(const FFirebaseMessage& Message)
{
	OnMessage.Broadcast(Message, CachedToken);
}

void UMessagingEventsProxy::OnTokenInternal(const FString& Token)
{
	CachedToken = Token;
	OnTokenReceived.Broadcast(FFirebaseMessage(), Token);
}


#undef CALL_CALLBACK
