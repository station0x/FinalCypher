// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "DynamicLinks/FirebaseDynamicLinksNodes.h"
#include "FirebaseFeatures.h"

FGeneratedDynamicLink UFirebaseDynamicLinksBlueprintLibrary::GetLongLink(const FDynamicLinkComponents& Components)
{
	return FDynamicLinksLibrary::GetLongLink(Components);
}

UBlueprintFirebaseDynamicLinksProxy* UBlueprintFirebaseDynamicLinksProxy::ListenOnDynamicLinkReceived()
{
	return NewObject<ThisClass>();
}

void UBlueprintFirebaseDynamicLinksProxy::Activate()
{
	FFirebaseFeaturesModule::OnDynamicLinkReceived().AddUObject(this, &ThisClass::OnLink);
}

void UBlueprintFirebaseDynamicLinksProxy::OnLink(const FString& URL, const ELinkMatchStrength MatchStrength)
{
	OnDynamicLinkReceived.Broadcast(URL, MatchStrength);
}

void UDynamicLinksGetShortLinkBase::OnTaskOver(bool bSuccess, const FGeneratedDynamicLink& Link)
{
	(bSuccess ? Success : Failed).Broadcast(Link);
	SetReadyToDestroy();
}

FDynamicLinksCallback UDynamicLinksGetShortLinkBase::GetCallback()
{
	return FDynamicLinksCallback::CreateUObject(this, &ThisClass::OnTaskOver);
}

void UDynamicLinksGetShortLinkWithComponents::Activate()
{
	FDynamicLinksLibrary::GetShortLink(Components, GetCallback());
}

UDynamicLinksGetShortLinkWithComponents* UDynamicLinksGetShortLinkWithComponents::GetShortLink(const FDynamicLinkComponents& Components)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Components = Components;

	return Proxy;
}

void UDynamicLinksGetShortLinkWithComponentsAndOptions::Activate()
{
	FDynamicLinksLibrary::GetShortLink(Components, Options, GetCallback());
}

UDynamicLinksGetShortLinkWithComponentsAndOptions* UDynamicLinksGetShortLinkWithComponentsAndOptions::GetShortLink(const FDynamicLinkComponents& Components, const FDynamicLinkOptions& Options)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->Components = Components;
	Proxy->Options = Options;

	return Proxy;
}

void UDynamicLinksGetShortLinkWithString::Activate()
{
	FDynamicLinksLibrary::GetShortLink(LongDynamicLink, GetCallback());
}

UDynamicLinksGetShortLinkWithString* UDynamicLinksGetShortLinkWithString::GetShortLink(const FString& LongDynamicLink)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->LongDynamicLink = LongDynamicLink;

	return Proxy;
}

void UDynamicLinksGetShortLinkWithStringAndOptions::Activate()
{
	FDynamicLinksLibrary::GetShortLink(LongDynamicLink, Options, GetCallback());
}

UDynamicLinksGetShortLinkWithStringAndOptions* UDynamicLinksGetShortLinkWithStringAndOptions::GetShortLink(const FString& LongDynamicLink, const FDynamicLinkOptions& Options)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->LongDynamicLink = LongDynamicLink;
	Proxy->Options = Options;

	return Proxy;
}
