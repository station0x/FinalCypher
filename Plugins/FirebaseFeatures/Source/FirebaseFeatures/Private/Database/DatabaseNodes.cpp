// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Database/DatabaseNodes.h"
#include "FirebaseFeatures.h"

FMutableData UFirebaseDatabaseLibrary::Mutable_GetChild(FMutableData& MutableData, const FString& Path)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.GetChild(Path);
#else
	return {};
#endif
}

TArray<FMutableData> UFirebaseDatabaseLibrary::Mutable_GetChildren(FMutableData& MutableData)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.GetChildren();
#else
	return {};
#endif
}

int64 UFirebaseDatabaseLibrary::Mutable_GetChildrenCount(FMutableData& MutableData)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.GetChildrenCount();
#else
	return 0;
#endif
}

FString UFirebaseDatabaseLibrary::Mutable_GetKey(const FMutableData& MutableData)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.GetKey();
#else
	return {};
#endif
}

FFirebaseVariant UFirebaseDatabaseLibrary::Mutable_GetValue(const FMutableData& MutableData)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.GetValue();
#else
	return {};
#endif
}

FFirebaseVariant UFirebaseDatabaseLibrary::Mutable_GetPriority(FMutableData& MutableData)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.GetPriority();
#else
	return {};
#endif
}

bool UFirebaseDatabaseLibrary::Mutable_HasChild(const FMutableData& MutableData, const FString& Path)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.HasChild(Path);
#else
	return false;
#endif
}

void UFirebaseDatabaseLibrary::Mutable_SetValue(FMutableData& MutableData, const FFirebaseVariant& Value)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.SetValue(Value);
#endif
}

void UFirebaseDatabaseLibrary::Mutable_SetPriority(FMutableData& MutableData, const FFirebaseVariant& Priority)
{
#if WITH_FIREBASE_DATABASE
	return MutableData.SetPriority(Priority);
#endif
}

#define PROXY_CALL_RESULT(Callback, ...)  \
	(Error == EFirebaseDatabaseError::None ? Callback : OnError).Broadcast(Error, ## __VA_ARGS__); \
	SetReadyToDestroy()

#define CHECK_HANDLER(...)																		\
	if (!Handler)																				\
	{																							\
		UE_LOG(LogFirebaseDatabase, Error, TEXT("Passed nullptr for Disconnection Handler."));	\
		OnError.Broadcast(EFirebaseDatabaseError::OperationFailed, ## __VA_ARGS__);				\
		SetReadyToDestroy();																	\
		return;																					\
	}

UDisconnectionCancelProxy* UDisconnectionCancelProxy::Cancel(UDisconnectionHandler* const DisconnectionHandler)
{
	UDisconnectionCancelProxy* const Proxy = NewObject<UDisconnectionCancelProxy>();
	
	Proxy->Handler = DisconnectionHandler;

	return Proxy;
}

void UDisconnectionCancelProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_HANDLER();

	Handler->Cancel(FDatabaseCallback::CreateUObject(this, &UDisconnectionCancelProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void UDisconnectionCancelProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnCanceled);
}

UDisconnectionRemoveValueProxy* UDisconnectionRemoveValueProxy::RemoveValue(UDisconnectionHandler* const DisconnectionHandler)
{
	UDisconnectionRemoveValueProxy* const Proxy = NewObject<UDisconnectionRemoveValueProxy>();

	Proxy->Handler = DisconnectionHandler;

	return Proxy;
}

void UDisconnectionRemoveValueProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_HANDLER();

	Handler->RemoveValue(FDatabaseCallback::CreateUObject(this, &UDisconnectionRemoveValueProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void UDisconnectionRemoveValueProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnRemoved);
}

UDisconnectionSetValueProxy* UDisconnectionSetValueProxy::SetValue(UDisconnectionHandler* const DisconnectionHandler, const FFirebaseVariant& Value)
{
	UDisconnectionSetValueProxy* const Proxy = NewObject<UDisconnectionSetValueProxy>();

	Proxy->Handler = DisconnectionHandler;
	Proxy->Val = Value;

	return Proxy;
}

void UDisconnectionSetValueProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_HANDLER();

	Handler->SetValue(Val, FDatabaseCallback::CreateUObject(this, &UDisconnectionSetValueProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void UDisconnectionSetValueProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnSet);
}

UDisconnectionSetValueAndPriorityProxy* UDisconnectionSetValueAndPriorityProxy::SetValueAndPriority(UDisconnectionHandler* const DisconnectionHandler, const FFirebaseVariant& Value, const FFirebaseVariant& Priority)
{
	UDisconnectionSetValueAndPriorityProxy* const Proxy = NewObject<UDisconnectionSetValueAndPriorityProxy>();

	Proxy->Handler = DisconnectionHandler;
	Proxy->Val  = Value;
	Proxy->Prio = Priority;

	return Proxy;
}

void UDisconnectionSetValueAndPriorityProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_HANDLER();
	Handler->SetValueAndPriority(Val, Prio, FDatabaseCallback::CreateUObject(this, &UDisconnectionSetValueAndPriorityProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void UDisconnectionSetValueAndPriorityProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnSet);
}


UDisconnectionUpdateChildrenProxy* UDisconnectionUpdateChildrenProxy::UpdateChildren(UDisconnectionHandler* const DisconnectionHandler, const FFirebaseVariant& Value)
{
	UDisconnectionUpdateChildrenProxy* const Proxy = NewObject<UDisconnectionUpdateChildrenProxy>();

	Proxy->Handler = DisconnectionHandler;
	Proxy->Val = Value;

	return Proxy;
}

void UDisconnectionUpdateChildrenProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_HANDLER();

	Handler->UpdateChildren(Val, FDatabaseCallback::CreateUObject(this, &UDisconnectionUpdateChildrenProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void UDisconnectionUpdateChildrenProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnUpdated);
}

#undef CHECK_HANDLER

UDatabaseQueryGetValue* UDatabaseQueryGetValue::GetValue(UDatabaseQuery* const DatabaseQuery)
{
	UDatabaseQueryGetValue* const Proxy = NewObject< UDatabaseQueryGetValue>();
	
	Proxy->Query = DatabaseQuery;
	
	return Proxy;
}

void UDatabaseQueryGetValue::Activate()
{
#if WITH_FIREBASE_DATABASE
	if (!Query)
	{
		UE_LOG(LogFirebaseDatabase, Error, TEXT("Passed nullptr for UDatabaseQuery::GetValue()."));
		OnError.Broadcast(EFirebaseDatabaseError::OperationFailed, nullptr);
		SetReadyToDestroy();
		return;
	}

	Query->GetValue(FSnapshotCallback::CreateUObject(this, &UDatabaseQueryGetValue::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable, nullptr);
#endif
}

void UDatabaseQueryGetValue::OnActionOver(const EFirebaseDatabaseError Error, UDataSnapshot* const Snapshot)
{
	(Error == EFirebaseDatabaseError::None ? OnDataReceived : OnError).Broadcast(Error, Snapshot);
	SetReadyToDestroy();
}


#define CHECK_REFERENCE(...)																	\
	if (!Reference)																				\
	{																							\
		UE_LOG(LogFirebaseDatabase, Error, TEXT("Passed nullptr for Database Reference."));		\
		OnError.Broadcast(EFirebaseDatabaseError::OperationFailed, ## __VA_ARGS__);				\
		SetReadyToDestroy();																	\
		return;																					\
	}


URemoveValueProxy* URemoveValueProxy::RemoveValue(UDatabaseReference* const DatabaseReference)
{
	URemoveValueProxy* const Proxy = NewObject<URemoveValueProxy>();

	Proxy->Reference = DatabaseReference;

	return Proxy;
}

void URemoveValueProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_REFERENCE();

	Reference->RemoveValue(FDatabaseCallback::CreateUObject(this, &URemoveValueProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void URemoveValueProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnRemoved);
}

URunTransactionProxy* URunTransactionProxy::RunTransaction(UDatabaseReference* const DatabaseReference, const bool bTriggerLocalEvents, const FRunTransactionCallback& TransactionFunction)
{
	URunTransactionProxy* const Proxy = NewObject<URunTransactionProxy>();

	Proxy->Reference = DatabaseReference;
	Proxy->bTriggerLocalEvents = bTriggerLocalEvents;
	Proxy->TransactionFunction = TransactionFunction;

	return Proxy;
}

void URunTransactionProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_REFERENCE(nullptr);

	Reference->RunTransaction(bTriggerLocalEvents, 
		FTransactionCallback::CreateUObject(this, &URunTransactionProxy::OnTransaction),
		FSnapshotCallback::CreateUObject(this, &URunTransactionProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable, nullptr);
#endif
}

void URunTransactionProxy::OnActionOver(const EFirebaseDatabaseError Error, UDataSnapshot* const Snapshot)
{
	PROXY_CALL_RESULT(OnTrasactionOver, nullptr);
}

ETransactionResult URunTransactionProxy::OnTransaction(const FMutableData& Data)
{
#if WITH_FIREBASE_DATABASE
	if (TransactionFunction.IsBound())
	{
		return TransactionFunction.Execute(Data);
	}
	
	UE_LOG(LogFirebaseDatabase, Warning, TEXT("Transaction ran without a bound function."));
#endif
	
	return ETransactionResult::Abort;
}

USetPriorityProxy* USetPriorityProxy::SetPriority(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Priority)
{
	USetPriorityProxy* const Proxy = NewObject<USetPriorityProxy>();

	Proxy->Reference = DatabaseReference;
	Proxy->Prio = Priority;

	return Proxy;
}

void USetPriorityProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_REFERENCE();

	Reference->SetPriority(Prio, FDatabaseCallback::CreateUObject(this, &USetPriorityProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void USetPriorityProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnSet);
}

USetValueProxy* USetValueProxy::SetValue(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Value)
{
	USetValueProxy* const Proxy = NewObject<USetValueProxy>();

	Proxy->Reference = DatabaseReference;
	Proxy->Val = Value;

	return Proxy;
}

void USetValueProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_REFERENCE();

	Reference->SetValue(Val, FDatabaseCallback::CreateUObject(this, &USetValueProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void USetValueProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnSet);
}

USetValueAndPriorityProxy* USetValueAndPriorityProxy::SetValueAndPriority(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Priority, const FFirebaseVariant& Value)
{
	USetValueAndPriorityProxy* const Proxy = NewObject<USetValueAndPriorityProxy>();

	Proxy->Reference = DatabaseReference;
	Proxy->Prio = Priority;
	Proxy->Val  = Value;

	return Proxy;
}

void USetValueAndPriorityProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_REFERENCE();
	Reference->SetValueAndPriority(Val, Prio, FDatabaseCallback::CreateUObject(this, &USetValueAndPriorityProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void USetValueAndPriorityProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnSet);
}

UUpdateChildrenProxy* UUpdateChildrenProxy::UpdateChildren(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Value)
{
	UUpdateChildrenProxy* const Proxy = NewObject<UUpdateChildrenProxy>();

	Proxy->Reference = DatabaseReference;
	Proxy->Val = Value;

	return Proxy;
}

void UUpdateChildrenProxy::Activate()
{
#if WITH_FIREBASE_DATABASE
	CHECK_REFERENCE();

	Reference->UpdateChildren(Val, FDatabaseCallback::CreateUObject(this, &UUpdateChildrenProxy::OnActionOver));
#else 
	OnActionOver(EFirebaseDatabaseError::Unavailable);
#endif
}

void UUpdateChildrenProxy::OnActionOver(const EFirebaseDatabaseError Error)
{
	PROXY_CALL_RESULT(OnUpdated);
}

#undef CHECK_REFERENCE
#undef PROXY_CALL_RESULT
