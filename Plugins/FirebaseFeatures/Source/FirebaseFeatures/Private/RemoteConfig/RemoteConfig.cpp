// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "RemoteConfig/RemoteConfig.h"
#include "FirebaseFeatures.h"

#if WITH_FIREBASE_REMOTE_CONFIG
THIRD_PARTY_INCLUDES_START
#	include "firebase/remote_config.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "Async/Async.h"

// Linux uses another version of the SDK where the static method version of Remote Config 
// is deprecated. 
// We'll remove this when the SDK is updated for all platforms and we use instace-base.
#if PLATFORM_LINUX
#	pragma clang diagnostic push
#	pragma clang diagnostic ignored "-Wdeprecated-declarations"
#endif

#if WITH_FIREBASE_REMOTE_CONFIG
#if !FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
static firebase::remote_config::RemoteConfig* GetInstance()
{
	auto* const Inst = firebase::remote_config::RemoteConfig::GetInstance(FFirebaseFeaturesModule::GetCurrentFirebaseApp());
	check(Inst);
	return Inst;
}
#endif
#endif

void UFirebaseRemoteConfig::Terminate()
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	firebase::remote_config::Terminate();
#else
	delete GetInstance();
#endif
#endif
}

#if PLATFORM_ANDROID && WITH_FIREBASE_REMOTE_CONFIG
void UFirebaseRemoteConfig::SetDefaults(const int32 DefaultsResourceId)
{
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	firebase::remote_config::SetDefaults(DefaultsResourceId);
#else
	GetInstance()->SetDefaults(DefaultsResourceId);
#endif
}
#endif // PLATFORM_ANDROID

void UFirebaseRemoteConfig::SetDefaultsWithVariant(const TArray<FFirebaseConfigKeyValueVariant>& Defaults)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	TArray<firebase::remote_config::ConfigKeyValueVariant> RawDefaults;
	RawDefaults.AddUninitialized(Defaults.Num());

	// These should outlive SetDefaults call. TCHAR_TO_UTF8 macro can't achieve that.
	TArray<FTCHARToUTF8> Converters;
	Converters.Reserve(Defaults.Num());

	for (int32 i = 0; i < Defaults.Num(); ++i)
	{
		RawDefaults[i].key = Converters.Add_GetRef(FTCHARToUTF8(*Defaults[i].Key)).Get();
		RawDefaults[i].value = Defaults[i].Value;
	}

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	firebase::remote_config::SetDefaults(RawDefaults.GetData(), RawDefaults.Num());
#else
	GetInstance()->SetDefaults(RawDefaults.GetData(), RawDefaults.Num());
#endif
#endif
}

void UFirebaseRemoteConfig::SetDefaults(const TArray<FFirebaseConfigKeyValue>& Defaults)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	TArray<firebase::remote_config::ConfigKeyValue> RawDefaults;
	RawDefaults.AddUninitialized(Defaults.Num());

	// These should outlive SetDefaults call. TCHAR_TO_UTF8 macro can't achieve that.
	TArray<FTCHARToUTF8> Converters;
	Converters.Reserve(Defaults.Num() * 2);

	for (int32 i = 0; i < Defaults.Num(); ++i)
	{
		RawDefaults[i].key   = Converters.Add_GetRef(FTCHARToUTF8(*Defaults[i].Key)).Get();
		RawDefaults[i].value = Converters.Add_GetRef(FTCHARToUTF8(*Defaults[i].Value)).Get();
	}

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	firebase::remote_config::SetDefaults(RawDefaults.GetData(), RawDefaults.Num());
#else
	GetInstance()->SetDefaults(RawDefaults.GetData(), RawDefaults.Num());
#endif
#endif
}

FString UFirebaseRemoteConfig::GetConfigSetting(EFirebaseConfigSetting Setting)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return UTF8_TO_TCHAR(firebase::remote_config::GetConfigSetting((firebase::remote_config::ConfigSetting)Setting).c_str());
#else
	return TEXT("");
#endif
#else
	return {};
#endif
}

void UFirebaseRemoteConfig::SetConfigSetting(EFirebaseConfigSetting setting, const FString& Value)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	firebase::remote_config::SetConfigSetting(
		(firebase::remote_config::ConfigSetting)setting, TCHAR_TO_UTF8(*Value));
#else
	// No method available.
#endif
#endif
}

bool UFirebaseRemoteConfig::GetBoolean(const FString& Key)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return firebase::remote_config::GetBoolean(TCHAR_TO_UTF8(*Key));
#else
	return GetInstance()->GetBoolean(TCHAR_TO_UTF8(*Key));
#endif
#else 
	return false;
#endif
}

bool UFirebaseRemoteConfig::GetBooleanWithInfo(const FString& Key, FFirebaseValueInfo& Info)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	firebase::remote_config::ValueInfo RawInfo;

	RawInfo.conversion_successful	= Info.bConversionSuccessful;
	RawInfo.source					= (firebase::remote_config::ValueSource)Info.Source;

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return firebase::remote_config::GetBoolean(TCHAR_TO_UTF8(*Key), &RawInfo);
#else
	return GetInstance()->GetBoolean(TCHAR_TO_UTF8(*Key), &RawInfo);
#endif
#else
	return false;
#endif
}

uint8 UFirebaseRemoteConfig::GetByte(const FString& Key)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	return (uint8)GetInt64(Key);
#else
	return 0;
#endif
}

int32 UFirebaseRemoteConfig::GetInt32(const FString& Key)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	return (int32)GetInt64(Key);
#else
	return 0;
#endif
}

int64 UFirebaseRemoteConfig::GetInt64(const FString& Key)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return firebase::remote_config::GetLong(TCHAR_TO_UTF8(*Key));
#else
	return GetInstance()->GetLong(TCHAR_TO_UTF8(*Key));
#endif
#else
	return 0;
#endif
}

int64 UFirebaseRemoteConfig::GetInt64WithInfo(const FString& Key, FFirebaseValueInfo& Info)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	firebase::remote_config::ValueInfo RawInfo;

	RawInfo.conversion_successful = Info.bConversionSuccessful;
	RawInfo.source = (firebase::remote_config::ValueSource)Info.Source;

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return firebase::remote_config::GetLong(TCHAR_TO_UTF8(*Key), &RawInfo);
#else
	return GetInstance()->GetLong(TCHAR_TO_UTF8(*Key), &RawInfo);
#endif
#else
	return 0;
#endif
}

float UFirebaseRemoteConfig::GetFloat(const FString& Key)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return (float)firebase::remote_config::GetDouble(TCHAR_TO_UTF8(*Key));
#else
	return (float)GetInstance()->GetDouble(TCHAR_TO_UTF8(*Key));
#endif
#else
	return 0.f;
#endif
}

float UFirebaseRemoteConfig::GetFloatWithInfo(const FString& Key, FFirebaseValueInfo& Info)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	firebase::remote_config::ValueInfo RawInfo;

	RawInfo.conversion_successful = Info.bConversionSuccessful;
	RawInfo.source = (firebase::remote_config::ValueSource)Info.Source;

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return (float)firebase::remote_config::GetDouble(TCHAR_TO_UTF8(*Key), &RawInfo);
#else
	return (float)GetInstance()->GetDouble(TCHAR_TO_UTF8(*Key), &RawInfo);
#endif
#else
	return 0.f;
#endif
}

FString UFirebaseRemoteConfig::GetString(const FString& Key)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return UTF8_TO_TCHAR(firebase::remote_config::GetString(TCHAR_TO_UTF8(*Key)).c_str());
#else
	return UTF8_TO_TCHAR(GetInstance()->GetString(TCHAR_TO_UTF8(*Key)).c_str());
#endif
#else
	return {};
#endif
}

FString UFirebaseRemoteConfig::GetStringWithInfo(const FString& Key, FFirebaseValueInfo& Info)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	firebase::remote_config::ValueInfo RawInfo;

	RawInfo.conversion_successful = Info.bConversionSuccessful;
	RawInfo.source = (firebase::remote_config::ValueSource)Info.Source;

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return UTF8_TO_TCHAR(firebase::remote_config::GetString(TCHAR_TO_UTF8(*Key), &RawInfo).c_str());
#else
	return UTF8_TO_TCHAR(GetInstance()->GetString(TCHAR_TO_UTF8(*Key), &RawInfo).c_str());
#endif
#else
	return {};
#endif
}

TArray<uint8> UFirebaseRemoteConfig::GetData(const FString& Key)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	const auto Data = firebase::remote_config::GetData(TCHAR_TO_UTF8(*Key));
#else
	const auto Data = GetInstance()->GetData(TCHAR_TO_UTF8(*Key));
#endif

	return TArray<uint8>(Data.data(), Data.size());
#else
	return {};
#endif
}

TArray<uint8> UFirebaseRemoteConfig::GetDataWithInfo(const FString& Key, FFirebaseValueInfo& Info)
{
#if WITH_FIREBASE_REMOTE_CONFIG
	firebase::remote_config::ValueInfo RawInfo;

	RawInfo.conversion_successful = Info.bConversionSuccessful;
	RawInfo.source = (firebase::remote_config::ValueSource)Info.Source;

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	const auto Data = firebase::remote_config::GetData(TCHAR_TO_UTF8(*Key), &RawInfo);
#else
	const auto Data = GetInstance()->GetData(TCHAR_TO_UTF8(*Key), &RawInfo);
#endif

	return TArray<uint8>(Data.data(), Data.size());
#else
	return {};
#endif
}

TArray<FString> UFirebaseRemoteConfig::GetKeysByPrefix(const FString& Prefix)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	const auto RawData = firebase::remote_config::GetKeysByPrefix(TCHAR_TO_UTF8(*Prefix));
#else
	const auto RawData = GetInstance()->GetKeysByPrefix(TCHAR_TO_UTF8(*Prefix));
#endif
	
	TArray<FString> Data;

	Data.Reserve(RawData.size());
	for (const auto& DataElem : RawData)
	{
		Data.Emplace(UTF8_TO_TCHAR(DataElem.c_str()));
	}

	return Data;
#else
	return {};
#endif
}

TArray<FString> UFirebaseRemoteConfig::GetKeys()
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	const auto RawKeys = firebase::remote_config::GetKeys();
#else
	const auto RawKeys = GetInstance()->GetKeys();
#endif

	TArray<FString> Keys;

	Keys.Reserve(RawKeys.size());
	for (const auto& KeyElem : RawKeys)
	{
		Keys.Emplace(UTF8_TO_TCHAR(KeyElem.c_str()));
	}

	return Keys;
#else
	return {};
#endif
}

void UFirebaseRemoteConfig::Fetch(const FRemoteConfigCallback& Callback)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	firebase::remote_config::Fetch()
#else
	GetInstance()->Fetch()
#endif
		.OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		const int32 Error = Future.error();
		if (Error != 0)
		{
			UE_LOG(LogFirebaseRemoteConfig, Error, TEXT("Failed to fetch remote config: %s."),
				UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Error, Callback]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif
}

void UFirebaseRemoteConfig::Fetch(const int64 CacheExpirationInSeconds, const FRemoteConfigCallback& Callback)
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	firebase::remote_config::Fetch(CacheExpirationInSeconds)
#else
	GetInstance()->Fetch(CacheExpirationInSeconds)
#endif
		.OnCompletion([Callback](const firebase::Future<void>& Future) -> void
	{
		const int32 Error = Future.error();
		if (Error != 0)
		{
			UE_LOG(LogFirebaseRemoteConfig, Error, TEXT("Failed to fetch remote config: %s."),
				UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Error, Callback]() -> void
			{
				Callback.ExecuteIfBound(Error);
			});
		}
	});
#endif
}

void UFirebaseRemoteConfig::ActivateFetched(EFirebaseConfigFetchActivationBranch& Branch)
{
	Branch = ActivateFetched() ? EFirebaseConfigFetchActivationBranch::Activated : EFirebaseConfigFetchActivationBranch::NotActivated;
}

bool UFirebaseRemoteConfig::ActivateFetched()
{
#if WITH_FIREBASE_REMOTE_CONFIG
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	return firebase::remote_config::ActivateFetched();
#else
	GetInstance()->Activate();
	return true;
#endif
#else
	return false;
#endif
}

FFirebaseConfigInfo UFirebaseRemoteConfig::GetInfo()
{
#if WITH_FIREBASE_REMOTE_CONFIG
	FFirebaseConfigInfo ConfigInfo;

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	const auto& RawConfigInfo = firebase::remote_config::GetInfo();
#else
	const auto& RawConfigInfo = GetInstance()->GetInfo();
#endif

	ConfigInfo.FetchTime				= RawConfigInfo.fetch_time;
	ConfigInfo.LastFetchFailureReason	= (EFirebaseFetchFailureReason)RawConfigInfo.last_fetch_failure_reason;
	ConfigInfo.LastFetchStatus			= (EFirebaseLastFetchStatus)RawConfigInfo.last_fetch_status;
	ConfigInfo.ThrottledEndTime			= RawConfigInfo.throttled_end_time;

	return ConfigInfo;
#else
	return {};
#endif
}

#if PLATFORM_LINUX
#	pragma clang diagnostic pop
#endif
