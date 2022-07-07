// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseConfig.generated.h"

UCLASS(Config = Engine, defaultconfig)
class FIREBASEFEATURES_API UFirebaseConfig : public UObject
{
	GENERATED_BODY()
public:
	UFirebaseConfig();

	static const UFirebaseConfig* Get();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Firebase Authentication", Tooltip = "If you want to enable Authentication."))
	bool bEnableAuth = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Firestore", Tooltip = "If you want to enable Firestore."))
	bool bEnableFirestore = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Realtime Database", Tooltip = "If you want to enable Realtime Database."))
	bool bEnableDatabase = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable AdMob", Tooltip = "If you want to enable AdMob (available only on iOS and Android)."))
	bool bEnableAdMob = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Analytics", Tooltip = "If you want to enable Firebase Analytics."))
	bool bEnableAnalytics = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Firebase Functions", Tooltip = "If you want to enable Firebase Functions."))
	bool bEnableFunctions = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Firebase Messaging", Tooltip = "If you want to enable Messaging."))
	bool bEnableMessaging = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Remote Config", Tooltip = "If you want to enable Messaging."))
	bool bEnableRemoteConfig = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Firebase Storage", Tooltip = "If you want to enable Firebase Storage."))
	bool bEnableStorage = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Firebase Crashlytics", Tooltip = "If you want to enable Firebase Crashlytics."))
	bool bEnableCrashlytics = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Features", meta = (DisplayName = "Enable Firebase Performances", Tooltip = "If you want to enable Firebase Performances."))
	bool bEnablePerformances = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Analytics", meta = (DisplayName = "Analytics Session Timeout (in seconds)", Tooltip = "The duration of inactivity that terminates the current session."))
	int32 FirebaseAnalyticsSessionTimeoutDuration = 1800;

	/**
	 * Disable the prompt for notification. You should call RequestPermission if you suppress it.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Messaging", meta = (DisplayName = "Suppress Notification Permission Prompt"))
	bool bSuppressNotificationPermissionPrompt;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Messaging", meta = (DisplayName = "Token Registration On Init"))
	bool bTokenRegistrationOnInit = true;

	/**
	 * Enables Google Sign-In on iOS. Note that you need to set your Google's URL scheme or the application will crash
	 * on launch.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Google Sign-In", meta = (DisplayName = "Enable Google Sign-In"))
	bool bEnableGoogleSignIn = false;

	/**
	 * The Google Reversed Client ID for Google Sign-In. (com.googleusercontent.apps.1234567890-abcdefg).
	 * This value will be inserted in the info.plist's URL scheme.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Google Sign-In", meta = (DisplayName = "Google Sign-In Reversed Client ID"))
	FString GoogleReversedClientId;

	/**
	 * If set to true, Google Play Games's libraries will be included in the build.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Google Play Games", Meta = (DisplayName = "Enable Google Play Games Sign In (Android)"))
	bool bEnableGooglePlayGamesSignIn = false;

	// Gets the host of the Firestore backend to connect to.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Firestore", Meta = (DisplayName = "Host"))
	FString Host;
	
	// Enables or disables SSL for communication.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Firestore", Meta = (DisplayName = "SSL Enabled"))
	bool bSslEnabled = true;

	// Enables or disables local persistent storage.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Firestore", Meta = (DisplayName = "Persistence Enabled"))
	bool bPersistenceEnabled = true;

	/**
	 * If READ_PHONE_STATE permission should end up in the manifest file.
	 * Setting it to false will remove it. Setting it to true does nothing and the native Firebase SDK
	 * will add it if required.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Android Permissions", Meta = (DisplayName = "Request READ_PHONE_STATE Permission"))
	bool bRequestReadPhoneStatePermission = true;

	/**
	 * If ACCESS_COARSE_LOCATION permission should end up in the manifest file.
	 * Setting it to false will remove it. Setting it to true does nothing and the native Firebase SDK
	 * will add it if required.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "Android Permissions", Meta = (DisplayName = "Request ACCESS_COARSE_LOCATION Permission"))
	bool bRequestAccessCoarseLocation = true;

	/**
	 * The value of the key FirebaseAppStoreReceiptURLCheckEnabled in the plist file on iOS.
	 * Note: As the default value is true, the key will be added in the plist only if this value is set to false.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, config, Category = "iOS", Meta = (DisplayName = "Firebase App Store Receipt URL Check Enabled"))
	bool bFirebaseAppStoreReceiptURLCheckEnabled = true;
};
