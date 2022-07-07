// Copyright Pandores Marketplace 2022. All Rights Reserved.
#include "FirebaseSdk/FirebaseConfig.h"

UFirebaseConfig::UFirebaseConfig()
{
	LoadConfig();
}

const UFirebaseConfig* UFirebaseConfig::Get()
{
	return GetDefault<UFirebaseConfig>();
}
