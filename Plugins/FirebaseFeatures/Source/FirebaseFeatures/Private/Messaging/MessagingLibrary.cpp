// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Messaging/MessagingLibrary.h"

#include "FirebaseFeatures.h"

#if WITH_FIREBASE_MESSAGING
THIRD_PARTY_INCLUDES_START
#	include "firebase/messaging.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "Async/Async.h"


FFirebaseMessagingOnTokenReceived FFirebaseMessagingLibrary::OnTokenReceivedEvent;
FFirebaseMessagingOnMessage		  FFirebaseMessagingLibrary::OnMessageEvent;
FString							  FFirebaseMessagingLibrary::CachedFirebaseCloudMessagingToken;
FFirebaseMessage				  FFirebaseMessagingLibrary::CachedLastMessage;

static void ExecuteCallbackOnGameThread(FFirebaseMessagingCallback&& Callback, const EFirebaseMessagingError Error)
{																					
	if (Callback.IsBound())															
	{																				
		if (IsInGameThread())														
		{																			
			Callback.Execute(Error);												
		}																			
		else																		
		{																			
			AsyncTask(ENamedThreads::GameThread, [Callback = MoveTemp(Callback), Error]() -> void
			{																		
				Callback.ExecuteIfBound(Error);										
			});																		
		}																			
	}
}


FFirebaseMessage::FFirebaseMessage(const FFirebaseMessage& other)
{
	*this = other;
}

FFirebaseMessage::FFirebaseMessage(FFirebaseMessage&& other)
{
	*this = MoveTemp(other);
}

FFirebaseMessage& FFirebaseMessage::operator=(const FFirebaseMessage& other)
{
	this->    From					= other.   From					;
	this->    To					= other.   To					;
	this->    CollapseKey			= other.   CollapseKey			;
	this->    Data					= other.   Data					;
	this->    RawData				= other.   RawData				;
	this->    MessageId				= other.   MessageId			;	
	this->    MessageType			= other.   MessageType			;
	this->    Priority				= other.   Priority				;
	this->    OriginalPriority		= other.   OriginalPriority		;
	this->    SentTime				= other.   SentTime				;
	this->    TimeToLive			= other.   TimeToLive			;
	this->    Error					= other.   Error				;	
	this->    ErrorDescription		= other.   ErrorDescription		;
	this->    Notification			= other.   Notification			;
	this->    bNotificationOpened   = other.   bNotificationOpened  ; 
	this->    Link					= other.   Link					;
	this->	 bIsValid				= other.   bIsValid;
	
	return *this;
}

FFirebaseMessage& FFirebaseMessage::operator=(FFirebaseMessage&& other)
{
	this->    From					= MoveTempIfPossible(other.   From					);
	this->    To					= MoveTempIfPossible(other.   To					);
	this->    CollapseKey			= MoveTempIfPossible(other.   CollapseKey			);
	this->    Data					= MoveTempIfPossible(other.   Data					);
	this->    RawData				= MoveTempIfPossible(other.   RawData				);
	this->    MessageId				= MoveTempIfPossible(other.   MessageId				);	
	this->    MessageType			= MoveTempIfPossible(other.   MessageType			);
	this->    Priority				= MoveTempIfPossible(other.   Priority				);
	this->    OriginalPriority		= MoveTempIfPossible(other.   OriginalPriority		);
	this->    SentTime				= MoveTempIfPossible(other.   SentTime				);
	this->    TimeToLive			= MoveTempIfPossible(other.   TimeToLive			);
	this->    Error					= MoveTempIfPossible(other.   Error					);	
	this->    ErrorDescription		= MoveTempIfPossible(other.   ErrorDescription		);
	this->    Notification			= MoveTempIfPossible(other.   Notification			);
	this->    bNotificationOpened   = MoveTempIfPossible(other.   bNotificationOpened	); 
	this->    Link					= MoveTempIfPossible(other.   Link					);
	
	bIsValid = other.bIsValid;
	other.bIsValid = false;

	return *this;
}


#define CreateFutureVoidCallback(ErrorMessage)													\
	[Callback = MoveTemp(Callback)](const firebase::Future<void>& Future) mutable -> void		\
	{																							\
		const EFirebaseMessagingError Error = (EFirebaseMessagingError)Future.error();			\
		if (Error != EFirebaseMessagingError::None)												\
		{																						\
			UE_LOG(LogFirebaseMessaging, Error,													\
				TEXT(ErrorMessage) TEXT(" Code: %d. Message: %s"),								\
				UTF8_TO_TCHAR(Future.error_message()));											\
		}																						\
		ExecuteCallbackOnGameThread(MoveTemp(Callback), Error);									\
	}

#if WITH_FIREBASE_MESSAGING
FFirebaseMessage::FFirebaseMessage(const firebase::messaging::Message* const Message)
{
	bIsValid = true;

	bNotificationOpened = Message->notification_opened;
	CollapseKey			= UTF8_TO_TCHAR(Message->collapse_key		.c_str());
	Error				= UTF8_TO_TCHAR(Message->error				.c_str());
	ErrorDescription	= UTF8_TO_TCHAR(Message->error_description	.c_str());
	From				= UTF8_TO_TCHAR(Message->from				.c_str());
	Link				= UTF8_TO_TCHAR(Message->link				.c_str());
	MessageId			= UTF8_TO_TCHAR(Message->message_id			.c_str());
	MessageType			= UTF8_TO_TCHAR(Message->message_type		.c_str());
	OriginalPriority	= UTF8_TO_TCHAR(Message->original_priority	.c_str());
	Priority			= UTF8_TO_TCHAR(Message->priority			.c_str());
#if !PLATFORM_LINUX && FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	RawData				= UTF8_TO_TCHAR(Message->raw_data			.c_str());
#endif
	To					= UTF8_TO_TCHAR(Message->to					.c_str());

	SentTime	= Message->sent_time;
	TimeToLive	= Message->time_to_live;
	
	if (Message->notification)
	{
		if (Message->notification->android)
		{
			Notification.Android.ChannelId = UTF8_TO_TCHAR(Message->notification->android->channel_id.c_str());
		}

		Notification.Badge			= UTF8_TO_TCHAR(Message->notification->badge		.c_str());
		Notification.Body			= UTF8_TO_TCHAR(Message->notification->body			.c_str());
		Notification.BodyLocKey		= UTF8_TO_TCHAR(Message->notification->body_loc_key	.c_str());
		Notification.ClickAction	= UTF8_TO_TCHAR(Message->notification->click_action	.c_str());
		Notification.Color			= UTF8_TO_TCHAR(Message->notification->color		.c_str());
		Notification.Icon			= UTF8_TO_TCHAR(Message->notification->icon			.c_str());
		Notification.Sound			= UTF8_TO_TCHAR(Message->notification->sound		.c_str());
		Notification.Tag			= UTF8_TO_TCHAR(Message->notification->tag			.c_str());
		Notification.Title			= UTF8_TO_TCHAR(Message->notification->title		.c_str());
		Notification.TitleLocKey	= UTF8_TO_TCHAR(Message->notification->title_loc_key.c_str());

		Notification.BodyLocArgs.Reserve(Message->notification->body_loc_args.size());
		for (const auto& LocArg : Message->notification->body_loc_args)
		{
			Notification.BodyLocArgs.Emplace(UTF8_TO_TCHAR(LocArg.c_str()));
		}

		Notification.TitleLocArgs.Reserve(Message->notification->title_loc_args.size());
		for (const auto& LocArg : Message->notification->title_loc_args)
		{
			Notification.TitleLocArgs.Emplace(UTF8_TO_TCHAR(LocArg.c_str()));
		}
	}

	for (const auto& DataElem : Message->data)
	{
		Data.Emplace(UTF8_TO_TCHAR(DataElem.first.c_str()), UTF8_TO_TCHAR(DataElem.second.c_str()));
	}
}
#endif
	

void FFirebaseMessagingLibrary::Terminate()
{
#if WITH_FIREBASE_MESSAGING
	firebase::messaging::Terminate();
#endif // WITH_FIREBAE_MESSAGING
}

bool FFirebaseMessagingLibrary::IsTokenRegistrationOnInitEnabled()
{
#if WITH_FIREBASE_MESSAGING
	return firebase::messaging::IsTokenRegistrationOnInitEnabled();
#else 
	return false;
#endif // WITH_FIREBAE_MESSAGING
}

void FFirebaseMessagingLibrary::SetTokenRegistrationOnInitEnabled(const bool bEnable)
{
#if WITH_FIREBASE_MESSAGING
	firebase::messaging::SetTokenRegistrationOnInitEnabled(bEnable);
#endif // WITH_FIREBAE_MESSAGING
}

void FFirebaseMessagingLibrary::RequestPermission(FFirebaseMessagingCallback Callback)
{
#if WITH_FIREBASE_MESSAGING
	firebase::messaging::RequestPermission()
		.OnCompletion(CreateFutureVoidCallback("Failed to request permission."));
#endif // WITH_FIREBAE_MESSAGING
}

void FFirebaseMessagingLibrary::Send(const FFirebaseMessage& Message)
{
#if !FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	UE_LOG(LogFirebaseMessaging, Error, TEXT("Send has been removed from the C++ SDK."));
#elif WITH_FIREBASE_MESSAGING
	firebase::messaging::Message RawMessage;

	RawMessage.collapse_key = TCHAR_TO_UTF8(*Message.CollapseKey);

	for (const auto& DataElem : Message.Data)
	{
		RawMessage.data.insert({ TCHAR_TO_UTF8(*DataElem.Key), TCHAR_TO_UTF8(*DataElem.Key) });
	}

	RawMessage.error				= TCHAR_TO_UTF8(*Message.Error);
	RawMessage.error_description	= TCHAR_TO_UTF8(*Message.ErrorDescription);
	RawMessage.from					= TCHAR_TO_UTF8(*Message.From);
	RawMessage.link					= TCHAR_TO_UTF8(*Message.Link);
	RawMessage.message_id			= TCHAR_TO_UTF8(*Message.MessageId);
	RawMessage.message_type			= TCHAR_TO_UTF8(*Message.MessageType);
	RawMessage.notification_opened	= Message.bNotificationOpened;
	RawMessage.original_priority	= TCHAR_TO_UTF8(*Message.OriginalPriority);
	RawMessage.priority				= TCHAR_TO_UTF8(*Message.Priority);
#if !PLATFORM_LINUX && FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	RawMessage.raw_data				= TCHAR_TO_UTF8(*Message.RawData);
#endif
	RawMessage.sent_time			= Message.SentTime;
	RawMessage.time_to_live			= Message.TimeToLive;
	RawMessage.to					= TCHAR_TO_UTF8(*Message.To);
	
	// The message takes ownership of the pointer.
	RawMessage.notification			 = new firebase::messaging::Notification();
	RawMessage.notification->android = new firebase::messaging::AndroidNotificationParams();

	RawMessage.notification->android->channel_id = TCHAR_TO_UTF8(*Message.Notification.Android.ChannelId);

	RawMessage.notification->badge	= TCHAR_TO_UTF8(*Message.Notification.Badge);
	RawMessage.notification->body	= TCHAR_TO_UTF8(*Message.Notification.Body);
	
	RawMessage.notification->body_loc_args.reserve(Message.Notification.BodyLocArgs.Num());
	for (const auto& BodyLocArg : Message.Notification.BodyLocArgs)
	{
		RawMessage.notification->body_loc_args.emplace_back(TCHAR_TO_UTF8(*BodyLocArg));
	}

	RawMessage.notification->body_loc_key	= TCHAR_TO_UTF8(*Message.Notification.BodyLocKey);
	RawMessage.notification->click_action	= TCHAR_TO_UTF8(*Message.Notification.ClickAction);
	RawMessage.notification->color			= TCHAR_TO_UTF8(*Message.Notification.Color);
	RawMessage.notification->icon			= TCHAR_TO_UTF8(*Message.Notification.Icon);
	RawMessage.notification->sound			= TCHAR_TO_UTF8(*Message.Notification.Sound);
	RawMessage.notification->tag			= TCHAR_TO_UTF8(*Message.Notification.Tag);
	RawMessage.notification->title			= TCHAR_TO_UTF8(*Message.Notification.Title);
	RawMessage.notification->title_loc_key  = TCHAR_TO_UTF8(*Message.Notification.TitleLocKey);

	RawMessage.notification->title_loc_args.reserve(Message.Notification.TitleLocArgs.Num());
	for (const auto& TitleLocArg : Message.Notification.TitleLocArgs)
	{
		RawMessage.notification->title_loc_args.emplace_back(TCHAR_TO_UTF8(*TitleLocArg));
	}

#if !PLATFORM_LINUX
#	if PLATFORM_IOS
#		pragma clang diagnostic push
#		pragma clang diagnostic ignored "-Wdeprecated-implementations"
#	endif
	firebase::messaging::Send(RawMessage);
#	if PLATFORM_IOS
#		pragma clang diagnostic pop
#	endif
#endif

#endif // WITH_FIREBAE_MESSAGING
}

void FFirebaseMessagingLibrary::Subscribe(const FString& Topic, FFirebaseMessagingCallback Callback)
{
#if WITH_FIREBASE_MESSAGING
	firebase::messaging::Subscribe(TCHAR_TO_UTF8(*Topic))
		.OnCompletion(CreateFutureVoidCallback("Failed to subscribe to topic."));
#endif // WITH_FIREBAE_MESSAGING
}

void FFirebaseMessagingLibrary::Unsubscribe(const FString& Topic, FFirebaseMessagingCallback Callback)
{
#if WITH_FIREBASE_MESSAGING
	firebase::messaging::Unsubscribe(TCHAR_TO_UTF8(*Topic))
		.OnCompletion(CreateFutureVoidCallback("Failed to unsubscribe to topic."));
#endif // WITH_FIREBAE_MESSAGING
}

bool FFirebaseMessagingLibrary::IsDeliveryMetricsExportToBigQueryEnabled()
{
#if WITH_FIREBASE_MESSAGING
	return firebase::messaging::DeliveryMetricsExportToBigQueryEnabled();
#else
	return false;
#endif // WITH_FIREBAE_MESSAGING
}

void FFirebaseMessagingLibrary::SetDeliveryMetricsExportToBigQuery(const bool bEnable)
{
#if WITH_FIREBASE_MESSAGING
	firebase::messaging::SetDeliveryMetricsExportToBigQuery(bEnable);
#endif // WITH_FIREBAE_MESSAGING
}

FString FFirebaseMessagingLibrary::GetCachedCloudMessagingToken()
{
	return CachedFirebaseCloudMessagingToken;
}

FFirebaseMessage FFirebaseMessagingLibrary::GetCachedLastMessage()
{
	return CachedLastMessage;
}

#undef CreateFutureVoidCallback


