// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "AdMob/FbBannerView.h"

#if PLATFORM_ANDROID
#	include "Android/AndroidJNI.h"
#	include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

#if PLATFORM_IOS
#	include "IOS/IOSAppDelegate.h"
#endif // PLATFORM_IOS

THIRD_PARTY_INCLUDES_START
#	include "firebase/admob/types.h"
THIRD_PARTY_INCLUDES_END

#include "FirebaseSdk/FirebaseConfig.h"

#include <string>

#define ADMOB_CONVERT_AD_REQUEST(Req) FAdMobHelper::FInitAdRequestMemoryHandler(Req).Get()

namespace FAdMobHelper
{
	class FInitAdRequestMemoryHandler
	{
	public:
		FInitAdRequestMemoryHandler(const FAdMobAdRequest& InReq);
		FInitAdRequestMemoryHandler(const FInitAdRequestMemoryHandler& ) = delete;
		FInitAdRequestMemoryHandler(const FInitAdRequestMemoryHandler&&) = delete;

		~FInitAdRequestMemoryHandler();

		const firebase::admob::AdRequest& Get();

	private:
		const FAdMobAdRequest& Req;
		firebase::admob::AdRequest RawReq;
		TArray<std::string> Keywords;
	};

	firebase::admob::AdParent GetAdParent();
};

