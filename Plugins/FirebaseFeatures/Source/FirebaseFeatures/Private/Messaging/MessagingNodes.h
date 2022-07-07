// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Messaging/MessagingLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MessagingNodes.generated.h"

UCLASS()
class UFirebaseMesssagingBlueprintLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
	/// @brief Terminate Firebase Cloud Messaging.
	///
	/// Frees resources associated with Firebase Cloud Messaging.
	///
	/// @note On Android, the services will not be shut down by this method.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging")
	static void Terminate();

	/// Determines if automatic token registration during initalization is enabled.
	///
	/// @return true if auto token registration is enabled and false if disabled.
	UFUNCTION(BlueprintCallable, BlueprintCallable, Category = "Firebase|Messaging")
	static UPARAM(DisplayName = "Is Enabled") bool IsTokenRegistrationOnInitEnabled();

	/// Enable or disable token registration during initialization of Firebase Cloud
	/// Messaging.
	///
	/// This token is what identifies the user to Firebase, so disabling this avoids
	/// creating any new identity and automatically sending it to Firebase, unless
	/// consent has been granted.
	///
	/// If this setting is enabled, it triggers the token registration refresh
	/// immediately. This setting is persisted across app restarts and overrides the
	/// setting "firebase_messaging_auto_init_enabled" specified in your Android
	/// manifest (on Android) or Info.plist (on iOS).
	///
	/// @param enable sets if a registration token should be requested on
	/// initialization.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging")
	static void SetTokenRegistrationOnInitEnabled(const bool bEnable);

	/// Send an upstream ("device to cloud") message. You can only use the upstream
	/// feature if your FCM implementation uses the XMPP-based Cloud Connection
	/// Server. The current limits for max storage time and number of outstanding
	/// messages per application are documented in the [FCM Developers Guide].
	///
	/// [FCM Developers Guide]: https://firebase.google.com/docs/cloud-messaging/
	///
	/// @param[in] message The message to send upstream.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging", meta = (DeprecatedFunction, DeprecationMessage = "Send() will be removed in a future version."))
	static void Send(const FFirebaseMessage& Message);

	/// Determines whether Firebase Cloud Messaging exports message delivery metrics
	/// to BigQuery.
	///
	/// This function is currently only implemented on Android, and returns false
	/// with no other behavior on other platforms.
	///
	/// @return true if Firebase Cloud Messaging exports message delivery metrics to
	/// BigQuery.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Messaging")
	static UPARAM(DisplayName = "Is Enabled") bool IsDeliveryMetricsExportToBigQueryEnabled();

	/// Enables or disables Firebase Cloud Messaging message delivery metrics export
	/// to BigQuery.
	///
	/// By default, message delivery metrics are not exported to BigQuery. Use this
	/// method to enable or disable the export at runtime. In addition, you can
	/// enable the export by adding to your manifest. Note that the run-time method
	/// call will override the manifest value.
	///
	/// <meta-data android:name= "delivery_metrics_exported_to_big_query_enabled"
	///            android:value="true"/>
	///
	/// This function is currently only implemented on Android, and has no behavior
	/// on other platforms.
	///
	/// @param[in] enable Whether Firebase Cloud Messaging should export message
	///            delivery metrics to BigQuery.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging")
	static void SetDeliveryMetricsExportToBigQuery(const bool bEnable);


	/**
	 * Gets the cached Firebase Cloud Messaging token if any.
	 * @return The cached Firebase Cloud Messaging token or an empty string if none.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Messaging")
	static UPARAM(DisplayName = "Token") FString GetCachedCloudMessagingToken();

	/**
	 * Gets the last message received through FCM.
	 * @return The last message received through FCM.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Messaging")
	static UPARAM(DisplayName = "Message") FFirebaseMessage GetLastMessage();
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynMultMessaging, const EFirebaseMessagingError, Error);

UCLASS()
class URequestPermissionProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * completes when the notification prompt has been
	 * dismissed.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultMessaging OnRequestOver;

	UPROPERTY(BlueprintAssignable)
	FDynMultMessaging OnError;

public:
	/// @brief Displays a prompt to the user requesting permission to display
	///        notifications.
	///
	/// The permission prompt only appears on iOS. If the user has already agreed to
	/// allow notifications, no prompt is displayed and the returned future is
	/// completed immediately.
	///
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging", meta = (BlueprintInternalUseOnly = "true"))
	static URequestPermissionProxy* RequestPermission();

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseMessagingError Error);
};

UCLASS()
class USubscribeProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultMessaging OnSubscribed;
	UPROPERTY(BlueprintAssignable)
	FDynMultMessaging OnError;

public:
	/// @brief Subscribe to receive all messages to the specified topic.
	///
	/// Subscribes an app instance to a topic, enabling it to receive messages
	/// sent to that topic.
	///
	/// Call this function from the main thread. FCM is not thread safe.
	///
	/// @param[in] Topic The name of the topic to subscribe. Must match the
	///            following regular expression: `[a-zA-Z0-9-_.~%]{1,900}`.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging", meta = (BlueprintInternalUseOnly = "true"))
	static USubscribeProxy* Subscribe(const FString& Topic);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseMessagingError Error);

	FString _Topic;
};

UCLASS()
class UUnsubscribeProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultMessaging OnUnsubscribed;
	UPROPERTY(BlueprintAssignable)
	FDynMultMessaging OnError;

public:
	/// @brief Unsubscribe from a topic.
	///
	/// Unsubscribes an app instance from a topic, stopping it from receiving
	/// any further messages sent to that topic.
	///
	/// Call this function from the main thread. FCM is not thread safe.
	///
	/// @param[in] topic The name of the topic to unsubscribe from. Must match the
	///            following regular expression: `[a-zA-Z0-9-_.~%]{1,900}`.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging", meta = (BlueprintInternalUseOnly = "true"))
	static UUnsubscribeProxy* Unsubscribe(const FString& Topic);

	virtual void Activate();

private:
	void OnActionOver(const EFirebaseMessagingError Error);

	FString _Topic;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultMessagingEvent, const FFirebaseMessage&, Message, const FString&, Token);

UCLASS()
class UMessagingEventsProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Called when a message has been received.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultMessagingEvent OnMessage;

	/**
	 * Called when the FCM token has been sent to this client.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultMessagingEvent OnTokenReceived;

public:
	UFUNCTION(BlueprintCallable, Category = "Firebase|Messaging", meta = (BlueprintInternalUseOnly = "true"))
	static UMessagingEventsProxy* ListenForMessagingEvents();

	virtual void Activate();

private:
	void OnMessageInternal(const FFirebaseMessage& Message);
	void OnTokenInternal(const FString& Token);

private:
	FString CachedToken;
};


