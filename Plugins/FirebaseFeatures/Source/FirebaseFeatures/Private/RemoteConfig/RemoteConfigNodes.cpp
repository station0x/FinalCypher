// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "RemoteConfigNodes.h"

#include "FirebaseFeatures.h"

UFetchProxy* UFetchProxy::Fetch()
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	return Proxy;
}

void UFetchProxy::Activate()
{
#if WITH_FIREBASE_REMOTE_CONFIG
	UFirebaseRemoteConfig::Fetch(FRemoteConfigCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(-1);
#endif
}

void UFetchProxy::OnActionOver(int32 Code)
{
	(Code == 0 ? OnFetched : OnFailed).Broadcast();
	SetReadyToDestroy();
}

UFetchWithCacheExpirationProxy* UFetchWithCacheExpirationProxy::FetchWithCacheExpiration(const int32 CacheExpirationInSeconds)
{
	ThisClass* const Proxy = NewObject<ThisClass>();

	Proxy->_CacheExpirationInSeconds = CacheExpirationInSeconds;

	return Proxy;
}

void UFetchWithCacheExpirationProxy::Activate()
{
#if WITH_FIREBASE_REMOTE_CONFIG
	UFirebaseRemoteConfig::Fetch(_CacheExpirationInSeconds, FRemoteConfigCallback::CreateUObject(this, &ThisClass::OnActionOver));
#else
	OnActionOver(-1);
#endif
}

void UFetchWithCacheExpirationProxy::OnActionOver(int32 Code)
{
	(Code == 0 ? OnFetched : OnFailed).Broadcast();
	SetReadyToDestroy();
}






