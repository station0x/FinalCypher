// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MessagingLibrary.generated.h"

#ifndef WITH_FIREBASE_MESSAGING
#	define WITH_FIREBASE_MESSAGING 0
#endif
 
namespace firebase { namespace messaging { struct Message; } }

/// Error code returned by Firebase Cloud Messaging C++ functions.
UENUM(BlueprintType)
enum class EFirebaseMessagingError : uint8
{
	/// The operation was a success, no error occurred.
	None = 0,
	/// Permission to receive notifications was not granted.
	FailedToRegisterForRemoteNotifications,
	/// Topic name is invalid for subscription/unsubscription.
	InvalidTopicName,
	/// Could not subscribe/unsubscribe because there is no registration token.
	NoRegistrationToken,
	/// Unknown error.
	Unknown,
};


/// @brief Data structure for parameters that are unique to the Android
/// implementation.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseAndroidNotificationParams
{
	GENERATED_BODY()
public:
	/// The channel id that was provided when the message was sent.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString ChannelId;
};

/// Used for messages that display a notification.
///
/// On android, this requires that the app is using the Play Services client
/// library.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseNotification
{
	GENERATED_BODY()
public:
	FFirebaseNotification() {}

	/// Copy constructor. Makes a deep copy of this Message.
	FFirebaseNotification(const FFirebaseNotification& other) { *this = other; }

	/// Copy assignment operator. Makes a deep copy of this Message.
	FFirebaseNotification& operator=(const FFirebaseNotification& other) {
		this->Title				= other.Title		;
		this->Body				= other.Body		;
		this->Icon				= other.Icon		;
		this->Sound				= other.Sound		;
		this->Tag				= other.Tag			;
		this->Color				= other.Color		;
		this->ClickAction		= other.ClickAction	;
		this->BodyLocKey		= other.BodyLocKey	;
		this->BodyLocArgs		= other.BodyLocArgs	;
		this->TitleLocKey		= other.TitleLocKey	;
		this->TitleLocArgs		= other.TitleLocArgs;
		this->Android			= other.Android		;
		return *this;
	}

	/// Indicates notification title. This field is not visible on iOS phones
	/// and tablets.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Title;

	/// Indicates notification body text.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Body;

	/// Indicates notification icon. Sets value to myicon for drawable resource
	/// myicon.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Icon;

	/// Indicates a sound to play when the device receives the notification.
	/// Supports default, or the filename of a sound resource bundled in the
	/// app.
	///
	/// Android sound files must reside in /res/raw/, while iOS sound files
	/// can be in the main bundle of the client app or in the Library/Sounds
	/// folder of the app's data container.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Sound;

	/// Indicates the badge on the client app home icon. iOS only.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Badge;

	/// Indicates whether each notification results in a new entry in the
	/// notification drawer on Android. If not set, each request creates a new
	/// notification. If set, and a notification with the same tag is already
	/// being shown, the new notification replaces the existing one in the
	/// notification drawer.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Tag;

	/// Indicates color of the icon, expressed in \#rrggbb format. Android only.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Color;

	/// The action associated with a user click on the notification.
	///
	/// On Android, if this is set, an activity with a matching intent filter is
	/// launched when user clicks the notification.
	///
	/// If set on iOS, corresponds to category in APNS payload.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString ClickAction;

	/// Indicates the key to the body string for localization.
	///
	/// On iOS, this corresponds to "loc-key" in APNS payload.
	///
	/// On Android, use the key in the app's string resources when populating this
	/// value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString BodyLocKey;

	/// Indicates the string value to replace format specifiers in body string
	/// for localization.
	///
	/// On iOS, this corresponds to "loc-args" in APNS payload.
	///
	/// On Android, these are the format arguments for the string resource. For
	/// more information, see [Formatting strings][1].
	///
	/// [1]:
	/// https://developer.android.com/guide/topics/resources/string-resource.html#FormattingAndStyling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	TArray<FString> BodyLocArgs;

	/// Indicates the key to the title string for localization.
	///
	/// On iOS, this corresponds to "title-loc-key" in APNS payload.
	///
	/// On Android, use the key in the app's string resources when populating this
	/// value.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString TitleLocKey;

	/// Indicates the string value to replace format specifiers in title string
	/// for localization.
	///
	/// On iOS, this corresponds to "title-loc-args" in APNS payload.
	///
	/// On Android, these are the format arguments for the string resource. For
	/// more information, see [Formatting strings][1].
	///
	/// [1]:
	/// https://developer.android.com/guide/topics/resources/string-resource.html#FormattingAndStyling
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	TArray<FString> TitleLocArgs;

	/// Parameters that are unique to the Android implementation.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FFirebaseAndroidNotificationParams Android;
};

/// @brief Data structure used to send messages to, and receive messages from,
/// cloud messaging.
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseMessage 
{
	GENERATED_BODY()
public:
	/// Initialize the message.
	FFirebaseMessage()
		: TimeToLive(0)
		, bNotificationOpened(false)
		, SentTime(0)
		, bIsValid(false)
	{}

#if WITH_FIREBASE_MESSAGING
	FFirebaseMessage(const firebase::messaging::Message* const Message);
#endif

	/// Copy constructor. Makes a deep copy of this Message.
	FFirebaseMessage(const FFirebaseMessage& other);
	FFirebaseMessage(FFirebaseMessage&& other);

	/// Copy assignment operator. Makes a deep copy of this Message.
	FFirebaseMessage& operator=(const FFirebaseMessage& other);
	FFirebaseMessage& operator=(FFirebaseMessage&& other);

	/// Authenticated ID of the sender. This is a project number in most cases.
	///
	/// Any value starting with google.com, goog. or gcm. are reserved.
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString From;

	/// This parameter specifies the recipient of a message.
	///
	/// For example it can be a registration token, a topic name, an Instance ID
	/// or project ID.
	///
	/// For upstream messages use the format  PROJECT_ID@gcm.googleapis.com.
	///
	/// This field is used for both upstream messages sent with
	/// firebase::messaging::Send() and downstream messages received through
	/// Listener::OnMessage(). For upstream messages,
	/// PROJECT_ID@gcm.googleapis.com or Instance ID are accepted.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString To;

	/// This parameter identifies a group of messages (e.g., with collapse_key:
	/// "Updates Available") that can be collapsed, so that only the last message
	/// gets sent when delivery can be resumed.  This is intended to avoid sending
	/// too many of the same messages when the device comes back online or becomes
	/// active.
	///
	/// Note that there is no guarantee of the order in which messages get sent.
	///
	/// Note: A maximum of 4 different collapse keys is allowed at any given time.
	/// This means a FCM connection server can simultaneously store 4 different
	/// send-to-sync messages per client app. If you exceed this number, there is
	/// no guarantee which 4 collapse keys the FCM connection server will keep.
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString CollapseKey;

	/// The metadata, including all original key/value pairs. Includes some of the
	/// HTTP headers used when sending the message. `gcm`, `google` and `goog`
	/// prefixes are reserved for internal use.
	///
	/// This field is used for both upstream messages sent with
	/// firebase::messaging::Send() and downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	TMap<FString, FString> Data;

	/// Binary payload. This field is currently unused.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString RawData;

	/// Message ID. This can be specified by sender. Internally a hash of the
	/// message ID and other elements will be used for storage. The ID must be
	/// unique for each topic subscription - using the same ID may result in
	/// overriding the original message or duplicate delivery.
	///
	/// This field is used for both upstream messages sent with
	/// firebase::messaging::Send() and downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString MessageId;

	/// Equivalent with a content-type.
	///
	/// Defined values:
	///   - "deleted_messages" - indicates the server had too many messages and
	///     dropped some, and the client should sync with his own server.
	///     Current limit is 100 messages stored.
	///   - "send_event" - indicates an upstream message has been pushed to the
	///     FCM server. It does not guarantee the upstream destination received
	///     it.
	///     Parameters: "message_id"
	///   - "send_error" - indicates an upstream message expired, without being
	///     sent to the FCM server.
	///     Parameters: "message_id" and "error"
	///
	/// If this field is missing, the message is a regular message.
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString MessageType;

	/// Sets the priority of the message. Valid values are "normal" and "high." On
	/// iOS, these correspond to APNs priority 5 and 10.
	///
	/// By default, messages are sent with normal priority. Normal priority
	/// optimizes the client app's battery consumption, and should be used unless
	/// immediate delivery is required. For messages with normal priority, the app
	/// may receive the message with unspecified delay.
	///
	/// When a message is sent with high priority, it is sent i§mmediately, and the
	/// app can wake a sleeping device and open a network connection to your
	/// server.
	///
	/// For more information, see [Setting the priority of a message][1].
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	///
	/// [1]:
	/// https://firebase.google.com/docs/cloud-messaging/concept-options#setting-the-priority-of-a-message
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Priority;

	/// This parameter specifies how long (in seconds) the message should be kept
	/// in FCM storage if the device is offline. The maximum time to live
	/// supported is 4 weeks, and the default value is 4 weeks. For more
	/// information, see [Setting the lifespan of a message][1].
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	///
	/// [1]: https://firebase.google.com/docs/cloud-messaging/concept-options#ttl
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	int32 TimeToLive;

	/// Error code. Used in "nack" messages for CCS, and in responses from the
	/// server.
	/// See the CCS specification for the externally-supported list.
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Error;

	/// Human readable details about the error.
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString ErrorDescription;

	/// Optional notification to show. This only set if a notification was
	/// received with this message, otherwise it is null.
	///
	/// The notification is only guaranteed to be valid during the call to
	/// Listener::OnMessage(). If you need to keep it around longer you will need
	/// to make a copy of either the Message or Notification. Copying the Message
	/// object implicitly makes a deep copy of the notification (allocated with
	/// new) which is owned by the Message.
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FFirebaseNotification Notification;

	/// A flag indicating whether this message was opened by tapping a
	/// notification in the OS system tray. If the message was received this way
	/// this flag is set to true.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	bool bNotificationOpened;

	/// The link into the app from the message.
	///
	/// This field is only used for downstream messages received through
	/// Listener::OnMessage().
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString Link;

	/// @cond FIREBASE_APP_INTERNAL
	/// Original priority of the message.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	FString OriginalPriority;

	/// UTC timestamp in milliseconds when the message was sent.
	/// See https://en.wikipedia.org/wiki/Unix_time for details of UTC.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Messaging|Message")
	int64 SentTime;

	/// If this FCM message is a valid message containing received data.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Firebase|Messaging|Message")
	bool bIsValid;
};


DECLARE_DELEGATE_OneParam(FFirebaseMessagingCallback, const EFirebaseMessagingError);

DECLARE_MULTICAST_DELEGATE_OneParam(FFirebaseMessagingOnMessage,		const FFirebaseMessage&);
DECLARE_MULTICAST_DELEGATE_OneParam(FFirebaseMessagingOnTokenReceived,	const FString&);


class FIREBASEFEATURES_API FFirebaseMessagingLibrary : public UBlueprintFunctionLibrary
{
private:
	FFirebaseMessagingLibrary() = delete;
	FFirebaseMessagingLibrary(const FFirebaseMessagingLibrary&) = delete;
	FFirebaseMessagingLibrary(FFirebaseMessagingLibrary&&) = delete;

	friend class FMessagingListener;
	
public:
	/// @brief Terminate Firebase Cloud Messaging.
	///
	/// Frees resources associated with Firebase Cloud Messaging.
	///
	/// @note On Android, the services will not be shut down by this method.
	static void Terminate();

	/// Determines if automatic token registration during initalization is enabled.
	///
	/// @return true if auto token registration is enabled and false if disabled.
	static bool IsTokenRegistrationOnInitEnabled();

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
	static void SetTokenRegistrationOnInitEnabled(const bool bEnable);

	/// @brief Displays a prompt to the user requesting permission to display
	///        notifications.
	///
	/// The permission prompt only appears on iOS. If the user has already agreed to
	/// allow notifications, no prompt is displayed and the returned future is
	/// completed immediately.
	///
	/// @param Callback A Callback that completes when the notification prompt has been
	///         dismissed.
	static void RequestPermission(FFirebaseMessagingCallback Callback);

	/// Send an upstream ("device to cloud") message. You can only use the upstream
	/// feature if your FCM implementation uses the XMPP-based Cloud Connection
	/// Server. The current limits for max storage time and number of outstanding
	/// messages per application are documented in the [FCM Developers Guide].
	///
	/// [FCM Developers Guide]: https://firebase.google.com/docs/cloud-messaging/
	///
	/// @param[in] message The message to send upstream.
	[[ deprecated ]]
	static void Send(const FFirebaseMessage& Message);

	/// @brief Subscribe to receive all messages to the specified topic.
	///
	/// Subscribes an app instance to a topic, enabling it to receive messages
	/// sent to that topic.
	///
	/// Call this function from the main thread. FCM is not thread safe.
	///
	/// @param[in] Topic The name of the topic to subscribe. Must match the
	///            following regular expression: `[a-zA-Z0-9-_.~%]{1,900}`.
	static void Subscribe(const FString& Topic, FFirebaseMessagingCallback Callback);

	/// @brief Unsubscribe from a topic.
	///
	/// Unsubscribes an app instance from a topic, stopping it from receiving
	/// any further messages sent to that topic.
	///
	/// Call this function from the main thread. FCM is not thread safe.
	///
	/// @param[in] topic The name of the topic to unsubscribe from. Must match the
	///            following regular expression: `[a-zA-Z0-9-_.~%]{1,900}`.
	static void Unsubscribe(const FString& Topic, FFirebaseMessagingCallback Callback);

	/// Determines whether Firebase Cloud Messaging exports message delivery metrics
	/// to BigQuery.
	///
	/// This function is currently only implemented on Android, and returns false
	/// with no other behavior on other platforms.
	///
	/// @return true if Firebase Cloud Messaging exports message delivery metrics to
	/// BigQuery.
	static bool IsDeliveryMetricsExportToBigQueryEnabled();

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
	static void SetDeliveryMetricsExportToBigQuery(const bool bEnable);

	/**
	 * Gets the cached Firebase Cloud Messaging token if any.
	 * @return The cached Firebase Cloud Messaging token or an empty string if none.
	*/
	static FString GetCachedCloudMessagingToken();

	/**
	 * Gets the last message received through FCM.
	 * @return The last message received through FCM.
	*/
	static FFirebaseMessage GetCachedLastMessage();

	static FORCEINLINE FFirebaseMessagingOnTokenReceived& OnTokenReceived()
	{
		return OnTokenReceivedEvent;
	}

	static FORCEINLINE FFirebaseMessagingOnMessage& OnMessage()
	{
		return OnMessageEvent;
	}

private:
	static FFirebaseMessagingOnTokenReceived OnTokenReceivedEvent;
	static FFirebaseMessagingOnMessage		 OnMessageEvent;

	static FString CachedFirebaseCloudMessagingToken;
	static FFirebaseMessage CachedLastMessage;
};

