// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Analytics/FirebaseAnalyticsProvider.h"

#if WITH_FIREBASE_ANALYTICS
	THIRD_PARTY_INCLUDES_START
#		include "firebase/analytics.h"
#		if !PLATFORM_LINUX
#			include "firebase/analytics/event_names.h"
#			include "firebase/analytics/parameter_names.h"
#			include "firebase/analytics/user_property_names.h"
#		endif
	THIRD_PARTY_INCLUDES_END
#endif

#include "FirebaseSdk/FirebaseConfig.h"

DEFINE_LOG_CATEGORY(LogFirebaseAnalytics);

FFirebaseAnalyticsProvider::FFirebaseAnalyticsProvider()
	: bAnalyticSessionStarted(false)
{
}

FFirebaseAnalyticsProvider::~FFirebaseAnalyticsProvider()
{
}

bool FFirebaseAnalyticsProvider::StartSession(const TArray<FAnalyticsEventAttribute>& Attributes)
{
#if WITH_FIREBASE_ANALYTICS
	if (FFirebaseFeaturesModule::GetCurrentFirebaseApp() == nullptr)
	{
		UE_LOG(LogFirebaseAnalytics, Error, TEXT("Tried to start Analytics Session without a valid Firebase App created. Analytics won't be functional."));
		return false;
	}

	firebase::analytics::Initialize(*FFirebaseFeaturesModule::GetCurrentFirebaseApp());
	firebase::analytics::SetAnalyticsCollectionEnabled(true);
	firebase::analytics::SetSessionTimeoutDuration(1000LL * GetDefault<UFirebaseConfig>()->FirebaseAnalyticsSessionTimeoutDuration);

	bAnalyticSessionStarted = true;

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Firebase Analytics session started."));

	return true;
#else
	return false;
#endif
}

void FFirebaseAnalyticsProvider::EndSession()
{
#if WITH_FIREBASE_ANALYTICS
	if (bAnalyticSessionStarted)
	{
		firebase::analytics::Terminate();
		
		bAnalyticSessionStarted = false;
	}
#endif
}

void FFirebaseAnalyticsProvider::RecordEvent(const FString& EventName, const TArray<FAnalyticsEventAttribute>& Attributes)
{
#if WITH_FIREBASE_ANALYTICS
	if (!bAnalyticSessionStarted)
	{
		UE_LOG(LogFirebaseAnalytics, Error, TEXT("Tried to record an event but the Firebase Analytics session hasn't been started."));
		return;
	}

	if (FFirebaseFeaturesModule::GetCurrentFirebaseApp())
	{
		firebase::analytics::LogEvent(TCHAR_TO_UTF8(*EventName));
	}
	else
	{
		UE_LOG(LogFirebaseAnalytics, Warning, TEXT("Failed to log event \"%s\": Invalid Firebase App Instance."), *EventName);
	}
#endif
}

FString FFirebaseAnalyticsProvider::GetSessionID() const
{
	return TEXT("FIREBASE_NO_SESSION_ID");
}

bool FFirebaseAnalyticsProvider::SetSessionID(const FString& InSessionID)
{
	return false;
}

void FFirebaseAnalyticsProvider::FlushEvents()
{
}

void FFirebaseAnalyticsProvider::SetUserID(const FString& InUserID)
{	
}

FString FFirebaseAnalyticsProvider::GetUserID() const
{
	return TEXT("FIREBASE_NO_USER_ID");
}
