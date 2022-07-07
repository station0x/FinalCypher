// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Analytics/FirebaseAnalyticsLibrary.h"

#include "FirebaseFeatures.h"

#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
#	include "Analytics.h"
#	include "Analytics/FirebaseAnalyticsProvider.h"
#else // !FIREBASE_FEATURES_UE_4_25_OR_NEWER
#	include "Analytics/FirebaseAnalyticsProvider.h"
#endif // !FIREBASE_FEATURES_UE_4_25_OR_NEWER

#if WITH_FIREBASE_ANALYTICS
	THIRD_PARTY_INCLUDES_START
#		include "firebase/analytics.h"
	THIRD_PARTY_INCLUDES_END
#endif

#include "Async/Async.h"

// Prevents crash when trying to access Analytics without having started the session.
#define FIREBASE_ANALYTICS_GUARD_SESSION_STARTED(...) do									\
{																							\
	if (!GetAnalyticsProvider() || !GetAnalyticsProvider()->HasAnalyticSessionStarted())	\
	{																						\
		UE_LOG(LogFirebaseAnalytics, Error,													\
			TEXT("Firebase Analytics has not been started. Start it before using it."));	\
	}																						\
} while(0)

TSharedPtr<FFirebaseAnalyticsProvider> UFirebaseAnalyticsLibrary::GetAnalyticsProvider()
{
	static const FName FirebaseFeaturesModuleName = TEXT("FirebaseFeatures");
	return FModuleManager::Get().GetModuleChecked<FFirebaseFeaturesModule>(FirebaseFeaturesModuleName).AnalyticsProvider;
}

bool UFirebaseAnalyticsLibrary::StartSession()
{
#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
	if (FAnalytics::ConfigFromIni::GetDefaultProviderModuleName() != TEXT("FirebaseFeatures"))
	{
		UE_LOG(LogFirebaseAnalytics, Error, 
			TEXT("Unreal Engine's Analytics Provider is not set. Sets `ProviderModuleName` to `FirebaseAnalytics` in `DefaultEngine.ini`. Current provider name: %s"),
			*FAnalytics::ConfigFromIni::GetDefaultProviderModuleName().ToString());
		return false;
	}

	TSharedPtr<IAnalyticsProvider> Provider = FAnalytics::Get().GetDefaultConfiguredProvider();
#else // !FIREBASE_FEATURES_UE_4_25_OR_NEWER
	TSharedPtr<FFirebaseAnalyticsProvider> Provider = GetAnalyticsProvider();
#endif // !FIREBASE_FEATURES_UE_4_25_OR_NEWER

	if (!Provider.IsValid())
	{
		UE_LOG(LogFirebaseAnalytics, Error, 
			TEXT("Unreal Engine's Analytics Provider is not set. Sets `ProviderModuleName` to `FirebaseAnalytics` in `DefaultEngine.ini`. Current provider name: %s"),
			*FAnalytics::ConfigFromIni::GetDefaultProviderModuleName().ToString());
		return false;
	}

	const bool bResult = Provider->StartSession();

	if (bResult)
	{
		UE_LOG(LogFirebaseAnalytics, Log, TEXT("Analytics session started."));
	}
	else
	{
		UE_LOG(LogFirebaseAnalytics, Warning, TEXT("Analytics session didn't start."));
	}

	return bResult;
}

void UFirebaseAnalyticsLibrary::EndSession()
{
#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
	TSharedPtr<IAnalyticsProvider> Provider = FAnalytics::Get().GetDefaultConfiguredProvider();
#else // !FIREBASE_FEATURES_UE_4_25_OR_NEWER
	TSharedPtr<FFirebaseAnalyticsProvider> Provider = GetAnalyticsProvider();
#endif // !FIREBASE_FEATURES_UE_4_25_OR_NEWER

	if (Provider.IsValid())
	{
		Provider->EndSession();
		UE_LOG(LogFirebaseAnalytics, Log, TEXT("Analytics session terminated."));
	}
}

void UFirebaseAnalyticsLibrary::ResetAnalyticsData()
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::ResetAnalyticsData();
	
	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Analytics data reset."));
#endif
}

void UFirebaseAnalyticsLibrary::SetAnalyticsCollectionEnabled(const bool bEnabled)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::SetAnalyticsCollectionEnabled(bEnabled);

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Analytics collection enabled: %d."), (int32)bEnabled);
#endif
}

void UFirebaseAnalyticsLibrary::LogEventString(const FString& EventName, const FString& ParameterName, const FString& ParameterValue)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::LogEvent(TCHAR_TO_UTF8(*EventName), TCHAR_TO_UTF8(*ParameterName), TCHAR_TO_UTF8(*ParameterValue));

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("String event %s logged."), *EventName);
#endif
}

void UFirebaseAnalyticsLibrary::LogEventFloat(const FString& EventName, const FString& ParameterName, const float ParameterValue)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::LogEvent(TCHAR_TO_UTF8(*EventName), TCHAR_TO_UTF8(*ParameterName), ParameterValue);

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Float event %s logged."), *EventName);
#endif
}

void UFirebaseAnalyticsLibrary::LogEventInt64(const FString& EventName, const FString& ParameterName, const int64 ParameterValue)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::LogEvent(TCHAR_TO_UTF8(*EventName), TCHAR_TO_UTF8(*ParameterName), (int64_t)ParameterValue);

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Int64 event %s logged."), *EventName);
#endif
}

void UFirebaseAnalyticsLibrary::LogEventInt32(const FString& EventName, const FString& ParameterName, const int32 ParameterValue)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::LogEvent(TCHAR_TO_UTF8(*EventName), TCHAR_TO_UTF8(*ParameterName), ParameterValue);

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Event %s logged."), *EventName);
#endif
}

void UFirebaseAnalyticsLibrary::LogEvent(const FString& EventName)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::LogEvent(TCHAR_TO_UTF8(*EventName));

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Event %s logged."), *EventName);
#endif
}

void UFirebaseAnalyticsLibrary::LogEventWithParameter(const FString& EventName, const TMap<FString, FString>& Parameters)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	const int32 ParametersCount = Parameters.Num();

	if (ParametersCount <= 0)
	{
		return;
	}

	TUniquePtr<firebase::analytics::Parameter[]> RawParameters = MakeUnique<firebase::analytics::Parameter[]>(ParametersCount);

	TArray<FTCHARToUTF8> Converters;
	Converters.Reserve(Parameters.Num() * 2);

	{
		int32 i = 0;

		for (const auto& Parameter : Parameters)
		{
			Converters.Emplace(*Parameter.Key);
			Converters.Emplace(*Parameter.Value);

			RawParameters[i] = firebase::analytics::Parameter
			(
				Converters[2 * i    ].Get(),
				Converters[2 * i + 1].Get()
			);

			++i;
		}
	}

	firebase::analytics::LogEvent(TCHAR_TO_UTF8(*EventName), RawParameters.Get(), ParametersCount);	

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Event %s logged with parameters."), *EventName);
#endif
}

void UFirebaseAnalyticsLibrary::SetUserProperty(const FString& Name, const FString& Property)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::SetUserProperty(TCHAR_TO_UTF8(*Name), TCHAR_TO_UTF8(*Property));

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("User property %s set."), *Name);
#endif
}

void UFirebaseAnalyticsLibrary::SetUserId(const FString& UserId)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::SetUserId(TCHAR_TO_UTF8(*UserId));

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("User ID %s set."), *UserId);
#endif
}

void UFirebaseAnalyticsLibrary::SetSessionTimeoutDuration(const int64 Milliseconds)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();

	firebase::analytics::SetSessionTimeoutDuration(Milliseconds);

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Session timeout duration changed to %d."), (int32)Milliseconds);
#endif
}

void UFirebaseAnalyticsLibrary::SetCurrentScreen(const FString& ScreenName, const FString& ScreenClass)
{
#if WITH_FIREBASE_ANALYTICS
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED();
	
	firebase::analytics::SetCurrentScreen(TCHAR_TO_UTF8(*ScreenName), TCHAR_TO_UTF8(*ScreenClass));

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Current screen set to %d of class %s."), *ScreenName, *ScreenClass);
#endif
#endif
}

void UFirebaseAnalyticsLibrary::GetAnalyticsInstanceId(const FFirebaseAnalyticsStringCallback& Callback)
{
#if WITH_FIREBASE_ANALYTICS
	FIREBASE_ANALYTICS_GUARD_SESSION_STARTED((void)Callback.ExecuteIfBound(0, TEXT("")));

	firebase::analytics::GetAnalyticsInstanceId()
	
	.OnCompletion([Callback](const firebase::Future<std::string>& _Future) -> void
	{
		FString InstanceId;

		const int32 Error = _Future.error();

		if (Error)
		{
			UE_LOG(LogFirebaseAnalytics, Error, TEXT("Failed to get analytics instance ID. Reason: %s"), TCHAR_TO_UTF8(_Future.error_message()));
		}
		else if (_Future.result())
		{
			InstanceId = UTF8_TO_TCHAR(_Future.result()->c_str());
		}

		AsyncTask(ENamedThreads::GameThread, [Callback, _InstanceId = MoveTemp(InstanceId), Error]() -> void
		{
			Callback.ExecuteIfBound(Error, _InstanceId);
		});
	});
#endif
}


#undef FIREBASE_ANALYTICS_GUARD_SESSION_STARTED
