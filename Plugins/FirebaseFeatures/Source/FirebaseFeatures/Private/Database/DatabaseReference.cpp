// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Database/DatabaseReference.h"

#include "FirebaseFeatures.h"

#if WITH_FIREBASE_DATABASE
THIRD_PARTY_INCLUDES_START
#	include "firebase/database.h"
#	include "firebase/variant.h"
THIRD_PARTY_INCLUDES_END
#endif

#if WITH_EDITOR && WITH_FIREBASE_DATABASE
#	include "Editor.h"
#endif


#include "Async/Async.h"

/**
 *	Not a function, we don't want to copy the ErrorMessage across calls...
 **/
#define CreateCallbackForFutureVoid(ErrorMessage)														\
	[Callback](const firebase::Future<void>& Future) -> void											\
	{																									\
		const EFirebaseDatabaseError Error = (EFirebaseDatabaseError)Future.error();					\
		if (Error != EFirebaseDatabaseError::None)														\
		{																								\
			UE_LOG(LogFirebaseDatabase, Error, TEXT(ErrorMessage) TEXT(" Code: %d. Message: %s"),		\
				Future.error(), UTF8_TO_TCHAR(Future.error_message()));									\
		}																								\
		AsyncTask(ENamedThreads::GameThread, [Error, Callback]() -> void								\
		{																								\
			Callback.ExecuteIfBound(Error);																\
		});																								\
	}																							

	
class FChildListener final 
#if WITH_FIREBASE_DATABASE
	: public firebase::database::ChildListener
#endif
{
#if WITH_FIREBASE_DATABASE
 public:
	 FChildListener(UDatabaseQuery* const InQuery)
		 : Query(InQuery)
	 {
	 }
	virtual ~FChildListener()
	{
	}

	virtual void OnChildAdded(const firebase::database::DataSnapshot& snapshot, const char* previous_sibling_key)
	{
#if WITH_EDITOR
		UE_LOG(LogFirebaseDatabase, Log, TEXT("Child Added Event fired."));
#endif

		FString Key = previous_sibling_key ? UTF8_TO_TCHAR(previous_sibling_key) : TEXT("");

		AsyncTask(ENamedThreads::GameThread, [snapshot, Key = MoveTemp(Key), LocalQuery = this->Query]() -> void
		{
			if (LocalQuery.IsValid())
			{
				UDataSnapshot* const Snap = NewObject<UDataSnapshot>();
				Snap->Snapshot = snapshot;

				LocalQuery->OnChildAdded.Broadcast(Snap, Key);
			}
		});
	}
	virtual void OnChildChanged(const firebase::database::DataSnapshot& snapshot, const char* previous_sibling_key)
	{
#if WITH_EDITOR
		UE_LOG(LogFirebaseDatabase, Log, TEXT("Child Changed Event fired."));
#endif

		FString Key = UTF8_TO_TCHAR(previous_sibling_key);
		TWeakObjectPtr<UDatabaseQuery> LocalQuery = Query;

		AsyncTask(ENamedThreads::GameThread, [snapshot, Key = MoveTemp(Key), LocalQuery]() -> void
		{
			if (LocalQuery.IsValid())
			{
				UDataSnapshot* const Snap = NewObject<UDataSnapshot>();
				Snap->Snapshot = snapshot;

				LocalQuery->OnChildChanged.Broadcast(Snap, Key);
			}
		});
	}
	virtual void OnChildMoved(const firebase::database::DataSnapshot& snapshot, const char* previous_sibling_key)
	{
#if WITH_EDITOR
		UE_LOG(LogFirebaseDatabase, Log, TEXT("Child Moved Event fired."));
#endif

		FString Key = previous_sibling_key ? UTF8_TO_TCHAR(previous_sibling_key) : TEXT("");

		AsyncTask(ENamedThreads::GameThread, [snapshot, Key = MoveTemp(Key), LocalQuery = Query]() -> void
		{
			if (LocalQuery.IsValid())
			{
				UDataSnapshot* const Snap = NewObject<UDataSnapshot>();
				Snap->Snapshot = snapshot;

				LocalQuery->OnChildMoved.Broadcast(Snap, Key);
			}
		});
	}
	virtual void OnChildRemoved(const firebase::database::DataSnapshot& snapshot)
	{
#if WITH_EDITOR
		UE_LOG(LogFirebaseDatabase, Log, TEXT("Child Removed Event fired."));
#endif

		AsyncTask(ENamedThreads::GameThread, [snapshot, LocalQuery = Query]() -> void
		{
			if (LocalQuery.IsValid())
			{
				UDataSnapshot* const Snap = NewObject<UDataSnapshot>();
				Snap->Snapshot = snapshot;

				LocalQuery->OnChildRemoved.Broadcast(Snap, TEXT(""));
			}
		});
	}
	virtual void OnCancelled(const firebase::database::Error& error, const char* error_message)
	{
#if WITH_EDITOR
		UE_LOG(LogFirebaseDatabase, Log, TEXT("Child Event Cancelled fired. Reason: %s"), UTF8_TO_TCHAR(error_message));
#endif

		FString Message = UTF8_TO_TCHAR(error_message);
	
		const EFirebaseDatabaseError Error = (EFirebaseDatabaseError)error;

		AsyncTask(ENamedThreads::GameThread, [Error, Message = MoveTemp(Message), LocalQuery = Query]() -> void
		{
			if (LocalQuery.IsValid())
			{
				LocalQuery->OnCancelled.Broadcast(Error, Message);
			}
		});
	}
private:
	TWeakObjectPtr<UDatabaseQuery> Query;
#endif
};


class FValueListener final 
#if WITH_FIREBASE_DATABASE
	: public firebase::database::ValueListener
#endif
{
#if WITH_FIREBASE_DATABASE
public:
	FValueListener(UDatabaseQuery* const InQuery)
		: Query(InQuery)
	{
	}
	virtual ~FValueListener()
	{

	}
	virtual void OnValueChanged(const firebase::database::DataSnapshot& snapshot)
	{
#if WITH_EDITOR
		UE_LOG(LogFirebaseDatabase, Log, TEXT("A value with a listener has changed."));
#endif

		AsyncTask(ENamedThreads::GameThread, [snapshot, LocalQuery = this->Query]() -> void
		{
			if (LocalQuery.IsValid())
			{
				UDataSnapshot* const Snap = NewObject<UDataSnapshot>();
				Snap->Snapshot = snapshot;
				LocalQuery->OnValueChanged.Broadcast(Snap);
			}
		});
	}
	virtual void OnCancelled(const firebase::database::Error& error, const char* error_message)
	{
#if WITH_EDITOR
		UE_LOG(LogFirebaseDatabase, Warning, TEXT("Value Listener has been cancelled. Reason: %s"), UTF8_TO_TCHAR(error_message));
#endif
		FString Message = UTF8_TO_TCHAR(error_message);
		
		const EFirebaseDatabaseError Error = (EFirebaseDatabaseError)error;

		AsyncTask(ENamedThreads::GameThread, [Error, Message = MoveTemp(Message), 
			LocalQuery = this->Query]() -> void
		{
			if (LocalQuery.IsValid())
			{
				LocalQuery->OnCancelled.Broadcast(Error, Message);
			}
		});
	}
private:
	TWeakObjectPtr<UDatabaseQuery> Query;
#endif
};

#if WITH_FIREBASE_DATABASE
FMutableData::FMutableData(firebase::database::MutableData&& InMutable)
	: Mutable(MakeShared<firebase::database::MutableData>(MoveTemp(InMutable)))
{
}

FMutableData::FMutableData() 
{
}

FMutableData::FMutableData(const FMutableData& Other)
{
	*this = Other;
}

FMutableData::~FMutableData()
{
}

FMutableData& FMutableData::operator=(const FMutableData& Other)
{
	Mutable = nullptr;
	if (Other.Mutable.IsValid())
	{
		Mutable = Other.Mutable;
	}
	return *this;
}

FMutableData FMutableData::GetChild(const FString& Path)
{
	return Mutable.IsValid() ? Mutable->Child(TCHAR_TO_UTF8(*Path)) : FMutableData();
}

TArray<FMutableData> FMutableData::GetChildren()
{
	TArray<FMutableData> Mutables;

	if (Mutable)
	{
		auto Children = Mutable->children();
		Mutables.Reserve(Children.size());

		for (auto& Child : Children)
		{
			Mutables.Emplace(MoveTemp(Child));
		}
	}

	return Mutables;
}

int64 FMutableData::GetChildrenCount()
{
	return Mutable ? Mutable->children_count() : 0;
}

FString FMutableData::GetKey() const
{
	return Mutable ? UTF8_TO_TCHAR(Mutable->key()) : TEXT("");
}

FFirebaseVariant FMutableData::GetValue() const
{
	if (!Mutable)
	{
		UE_LOG(LogFirebaseDatabase, Warning, TEXT("Called GetValue() on an invalide mutable data."));
		return FFirebaseVariant();
	}

	return Mutable->value();
}

FFirebaseVariant FMutableData::GetPriority()
{
	return Mutable ? FFirebaseVariant(Mutable->priority()) : FFirebaseVariant();
}

bool FMutableData::HasChild(const FString& Path) const
{
	return Mutable ? Mutable->HasChild(TCHAR_TO_UTF8(*Path)) : false;
}

void FMutableData::SetValue(const FFirebaseVariant& Value)
{
	if (Mutable)
	{
		Mutable->set_value(Value);
	}
}

void FMutableData::SetPriority(const FFirebaseVariant& Priority)
{
	if (Mutable)
	{
		Mutable->set_priority(Priority);
	}
}
#endif

#if WITH_FIREBASE_DATABASE
void UDatabaseQuery::SetQuery(const firebase::database::Query& InQuery)
{
	GetQuery() = InQuery;
}
#endif

bool UDataSnapshot::Exists() const
{
#if WITH_FIREBASE_DATABASE
	return Snapshot.exists();
#else
	return false;
#endif
}

UDataSnapshot* UDataSnapshot::GetChild(const FString& Path) const
{
#if WITH_FIREBASE_DATABASE
	UDataSnapshot* const Snap = NewObject<UDataSnapshot>();
	Snap->Snapshot = Snapshot.Child(TCHAR_TO_UTF8(*Path));
	return Snap;
#else
	return nullptr;
#endif
}

TArray<UDataSnapshot*> UDataSnapshot::GetChildren() const
{
#if WITH_FIREBASE_DATABASE
	const std::vector<firebase::database::DataSnapshot> Snapshots = Snapshot.children();
	
	TArray<UDataSnapshot*> Snaps;

	for (const auto& RawSnap : Snapshots)
	{
		UDataSnapshot* const Snap = NewObject<UDataSnapshot>();
		
		Snap->Snapshot = RawSnap;
		
		Snaps.Add(Snap);
	}

	return Snaps;
#else
	return {};
#endif
}

bool UDataSnapshot::HasChildren() const
{
#if WITH_FIREBASE_DATABASE
	return Snapshot.has_children();
#else
	return false;
#endif
}

int64 UDataSnapshot::ChildrenCount() const
{
#if WITH_FIREBASE_DATABASE
	return (int64)Snapshot.children_count();
#else
	return 0;
#endif
}

FString UDataSnapshot::GetKey() const
{
#if WITH_FIREBASE_DATABASE
	return UTF8_TO_TCHAR(Snapshot.key());
#else
	return {};
#endif
}

FFirebaseVariant UDataSnapshot::GetValue() const
{
#if WITH_FIREBASE_DATABASE
	return Snapshot.value();
#else
	return {};
#endif
}

FFirebaseVariant UDataSnapshot::GetPriority() const
{
#if WITH_FIREBASE_DATABASE
	return Snapshot.priority();
#else
	return {};
#endif
}

UDatabaseReference* UDataSnapshot::GetReference() const
{
#if WITH_FIREBASE_DATABASE
	UDatabaseReference* const Ref = NewObject<UDatabaseReference>();
	Ref->Reference = Snapshot.GetReference();
	return Ref;
#else
	return nullptr;
#endif
}

bool UDataSnapshot::HasChild(const FString& Path) const
{
#if WITH_FIREBASE_DATABASE
	return Snapshot.HasChild(TCHAR_TO_UTF8(*Path));
#else
	return false;
#endif
}

bool UDataSnapshot::IsValid() const
{
#if WITH_FIREBASE_DATABASE
	return Snapshot.is_valid();
#else
	return false;
#endif
}

#if WITH_FIREBASE_DATABASE
/* static */ firebase::database::TransactionResult UDatabaseReference::DoTransactionWithContext(firebase::database::MutableData* data, void* context)
{
	FTransactionCallback* const Callback = (FTransactionCallback*)context;

	ETransactionResult Res = ETransactionResult::Success;
	if (Callback->IsBound())
	{
		FMutableData Data(MoveTemp(*data));
		Res = Callback->Execute(Data);
	}

	delete Callback;

	return firebase::database::TransactionResult(Res);
}
#endif

UDatabase* UDatabaseReference::GetDatabase() const
{
#if WITH_FIREBASE_DATABASE
	if (!Reference.database())
	{
		return nullptr;
	}

	UDatabase* const Database = NewObject<UDatabase>();
	Database->Url = UTF8_TO_TCHAR(Reference.database()->url());

	return Database;
#else
	return nullptr;
#endif
}

FString UDatabaseReference::GetKey() const
{
#if WITH_FIREBASE_DATABASE
	return UTF8_TO_TCHAR(Reference.key());
#else 
	return {};
#endif
}

bool UDatabaseReference::IsRoot() const
{
#if WITH_FIREBASE_DATABASE
	return Reference.is_root();
#else
	return false;
#endif
}

bool UDatabaseReference::IsValid() const
{
#if WITH_FIREBASE_DATABASE
	return Reference.is_valid();
#else
	return false;
#endif
}

UDatabaseReference* UDatabaseReference::GetParent() const
{
#if WITH_FIREBASE_DATABASE
	UDatabaseReference* const Ref = NewObject<UDatabaseReference>();

	Ref->Reference = Reference.GetParent();

	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseReference* UDatabaseReference::GetRoot() const
{
#if WITH_FIREBASE_DATABASE
	UDatabaseReference* const Ref = NewObject<UDatabaseReference>();

	Ref->Reference = Reference.GetRoot();

	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseReference* UDatabaseReference::Child(const FString& Path) const
{
#if WITH_FIREBASE_DATABASE
	UDatabaseReference* const Ref = NewObject<UDatabaseReference>();

	Ref->Reference = Reference.Child(TCHAR_TO_UTF8(*Path));

	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseReference* UDatabaseReference::ChildFromPaths(const TArray<FString>& Paths) const
{
#if WITH_FIREBASE_DATABASE
	UDatabaseReference* const Ref = NewObject<UDatabaseReference>();

	Ref->Reference = Reference;

	for (const auto& Path : Paths)
	{
		Ref->Reference = Reference.Child(TCHAR_TO_UTF8(*Path));
	}

	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseReference* UDatabaseReference::PushChild() const
{
#if WITH_FIREBASE_DATABASE
	UDatabaseReference* const Ref = NewObject<UDatabaseReference>();

	Ref->Reference = Reference.PushChild();

	return Ref;
#else
	return nullptr;
#endif
}

#define CHECK_DATABASE_REFERENCE_VALIDITY()											\
	//if (!Query->is_valid())														\
	//{																				\
	//	UE_LOG(LogFirebaseDatabase, Error, TEXT("Database Reference is invalid."));	\
	//	Callback.ExecuteIfBound(EFirebaseDatabaseError::OperationFailed);			\
	//	return;																		\
	//}

void UDatabaseReference::RemoveValue(const FDatabaseCallback& Callback)
{
#if WITH_FIREBASE_DATABASE
	CHECK_DATABASE_REFERENCE_VALIDITY();
	Reference.RemoveValue().OnCompletion(CreateCallbackForFutureVoid("Failed to remove value from Database Reference."));
#endif
}

void UDatabaseReference::RunTransaction(const bool bTriggerLocalEvents, const FTransactionCallback& TransactionFunction, const FSnapshotCallback& OnTransactionOver)
{
#if WITH_FIREBASE_DATABASE
	Reference.RunTransaction
	(
		&UDatabaseReference::DoTransactionWithContext,
		new FTransactionCallback(TransactionFunction),
		bTriggerLocalEvents
	).OnCompletion([OnTransactionOver](const firebase::Future<firebase::database::DataSnapshot>& Future) -> void
	{
		firebase::database::DataSnapshot Snapshot;

		const EFirebaseDatabaseError Error = (EFirebaseDatabaseError)Future.error();
		if (Error != EFirebaseDatabaseError::None)
		{
			UE_LOG(LogFirebaseDatabase, Error, TEXT("Failed to run transaction on Database Reference. Code: %d. Message: %s"), Future.error(), UTF8_TO_TCHAR(Future.error_message()));
		}
		else
		{
			Snapshot = *Future.result();;
		}

		AsyncTask(ENamedThreads::GameThread, [OnTransactionOver, Error, Snapshot]() -> void
		{
			UDataSnapshot* Snap = nullptr;

			if (Snapshot.is_valid())
			{
				Snap = NewObject<UDataSnapshot>();
				Snap->Snapshot = Snapshot;
			}
			
			OnTransactionOver.ExecuteIfBound(Error, Snap);
		});		
	});
#endif
}

void UDatabaseReference::SetPriority(const FFirebaseVariant& Priority, const FDatabaseCallback& Callback)
{
#if WITH_FIREBASE_DATABASE
	CHECK_DATABASE_REFERENCE_VALIDITY();
	Reference.SetPriority(Priority).OnCompletion(CreateCallbackForFutureVoid("Failed to set priority on Database Reference."));
#endif
}

void UDatabaseReference::SetValue(const FFirebaseVariant& Value, const FDatabaseCallback& Callback)
{
#if WITH_FIREBASE_DATABASE
	CHECK_DATABASE_REFERENCE_VALIDITY();
	Reference.SetValue(Value).OnCompletion(CreateCallbackForFutureVoid("Failed to set value on Database Reference."));
#endif
}

void UDatabaseReference::SetValueAndPriority(const FFirebaseVariant& Value, const FFirebaseVariant& Priority, const FDatabaseCallback& Callback)
{
#if WITH_FIREBASE_DATABASE
	CHECK_DATABASE_REFERENCE_VALIDITY();
	Reference.SetValueAndPriority(Value, Priority).OnCompletion(CreateCallbackForFutureVoid("Failed to set value and priority on Database Reference."));
#endif
}

void UDatabaseReference::UpdateChildren(const FFirebaseVariant& Values, const FDatabaseCallback& Callback)
{
#if WITH_FIREBASE_DATABASE
	CHECK_DATABASE_REFERENCE_VALIDITY();
	Reference.UpdateChildren(Values).OnCompletion(CreateCallbackForFutureVoid("Failed to update children."));
#endif
}

#undef CHECK_DATABASE_REFERENCE_VALIDITYs

FString UDatabaseReference::GetUrl() const
{
#if WITH_FIREBASE_DATABASE
	return UTF8_TO_TCHAR(Reference.url().c_str());
#else
	return {};
#endif
}

void UDatabaseReference::GoOffline()
{
#if WITH_FIREBASE_DATABASE
	Reference.GoOffline();
#endif
}

void UDatabaseReference::GoOnline()
{
#if WITH_FIREBASE_DATABASE
	Reference.GoOnline();
#endif
}

UDisconnectionHandler* UDatabaseReference::GetDisconnectionHandler()
{
#if WITH_FIREBASE_DATABASE
	UDisconnectionHandler* const Handler = NewObject<UDisconnectionHandler>();
	Handler->Handler = Reference.OnDisconnect();
	return Handler;
#else
	return nullptr;
#endif
}

UDatabaseReference::~UDatabaseReference()
{
}

UDisconnectionHandler::UDisconnectionHandler() 
#if WITH_FIREBASE_DATABASE
	: Handler(nullptr) 
#endif
{
}

#define SafeHandler if (Handler) Handler

#if WITH_FIREBASE_DATABASE
void UDisconnectionHandler::Cancel(const FDatabaseCallback& Callback)
{
	SafeHandler->Cancel().OnCompletion(CreateCallbackForFutureVoid("Failed to cancel Disconnection operations."));
}

void UDisconnectionHandler::RemoveValue(const FDatabaseCallback& Callback)
{
	SafeHandler->RemoveValue().OnCompletion(CreateCallbackForFutureVoid("Failed to remove value from disconnection handler."));
}

void UDisconnectionHandler::SetValue(const FFirebaseVariant& Value, const FDatabaseCallback& Callback)
{
	SafeHandler->SetValue(Value).OnCompletion(CreateCallbackForFutureVoid("Failed to set value from disconnection handler."));
}

void UDisconnectionHandler::SetValueAndPriority(const FFirebaseVariant& Value, const FFirebaseVariant& Priority, const FDatabaseCallback& Callback)
{
	SafeHandler->SetValueAndPriority(Value, Priority).OnCompletion(CreateCallbackForFutureVoid("Failed to set value and priority for disconnection handler"));
}

void UDisconnectionHandler::UpdateChildren(const FFirebaseVariant& values, const FDatabaseCallback& Callback)
{
	SafeHandler->UpdateChildren(values).OnCompletion(CreateCallbackForFutureVoid("Failed to update children for disconnection handler."));
}
#endif

UDatabaseQuery::UDatabaseQuery()
#if WITH_FIREBASE_DATABASE
	: Query()
	, ValueListener(nullptr)
	, ChildListener(nullptr)
#endif
{
}

UDatabaseQuery::UDatabaseQuery(FVTableHelper& Helper)
	: Super(Helper)
{}

UDatabaseReference::UDatabaseReference()
#if WITH_FIREBASE_DATABASE
	: Reference()
#endif
{
#if WITH_FIREBASE_DATABASE
	ValueListener = nullptr;
	ChildListener = nullptr;
#endif
}

bool UDatabaseQuery::IsValid() const
{
#if WITH_FIREBASE_DATABASE
	return GetQuery().is_valid();
#else
	return false;
#endif
}

void UDatabaseQuery::GetValue(const FSnapshotCallback& Callback) 
{
#if WITH_FIREBASE_DATABASE
	GetQuery().GetValue().OnCompletion([Callback](const firebase::Future<firebase::database::DataSnapshot>& Future) -> void
	{
		const EFirebaseDatabaseError Error = (EFirebaseDatabaseError)Future.error();
		if (Error != EFirebaseDatabaseError::None)
		{
			UE_LOG(LogFirebaseDatabase, Error, TEXT("Failed to get value from Query.") TEXT(" Code: %d. Message: %s"),
				Future.error(), UTF8_TO_TCHAR(Future.error_message()));
		}

		firebase::database::DataSnapshot Snapshot;
		if (Future.result())
		{
			Snapshot = *Future.result();
		}

		if (Callback.IsBound())
		{
			AsyncTask(ENamedThreads::GameThread, [Error, Callback, Snapshot]() -> void
			{
				UDataSnapshot* Snap = nullptr;

				if (Snapshot.is_valid())
				{
					Snap = NewObject<UDataSnapshot>();
					Snap->Snapshot = Snapshot;
				}

				Callback.ExecuteIfBound(Error, Snap);
			});
		}
	});
#endif
}

UDatabaseQuery::~UDatabaseQuery()
{
#if WITH_FIREBASE_DATABASE
	ClearListeners();
#endif
}

void UDatabaseQuery::SetupListeners()
{
#if WITH_FIREBASE_DATABASE
	if (GetQuery().is_valid())
	{
		ClearListeners();

		ChildListener.Reset(new FChildListener(this));
		ValueListener.Reset(new FValueListener(this));

		GetQuery().AddChildListener(ChildListener.Get());
		GetQuery().AddValueListener(ValueListener.Get());

#if WITH_EDITOR
#if FIREBASE_FEATURES_UE_4_25_OR_OLDER
		using FEndPieDelegate = TBaseDelegate<void, const bool>;
#else
		using FEndPieDelegate = TDelegate<void(const bool)>;
#endif
		FEditorDelegates::EndPIE.Add(FEndPieDelegate::CreateUObject(this, &ThisClass::RemoveListenersEndPIE));
#endif
	}
	else
	{
		UE_LOG(LogFirebaseDatabase, Warning, TEXT("Called SetupListeners() on an invalid query."));
	}
#endif
}

#if WITH_EDITOR && WITH_FIREBASE_DATABASE
void UDatabaseQuery::RemoveListenersEndPIE(const bool)
{
	GetQuery().RemoveAllChildListeners();
	GetQuery().RemoveAllValueListeners();
	
	ChildListener.Reset();
	ValueListener.Reset();

	FEditorDelegates::EndPIE.RemoveAll(this);
}
#endif

void UDatabaseQuery::ClearListeners()
{
#if WITH_FIREBASE_DATABASE
	GetQuery().RemoveChildListener(ChildListener.Get());
	ChildListener.Reset();
	GetQuery().RemoveValueListener(ValueListener.Get());
	ValueListener.Reset();
#endif
}

UDatabaseReference* UDatabaseQuery::GetReference() const
{
#if WITH_FIREBASE_DATABASE
	UDatabaseReference* const Ref = NewObject<UDatabaseReference>();
	Ref->Reference = GetQuery().GetReference();
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::OrderByChild(const FString& Path)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->GetQuery() = GetQuery().OrderByChild(TCHAR_TO_UTF8(*Path));
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::OrderByKey()
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->Query = GetQuery().OrderByKey();
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::OrderByPriority()
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->Query = GetQuery().OrderByPriority();
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::OrderByValue()
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->Query = GetQuery().OrderByValue();
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::StartAt(const FFirebaseVariant& OrderValue)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->Query = GetQuery().StartAt(OrderValue);
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::StartAtKey(FFirebaseVariant OrderValue, const FString& ChildKey)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->Query = GetQuery().StartAt(OrderValue, TCHAR_TO_UTF8(*ChildKey));
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::EndAt(const FFirebaseVariant& OrderValue)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->Query = GetQuery().EndAt(OrderValue);
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::EndAtKey(const FFirebaseVariant& order_value, const FString& ChildKey)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->Query = GetQuery().EndAt(order_value, TCHAR_TO_UTF8(*ChildKey));
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::EqualTo(const FFirebaseVariant& OrderValue)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->SetQuery(GetQuery().EqualTo(OrderValue));
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::EqualToKey(const FFirebaseVariant& OrderValue, const FString& ChildKey)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->SetQuery(GetQuery().EqualTo(OrderValue, TCHAR_TO_UTF8(*ChildKey)));
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::LimitToFirst(const int64 Limit)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->SetQuery(GetQuery().LimitToFirst(Limit));
	return Ref;
#else
	return nullptr;
#endif
}

UDatabaseQuery* UDatabaseQuery::LimitToLast(const int64 Limit)
{
#if WITH_FIREBASE_DATABASE
	UDatabaseQuery* const Ref = NewObject<UDatabaseQuery>();
	Ref->SetQuery(GetQuery().LimitToLast(Limit));
	return Ref;
#else
	return nullptr;
#endif
}

void UDatabaseQuery::SetKeepSynchronized(const bool bKeepSync)
{
#if WITH_FIREBASE_DATABASE
	GetQuery().SetKeepSynchronized(bKeepSync);
#endif
}

#undef SafeHandler

#undef CreateCallbackForFutureVoid

