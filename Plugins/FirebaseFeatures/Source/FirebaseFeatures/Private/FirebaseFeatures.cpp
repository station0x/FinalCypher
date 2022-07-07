// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "FirebaseFeatures.h"

#if WITH_EDITOR
#	include "ISettingsModule.h"
#	include "ISettingsSection.h"
#	include "GenericPlatform/GenericPlatformFile.h"
#	include "Interfaces/IPluginManager.h"
#	include "Misc/MessageDialog.h"
#	include "Misc/ConfigCacheIni.h"
#endif

#if PLATFORM_ANDROID
#	include "Android/AndroidJNI.h"
#	include "Android/AndroidApplication.h"
#endif // PLATFORM_ANDROID

#if PLATFORM_IOS
#	include "IOS/IOSAppDelegate.h"
#endif

THIRD_PARTY_INCLUDES_START
#	include "firebase/app.h"
#	include "firebase/log.h"
#	if WITH_FIREBASE_AUTH
#		include "firebase/auth.h"
#	endif
#	if WITH_FIREBASE_ANALYTICS
#		include "firebase/analytics.h"
#	endif
#	if WITH_FIREBASE_ADMOB
#		include "firebase/admob.h"
#		include "firebase/admob/rewarded_video.h"
#	endif
#	if WITH_FIREBASE_FUNCTIONS
#		include "firebase/functions.h"
#	endif
#	if WITH_FIREBASE_DATABASE
#		include "firebase/database.h"
#	endif
#	if WITH_FIREBASE_DYNAMIC_LINKS
#		include "firebase/dynamic_links.h"
#	endif
#	if WITH_FIREBASE_FIRESTORE
#		include "firebase/firestore.h"
#	endif
#	if WITH_FIREBASE_MESSAGING
#		include "firebase/messaging.h"
#	endif
#	if WITH_FIREBASE_REMOTE_CONFIG
#		include "firebase/remote_config.h"
#	endif
#	if WITH_FIREBASE_STORAGE
#		include "firebase/storage.h"
#	endif
THIRD_PARTY_INCLUDES_END

#if WITH_FIREBASE_ADMOB
#	include "AdMob/FbRewardedVideo.h"
#endif

#if WITH_FIREBASE_MESSAGING
#	include "Messaging/MessagingLibrary.h"
#endif

#include "FirebaseSdk/FirebaseConfig.h"
#include "FirebaseSdk/FirebaseApiConfig.h"

#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
#	include "Analytics/FirebaseAnalyticsProvider.h"
#else // !FIREBASE_FEATURES_UE_4_25_OR_NEWER
#	include "Analytics/FirebaseAnalyticsProvider.h"
#endif // !FIREBASE_FEATURES_UE_4_25_OR_NEWER

#include "Async/Async.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"

#define LOCTEXT_NAMESPACE "FFirebaseFeaturesModule"

#if ENGINE_MAJOR_VERSION >= 5
const char* const FirebaseFeaturesGameActivityPath = "com/epicgames/unreal/GameActivity";
#else
const char* const FirebaseFeaturesGameActivityPath = "com/epicgames/ue4/GameActivity";
#endif

DEFINE_LOG_CATEGORY(LogFirebaseSdk);
DEFINE_LOG_CATEGORY(LogFirebaseAuth);
DEFINE_LOG_CATEGORY(LogAdMob);
DEFINE_LOG_CATEGORY(LogFirebaseDatabase);
DEFINE_LOG_CATEGORY(LogFirebaseDynamicLinks);
DEFINE_LOG_CATEGORY(LogFirestore);
DEFINE_LOG_CATEGORY(LogFirebaseFunctions);
DEFINE_LOG_CATEGORY(LogFirebaseMessaging);
DEFINE_LOG_CATEGORY(LogFirebaseRemoteConfig);
DEFINE_LOG_CATEGORY(LogFirebaseStorage);
DEFINE_LOG_CATEGORY(LogFirebaseCrashlytics);
DEFINE_LOG_CATEGORY(LogFirebasePerformance);

FString FFirebaseFeaturesModule::CurrentFirebaseAppName;

FOnAuthEvent FFirebaseFeaturesModule::OnAuthStateChangedEvent;
FOnAuthEvent FFirebaseFeaturesModule::OnIdTokenChangedEvent;

FOnDynamicLinkReceived    FFirebaseFeaturesModule::OnDynamicLinkReceivedEvent;
FOnFirebaseSDKInitialized FFirebaseFeaturesModule::OnFirebaseSDKInitialized;

bool FFirebaseFeaturesModule::bIsSDKInitialized = false;

#define InitFirebaseModule(ModuleName)											\
	if (Config->bEnable ## ModuleName)											\
	{																			\
		Init ## ModuleName();													\
	}																			\
	else																		\
	{																			\
		UE_LOG(LogFirebaseSdk, Log,												\
			TEXT("Firebase module `%s` is disabled. Skipping initialization."), \
			TEXT(#ModuleName));													\
	}
	

class FDynamicLinksListener final 
#if WITH_FIREBASE_DYNAMIC_LINKS
	: public firebase::dynamic_links::Listener
#endif // WITH_FIREBASE_DYNAMIC_LINKS
{
public:
#if WITH_FIREBASE_DYNAMIC_LINKS
	void OnDynamicLinkReceived(const firebase::dynamic_links::DynamicLink* dynamic_link) override final
	{
		FString URL = UTF8_TO_TCHAR(dynamic_link->url.c_str());
		const ELinkMatchStrength Strength = (ELinkMatchStrength)dynamic_link->match_strength;

		UE_LOG(LogFirebaseDynamicLinks, Log, 
			TEXT("Dynamic link received. URL: %s."), *URL);

		AsyncTask(ENamedThreads::GameThread, [URL = MoveTemp(URL), Strength]() -> void
		{
			FFirebaseFeaturesModule::OnDynamicLinkReceived().Broadcast(URL, Strength);
		});
	}
#endif // WITH_FIREBASE_DYNAMIC_LINKS
};

#if WITH_FIREBASE_AUTH
class FAuthStateListener final : public firebase::auth::AuthStateListener
{
public:
	virtual ~FAuthStateListener()
	{
	}

	virtual void OnAuthStateChanged(firebase::auth::Auth* auth) override
	{
		UE_LOG(LogFirebaseAuth, Log, TEXT("Auth state has changed."));

		AsyncTask(ENamedThreads::GameThread, []() -> void
		{
			FFirebaseFeaturesModule::OnAuthStateChanged().Broadcast();
		});
	}		
};

class FIdTokenListener final : public firebase::auth::IdTokenListener
{
public:
	virtual ~FIdTokenListener()
	{
	}

	virtual void OnIdTokenChanged(firebase::auth::Auth* auth) override
	{
		UE_LOG(LogFirebaseAuth, Log, TEXT("ID token has changed."));

		AsyncTask(ENamedThreads::GameThread, []() -> void
		{
			FFirebaseFeaturesModule::OnIdTokenChanged().Broadcast();
		});
	}
};
#endif

class FMessagingListener final 
#if WITH_FIREBASE_MESSAGING
	: public firebase::messaging::Listener 
#endif // WITH_FIREBASE_MESSAGING
{
public:
	virtual ~FMessagingListener()
	{

	}
#if WITH_FIREBASE_MESSAGING
	virtual void OnMessage(const firebase::messaging::Message& message) override
	{
		UE_LOG(LogFirebaseMessaging, Log, TEXT("New message FCM message received."));
		
		AsyncTask(ENamedThreads::GameThread, [Message = FFirebaseMessage(&message)]() mutable -> void
		{
			FFirebaseMessagingLibrary::CachedLastMessage = MoveTemp(Message);
			FFirebaseMessagingLibrary::OnMessage().Broadcast(FFirebaseMessagingLibrary::CachedLastMessage);
		});
	}

	virtual void OnTokenReceived(const char* token) override
	{
		UE_LOG(LogFirebaseMessaging, Log, TEXT("Firebase Cloud Messaging token received."));
		
		AsyncTask(ENamedThreads::GameThread, [Token = FString(UTF8_TO_TCHAR(token))]() -> void
		{
			FFirebaseMessagingLibrary::CachedFirebaseCloudMessagingToken = Token;
			FFirebaseMessagingLibrary::OnTokenReceived().Broadcast(Token);
		});
	}
#endif // WITH_FIREBASE_MESSAGING
};


FFirebaseFeaturesModule::FFirebaseFeaturesModule()
{

}

FFirebaseFeaturesModule::~FFirebaseFeaturesModule()
{

}

#if WITH_EDITOR
static bool CopyGoogleServicesPlistToLocation(const FString& TargetLocation)
{
	IPlatformFile::GetPlatformPhysical().CreateDirectoryTree(*FPaths::GetPath(TargetLocation));
	return IPlatformFile::GetPlatformPhysical().CopyFile(*TargetLocation, *(FPaths::ProjectDir() / TEXT("Services/GoogleService-Info.plist")));
}
#endif

#if PLATFORM_IOS
extern void FirebaseFeatures_OnOpenURL(UIApplication* application, NSURL* url, NSString* sourceApplication, id annotation);
#endif

void FFirebaseFeaturesModule::StartupModule()
{
#if WITH_EDITOR
	// Creates the Services folder if it doesn't exist.
	{
		const FString FirebaseServicesDir = FPaths::ProjectDir() / TEXT("Services/");
		if (!FPaths::DirectoryExists(FirebaseServicesDir))
		{
			IFileManager::Get().MakeDirectory(*FirebaseServicesDir);
		}
	}


	// Register settings
	ISettingsModule* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>(TEXT("Settings"));

	if (SettingsModule)
	{
		ISettingsSectionPtr SettingsSection = SettingsModule->RegisterSettings("Project", "Plugins", TEXT("Firebase Features"),
			LOCTEXT("FirebaseFeaturesName", "Firebase Features"),
			LOCTEXT("FirebaseFeaturesSettingsDescription", "Configure the Firebase Features plugin."),
			GetMutableDefault<UFirebaseConfig>()
		);

		if (SettingsSection.IsValid())
		{
			SettingsSection->OnModified().BindRaw(this, &FFirebaseFeaturesModule::HandleSettingsSaved);
		}
	}

	{
		UE_LOG(LogFirebaseSdk, Log, TEXT("Searching for Google Services files..."));

		const FString iOSServicesLocation     = FPaths::ProjectDir() / TEXT("Services/GoogleService-Info.plist");
		const FString AndroidServicesLocation = FPaths::ProjectDir() / TEXT("Services/google-services.json");

		if (FPaths::FileExists(iOSServicesLocation))
		{
			UE_LOG(LogFirebaseSdk, Log, TEXT("iOS Google Services file found."));

			// We copy the iOS services file to a location watched by the RPC used during remote builds.
			// i.e. <Project>/Build/IOS/GoogleService-Info.plist
			if (!CopyGoogleServicesPlistToLocation(FPaths::ProjectDir() / TEXT("Build/IOS/") / TEXT("GoogleService-Info.plist")))
			{
				UE_LOG(LogFirebaseSdk, Warning, TEXT("Failed to copy the GoogleService-Info.plist to saved directory. Remote build might not work."));
			}
		}
		else
		{
			UE_LOG(LogFirebaseSdk, Warning, TEXT("iOS Google Services file not found. Expected exact location: \"%s\"."), *FPaths::ConvertRelativePathToFull(iOSServicesLocation));
		}

		if (!FPaths::FileExists(AndroidServicesLocation))
		{
			UE_LOG(LogFirebaseSdk, Warning, TEXT("Android Google Services file not found. Expected exact location: \"%s\"."), *FPaths::ConvertRelativePathToFull(AndroidServicesLocation));

#if !WITH_EDITOR && !UE_BUILD_SHIPPING
			const FText AlertTitle = NSLOCTEXT("FirebaseFeatures", "ServicesJsonNotFoundTitle", "Failed to find google-services.json");
			FMessageDialog::Open(EAppMsgType::Ok, NSLOCTEXT("FirebaseFeatures", "ServicesJsonNotFound",
				"Failed to find google-services.json. Your application will terminate. This message won't show in shipping builds."),
				&AlertTitle);
#endif
		}
		else
		{
			UE_LOG(LogFirebaseSdk, Log, TEXT("Android Google Services file found."));
		}
	}
#endif

#if WITH_EDITOR
	if (IsCooking())
	{
		UE_LOG(LogFirebaseSdk, Log, TEXT("Cooking detected. Skipping Firebase initialization."));
	}
	else
#endif
	{
		// We call it directly instead of GetApp() to avoid a call to firebase::app::GetInstance()
		// which logs a warning on some platforms when the app is not created yet.
		CreateApp();

		firebase::App* const FirebaseApp = GetApp();

		ensureMsgf(FirebaseApp != nullptr, TEXT("Failed to create Firebase Application. Make sure the google-services.json file exists and is valid."));

		if (FirebaseApp)
		{
			InitializeFirebaseComponents();
		}
		else
		{
			UE_LOG(LogFirebaseSdk, Error, TEXT("Skipped Firebase initializations as the creation of an app failed."));
		}
	}
	
#if WITH_EDITOR && WITH_FIREBASE_ADMOB
	{
		const FString DefaultEngineIni = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");

		const bool bSettingsAdMobStatus = UFirebaseConfig::Get()->bEnableAdMob;
		bool bAdMobEnabled = false;

		if (!GConfig->GetBool(TEXT("Firebase"), TEXT("AdMobEnabled"), bAdMobEnabled, DefaultEngineIni) || bAdMobEnabled != bSettingsAdMobStatus)
		{
			GConfig->SetBool(TEXT("Firebase"), TEXT("AdMobEnabled"), bSettingsAdMobStatus, DefaultEngineIni);

			UE_LOG(LogFirebaseSdk, Warning, TEXT("Incoherence in configuration settings for AdMob fixed."));
		}
	}
#endif

#if WITH_EDITOR
	if (GetDefault<UFirebaseConfig>()->bEnableCrashlytics)
	{
		const FString DefaultEngineIniPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
		GConfig->SetBool(TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"), TEXT("bGeneratedSYMFile"), true, DefaultEngineIniPath);
	}
#endif

#if WITH_EDITOR
	if (UFirebaseConfig::Get()->bEnableGoogleSignIn)
	{
		if (GetDefault<UFirebaseConfig>()->GoogleReversedClientId.IsEmpty())
		{
			UE_LOG(LogFirebaseSdk, Warning, TEXT("Google Reversed Client ID is empty but Google Sign-In is enabled. Trying to sign in with Google on iOS will crash your application.")
				TEXT(" Please, fill in your reversed client ID in Firebase Features' settings to get started using Google Sign-In on iOS."));
		}
		else
		{
			const FString DefaultEngineIniPath = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");
			GConfig->SetBool(TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"), TEXT("bEnableGoogleSupport"), true, DefaultEngineIniPath);
			GConfig->SetString(TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"), TEXT("GoogleReversedClientId"), *GetDefault<UFirebaseConfig>()->GoogleReversedClientId, DefaultEngineIniPath);
			UE_LOG(LogFirebaseSdk, Log, TEXT("Google Sign-In on iOS is enabled. Reversed client ID used: %s."), *GetDefault<UFirebaseConfig>()->GoogleReversedClientId);
		}
	}
#endif

#if PLATFORM_IOS
	if (UFirebaseConfig::Get()->bEnableGoogleSignIn)
	{
		UE_LOG(LogFirebaseSdk, Log, TEXT("Google Sign-In is enabled."));
		FIOSCoreDelegates::OnOpenURL.AddStatic(FirebaseFeatures_OnOpenURL);
	}
	else
	{
		UE_LOG(LogFirebaseSdk, Log, TEXT("Google Sign-In is disabled."));
	}
#endif
}

void FFirebaseFeaturesModule::ShutdownModule()
{
#if WITH_EDITOR
	// Unregister settings
	ISettingsModule* const SettingsModule = FModuleManager::GetModulePtr<ISettingsModule>("Settings");

	if (SettingsModule)
	{
		SettingsModule->UnregisterSettings(TEXT("Project"), TEXT("Plugins"), TEXT("Firebase Features"));
	}
#endif // WITH_EDITOR

#if WITH_EDITOR
	if (IsCooking())
	{
		UE_LOG(LogFirebaseSdk, Log, TEXT("Cooking detected. Skipping Firebase cleanup."))
	}
	else
#endif
	if (firebase::App* const App = firebase::App::GetInstance())
	{
#if WITH_FIREBASE_AUTH
		if (firebase::auth::Auth::GetAuth(App))
		{
			delete firebase::auth::Auth::GetAuth(App);
		}
#endif 

#if WITH_FIREBASE_FIRESTORE
		if (firebase::firestore::Firestore::GetInstance(App))
		{
			delete firebase::firestore::Firestore::GetInstance(App);
		}
#endif 

#if WITH_FIREBASE_DATABASE
		auto* const DatabaseInstance = firebase::database::Database::GetInstance(App);
		if (DatabaseInstance)
		{
			delete DatabaseInstance;
		}		
#endif	

#if WITH_FIREBASE_STORAGE
		if (firebase::storage::Storage::GetInstance(App))
		{
			delete firebase::storage::Storage::GetInstance(App);
		}
#endif	

#if WITH_FIREBASE_FUNCTIONS
		if (firebase::functions::Functions::GetInstance(App))
		{
			delete firebase::functions::Functions::GetInstance(App);
		}
#endif	

#if WITH_FIREBASE_ANALYTICS
		firebase::analytics::Terminate();
#endif

#if WITH_FIREBASE_ADMOB
		firebase::admob::rewarded_video::Destroy();
		firebase::admob::Terminate();
#endif // WITH_FIREBASE_ADMOB

#if WITH_FIREBASE_DYNAMIC_LINKS
		firebase::dynamic_links::Terminate();
#endif // WITH_FIREBASE_DYNAMIC_LINKS

#if WITH_FIREBASE_MESSAGING
		firebase::messaging::Terminate();
#endif // WITH_FIREBASE_MESSAGING

#if WITH_FIREBASE_REMOTE_CONFIG
#	if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
#		if PLATFORM_LINUX
#			pragma clang diagnostic push
#			pragma clang diagnostic ignored "-Wdeprecated-declarations"
#		endif
		firebase::remote_config::Terminate();
#		if PLATFORM_LINUX
#			pragma clang diagnostic pop
#		endif
#	else
		delete firebase::remote_config::RemoteConfig::GetInstance(App);
#	endif
#endif // WITH_FIREBASE_REMOTE_CONFIG
		
		delete App;

		UE_LOG(LogFirebaseSdk, Log, TEXT("Firebase App destroyed."));
	}
}


bool FFirebaseFeaturesModule::IsFirebaseSDKInitialized()
{
	return bIsSDKInitialized;
}

bool FFirebaseFeaturesModule::HandleSettingsSaved()
{
#if WITH_EDITOR
	const FString DefaultConfigIni = FPaths::ProjectConfigDir() / TEXT("DefaultEngine.ini");

#if ENGINE_MAJOR_VERSION >= 5
	if (!GetMutableDefault<UFirebaseConfig>()->TryUpdateDefaultConfigFile(DefaultConfigIni))
	{
		UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to update configuration."));
	}
#else
	GetMutableDefault<UFirebaseConfig>()->UpdateDefaultConfigFile(DefaultConfigIni);
#endif

	if (GetDefault<UFirebaseConfig>()->bEnableCrashlytics)
	{
		GConfig->SetBool(TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"), TEXT("bGeneratedSYMFile"), true, DefaultConfigIni);
	}

	if (!GetDefault<UFirebaseConfig>()->GoogleReversedClientId.IsEmpty())
	{
		GConfig->SetBool(TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"), TEXT("bEnableGoogleSupport"), true, DefaultConfigIni);
		GConfig->SetString(TEXT("/Script/IOSRuntimeSettings.IOSRuntimeSettings"), TEXT("GoogleReversedClientId"), *GetDefault<UFirebaseConfig>()->GoogleReversedClientId, DefaultConfigIni);
	}
#endif
	return true;
}

#if FIREBASE_FEATURES_UE_4_25_OR_NEWER
TSharedPtr<IAnalyticsProvider> FFirebaseFeaturesModule::CreateAnalyticsProvider(const FAnalyticsProviderConfigurationDelegate& GetConfigValue) const
{
	return AnalyticsProvider;
}
#endif // FIREBASE_FEATURES_UE_4_25_OR_NEWER

void FFirebaseFeaturesModule::InitializeFirebaseComponents()
{
	UE_LOG(LogFirebaseSdk, Log, TEXT("Initializing Firebase components."));

	// We use it in each init, so we check it here instead of checking it
	// every time we access it.
	check(firebase::App::GetInstance() != nullptr);

#if !UE_BUILD_SHIPPING
	firebase::SetLogLevel(firebase::LogLevel::kLogLevelVerbose);
#endif

	const UFirebaseConfig* const Config = UFirebaseConfig::Get();

	InitAuth();
	
	InitFirebaseModule(Analytics);
	InitFirebaseModule(AdMob);
	InitFirebaseModule(Database);
	InitFirebaseModule(Firestore);
	InitFirebaseModule(Messaging);
	InitFirebaseModule(Storage);
	InitFirebaseModule(Functions);
	InitFirebaseModule(RemoteConfig);

	InitDynamicLinks();

	bIsSDKInitialized = true;

	OnFirebaseSDKInitialized.Broadcast();
}

void FFirebaseFeaturesModule::InitStorage()
{
#if WITH_FIREBASE_STORAGE
	using namespace firebase;

	UE_LOG(LogFirebaseStorage, Log, TEXT("Initializing Firebase Storage..."));

	InitResult Result = InitResult::kInitResultFailedMissingDependency;

	storage::Storage::GetInstance(GetApp(), &Result);

	switch (Result)
	{
	case InitResult::kInitResultSuccess:
		UE_LOG(LogFirebaseStorage, Log, TEXT("Firebase Storage initialized."));
		break;
	case InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogFirebaseStorage, Error, TEXT("Firebase Storage initialization failed: InitResultFailedMisingDependency."));
		break;
	default:
		UE_LOG(LogFirebaseStorage, Warning, TEXT("Firebase Storage initialization unknown result: %d."), (int32)Result);
	};

#endif // WITH_FIREBASE_STORAGE
}

void FFirebaseFeaturesModule::InitRemoteConfig()
{
#if WITH_FIREBASE_REMOTE_CONFIG
	using namespace firebase;
	
	UE_LOG(LogFirebaseRemoteConfig, Log, TEXT("Initializing Firebase Remote Config..."));

#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
#	if PLATFORM_LINUX
#		pragma clang diagnostic push
#		pragma clang diagnostic ignored "-Wdeprecated-declarations"
#	endif
	const InitResult Result = firebase::remote_config::Initialize(*GetApp());
#	if PLATFORM_LINUX
#		pragma clang diagnostic pop
#	endif

	switch (Result)
	{
	case InitResult::kInitResultSuccess:
		UE_LOG(LogFirebaseRemoteConfig, Log, TEXT("Firebase Remote Config initialized."));
		break;
	case InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogFirebaseRemoteConfig, Error, TEXT("Firebase Remote Config initialization failed: InitResultFailedMisingDependency."));
		break;
	default:
		UE_LOG(LogFirebaseRemoteConfig, Warning, TEXT("Firebase Remote Config initialization unknown result: %d."), (int32)Result);
	};
#else
	firebase::remote_config::RemoteConfig::GetInstance(GetApp())->EnsureInitialized()
		.OnCompletion([](const Future<firebase::remote_config::ConfigInfo>& Future) -> void
	{
		if (Future.error())
		{
			UE_LOG(LogFirebaseRemoteConfig, Error, TEXT("Remote configuration initialization error: %s"), UTF8_TO_TCHAR(Future.error_message()));
		}
		else
		{
			UE_LOG(LogFirebaseRemoteConfig, Log, TEXT("Firebase Remote Config initialized."));
		}
	});
#endif
#endif // WITH_FIREBASE_REMOTE_CONFIG
}

void FFirebaseFeaturesModule::InitMessaging()
{
#if WITH_FIREBASE_MESSAGING
	using namespace firebase;

	UE_LOG(LogFirebaseMessaging, Log, TEXT("Initializing Firebase Messaging..."));

	messaging::MessagingOptions Options;

	Options.suppress_notification_permission_prompt = UFirebaseConfig::Get()->bSuppressNotificationPermissionPrompt;

	MessagingListener.Reset(new FMessagingListener());

	messaging::SetTokenRegistrationOnInitEnabled(UFirebaseConfig::Get()->bTokenRegistrationOnInit);

	const InitResult Result = messaging::Initialize(*GetApp(), MessagingListener.Get(), Options);
	
	switch (Result)
	{
	case InitResult::kInitResultSuccess:
		UE_LOG(LogFirebaseMessaging, Log, TEXT("Firebase Messaging initialized."));
		break;
	case InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogFirebaseMessaging, Error, TEXT("Firebase Messaging initialization failed: InitResultFailedMisingDependency."));
		break;
	default:
		UE_LOG(LogFirebaseMessaging, Warning, TEXT("Firebase Messaging initialization unknown result: %d."), (int32)Result);
	}
#endif // WITH_FIREBASE_MESSAGING
}

void FFirebaseFeaturesModule::InitFunctions()
{
#if WITH_FIREBASE_FUNCTIONS
	UE_LOG(LogFirebaseFunctions, Log, TEXT("Initializing Firebase Functions..."));

	firebase::InitResult Result = firebase::InitResult::kInitResultFailedMissingDependency;
	
	firebase::functions::Functions* const Instance = firebase::functions::Functions::GetInstance(GetApp(), &Result);

	switch (Result)
	{
	case firebase::InitResult::kInitResultSuccess:
		UE_LOG(LogFirebaseFunctions, Log, TEXT("Firebase Functions initialized."));
		break;
	case firebase::InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogFirebaseFunctions, Error, TEXT("Firebase Functions initialization failed: InitResultFailedMisingDependency."));
		break;
	default:
		UE_LOG(LogFirebaseFunctions, Warning, TEXT("Firebase Functions initialization unknown result: %d."), (int32)Result);
	};	
#endif
}

void FFirebaseFeaturesModule::InitFirestore()
{
#if WITH_FIREBASE_FIRESTORE
	UE_LOG(LogFirestore, Log, TEXT("Initializing Firestore."));

	firebase::InitResult Result = firebase::InitResult::kInitResultFailedMissingDependency;

	firebase::firestore::Firestore* const Firestore = firebase::firestore::Firestore::GetInstance(GetApp(), &Result);

	switch (Result)
	{
	case firebase::InitResult::kInitResultSuccess:
	{
		UE_LOG(LogFirestore, Log, TEXT("Firestore initialized."));

#if !UE_BUILD_SHIPPING
		Firestore->set_log_level(firebase::LogLevel::kLogLevelVerbose);
#endif

		auto Settings = Firestore->settings();

		Settings.set_persistence_enabled(UFirebaseConfig::Get()->bPersistenceEnabled);
		Settings.set_ssl_enabled(UFirebaseConfig::Get()->bSslEnabled);

		if (!UFirebaseConfig::Get()->Host.IsEmpty())
		{
			Settings.set_host(TCHAR_TO_UTF8(*UFirebaseConfig::Get()->Host));
		}

		Firestore->set_settings(Settings);

	} break;
	case firebase::InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogFirestore, Error, TEXT("Firestore initialization failed: InitResultFailedMisingDependency."));
		break;

	default:
		UE_LOG(LogFirestore, Warning, TEXT("Firestore initialization unknown result: %d."), (int32)Result);
	};	
#else
	UE_LOG(LogFirestore, Log, TEXT("Firestore is disabled, skipping initialization."));
#endif // WITH_FIREBASE_FIRESTORE
}

void FFirebaseFeaturesModule::InitAnalytics()
{
#if WITH_FIREBASE_ANALYTICS
	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Initializing Firebase Analytics."));

	firebase::analytics::Initialize(*GetApp());

	AnalyticsProvider = MakeShared<FFirebaseAnalyticsProvider>();

	UE_LOG(LogFirebaseAnalytics, Log, TEXT("Firebase Analytics initialized."));
#endif
}

void FFirebaseFeaturesModule::InitAuth()
{
#if WITH_FIREBASE_AUTH
	UE_LOG(LogFirebaseAuth, Log, TEXT("Initializing Firebase Auth..."));

	firebase::InitResult Result = firebase::InitResult::kInitResultFailedMissingDependency;

	firebase::auth::Auth* const Auth = firebase::auth::Auth::GetAuth(GetApp());

	ensureMsgf(Auth != nullptr, TEXT("Failed to create default Auth object."));

	if (Auth != nullptr)
	{
		AuthListener	.Reset(new FAuthStateListener());
		IdTokenListener	.Reset(new FIdTokenListener  ());

		Auth->AddAuthStateListener(AuthListener   .Get());
		Auth->AddIdTokenListener  (IdTokenListener.Get());

		UE_LOG(LogFirebaseAuth, Log, TEXT("Firebase Auth initialized."));
	}
#endif
}

void FFirebaseFeaturesModule::InitAdMob()
{
#if WITH_FIREBASE_ADMOB
	UE_LOG(LogAdMob, Log, TEXT("Initializing AdMob..."));

	const firebase::InitResult Result = firebase::admob::Initialize(*GetApp());

	switch (Result)
	{
	case firebase::InitResult::kInitResultSuccess:
		UE_LOG(LogAdMob, Log, TEXT("AdMob initialized."));
		break;
	case firebase::InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogAdMob, Error, TEXT("AdMob initialization failed: InitResultFailedMisingDependency."));
		break;
	default:
		UE_LOG(LogAdMob, Warning, TEXT("AdMob initialization unknown result: %d."), (int32)Result);
	};

	if (Result == firebase::InitResult::kInitResultSuccess)
	{
		UE_LOG(LogAdMob, Log, TEXT("Initializing AdMob rewarded video..."));
		firebase::admob::rewarded_video::Initialize().OnCompletion
		(
			[](const firebase::Future<void>& Future) -> void
			{
				if (Future.error() != firebase::admob::AdMobError::kAdMobErrorNone)
				{
					UE_LOG(LogAdMob, Error, TEXT("Failed to initialize AdMob rewarded video. Error code: %d."), Future.error());
				}
				else
				{
					UE_LOG(LogAdMob, Log, TEXT("AdMob rewarded video initialized."));
					FRewardedVideo::SetListener();
				}
			}
		);
	}
#endif // WITH_FIREBASE_ADMOB
}

void FFirebaseFeaturesModule::InitDynamicLinks()
{
#if WITH_FIREBASE_DYNAMIC_LINKS
	UE_LOG(LogFirebaseDynamicLinks, Log, TEXT("Initializing Firebase Dynamic Links..."));

	DynamicLinkListener.Reset(new FDynamicLinksListener());

	firebase::InitResult Result = firebase::dynamic_links::Initialize(
		*FFirebaseFeaturesModule::GetCurrentFirebaseApp(), DynamicLinkListener.Get());

	switch (Result)
	{
	case firebase::InitResult::kInitResultSuccess:
		UE_LOG(LogFirebaseDynamicLinks, Log, TEXT("Firebase Dynamic Links initialized."));
		break;
	default:
		UE_LOG(LogFirebaseDynamicLinks, Warning, TEXT("Failed to initialize Firebase Dynamic Links. Code: %d."), (int32)Result);
	}
#endif // WITH_FIREBASE_DYNAMIC_LINKS
}

void FFirebaseFeaturesModule::InitDatabase()
{
#if WITH_FIREBASE_DATABASE
	UE_LOG(LogFirebaseDatabase, Log, TEXT("Initializing Firebase Database..."));

	firebase::InitResult Result = firebase::InitResult::kInitResultFailedMissingDependency;

	firebase::database::Database* const Database =
		firebase::database::Database::GetInstance(GetApp(), &Result);

	switch (Result)
	{
	case firebase::InitResult::kInitResultSuccess:
#if !UE_BUILD_SHIPPING
		Database->set_log_level(firebase::LogLevel::kLogLevelVerbose);
#endif
		UE_LOG(LogFirebaseDatabase, Log, TEXT("Firebase Database initialized."));
		break;
	case firebase::InitResult::kInitResultFailedMissingDependency:
		UE_LOG(LogFirebaseDatabase, Error, TEXT("Failed to initialize Firebase Datase. Code: kInitResultFailedMissingDependency."));
		break;
	default:
		UE_LOG(LogFirebaseDatabase, Error, TEXT("Failed to initialize Firebase Datase. Unknown code %d."), (int32)Result);
		break;
	}
#endif
}

firebase::App* FFirebaseFeaturesModule::GetApp()
{
	if (!firebase::App::GetInstance())
	{
		CreateApp();
	}

	return firebase::App::GetInstance();
}

firebase::App* FFirebaseFeaturesModule::GetFirebaseApp(const FString& AppName)
{
	if (AppName.IsEmpty())
	{
		return GetApp();
	}

	FTCHARToUTF8 AppNameUtf8(*AppName);

	auto* const App = firebase::App::GetInstance(AppNameUtf8.Get());

	if (!App)
	{
		UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to get app %s."), UTF8_TO_TCHAR(AppNameUtf8.Get()));
	}

	return App;
}

firebase::App* FFirebaseFeaturesModule::CreateFirebaseApp(const FString& AppName, const FFirebaseAppOptions& Options)
{
	if (AppName.IsEmpty())
	{
		UE_LOG(LogFirebaseSdk, Warning, TEXT("Tried to create the default app manually. The unnamed app is automatically created."));
		return nullptr;
	}
	
	FTCHARToUTF8 AppNameUtf8(*AppName);

	firebase::AppOptions NativeOptions;

	NativeOptions.set_project_id			(TCHAR_TO_UTF8(*Options.ProjectID));
	NativeOptions.set_api_key				(TCHAR_TO_UTF8(*Options.APIKey));
	NativeOptions.set_app_id				(TCHAR_TO_UTF8(*Options.AppID));
	NativeOptions.set_database_url			(TCHAR_TO_UTF8(*Options.DatabaseURL));
	NativeOptions.set_ga_tracking_id		(TCHAR_TO_UTF8(*Options.GATrackingID));
	NativeOptions.set_messaging_sender_id	(TCHAR_TO_UTF8(*Options.MessagingSenderID));
	NativeOptions.set_storage_bucket		(TCHAR_TO_UTF8(*Options.StorageBucket));

#if PLATFORM_ANDROID
	auto* const App = firebase::App::Create(NativeOptions, AppNameUtf8.Get(), 
		FAndroidApplication::GetJavaEnv(), FJavaWrapper::GameActivityThis);
#else
	auto* const App = firebase::App::Create(NativeOptions, AppNameUtf8.Get());
#endif

	if (!App)
	{
		UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to create app %s."), UTF8_TO_TCHAR(AppNameUtf8.Get()));
	}

	return App;
}

firebase::App* FFirebaseFeaturesModule::SwitchFirebaseApp(FString NewAppName)
{
	auto* const App = GetFirebaseApp(NewAppName);
	if (!App)
	{
		return nullptr;
	}

	CurrentFirebaseAppName = MoveTemp(NewAppName);

	return App;
}

firebase::App* FFirebaseFeaturesModule::GetCurrentFirebaseApp()
{
	return GetFirebaseApp(CurrentFirebaseAppName);
}

void FFirebaseFeaturesModule::CreateApp()
{
#if PLATFORM_ANDROID
	firebase::App::Create(FAndroidApplication::GetJavaEnv(), FJavaWrapper::GameActivityThis);
#elif PLATFORM_IOS
	firebase::App::Create();
#else // Desktop
	FString JsonContent;

	const FString ServicesLocation = GetServicesLocation();

	if (!FPaths::FileExists(ServicesLocation))
	{
		UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to find google-services.json with path \"%s\". Firebase app creation failed."),
			*FPaths::ConvertRelativePathToFull(ServicesLocation));
		return;
	}
	if (!FFileHelper::LoadFileToString(JsonContent, *ServicesLocation))
	{
		UE_LOG(LogFirebaseSdk, Error, TEXT("Failed to read google-services.json content with path \"%s\"."),
			*FPaths::ConvertRelativePathToFull(ServicesLocation));
		return;
	}

	firebase::AppOptions Options;
	firebase::AppOptions::LoadFromJsonConfig(TCHAR_TO_UTF8(*JsonContent), &Options);
	firebase::App::Create(Options);

	UE_LOG(LogFirebaseSdk, Log, TEXT("New Firebase app created."));
#endif // !PLATFORM_ANDROID && !PLTAFORM_IOS

	if (firebase::App::GetInstance())
	{
		UE_LOG(LogFirebaseSdk, Log, TEXT("Firebase app created is valid."));
	}
	else
	{
		ensureMsgf(false, TEXT("Firebase app created is invalid. Check your google-services.json."));
	}
}

#if WITH_EDITOR
bool FFirebaseFeaturesModule::IsCooking() const
{
	return /* GIsCookerLoadingPackage && */ IsRunningCommandlet();
}
#endif

FString FFirebaseFeaturesModule::GetServicesLocation()
{
	return FPaths::ProjectDir() / TEXT("Services/google-services.json");
}

#undef LOCTEXT_NAMESPACE
#undef InitFirebaseModule

IMPLEMENT_MODULE(FFirebaseFeaturesModule, FirebaseFeatures)

