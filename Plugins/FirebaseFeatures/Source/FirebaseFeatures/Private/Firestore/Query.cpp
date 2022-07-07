// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Firestore/Query.h"
#include "Firestore/DocumentChange.h"

#if WITH_FIREBASE_FIRESTORE 
    THIRD_PARTY_INCLUDES_START
#		include "firebase/future.h"
#		include "firebase/firestore/query_snapshot.h"
#		include "firebase/firestore/listener_registration.h"
#		include "firebase/firestore/document_change.h"
    THIRD_PARTY_INCLUDES_END
#endif // WITH_FIREBASE_FIRESTORE 

#include "Async/Async.h"


FQuerySnapshotListenerHandle::FQuerySnapshotListenerHandle(FQuerySnapshotListenerHandle&& Other)
{
	*this = MoveTemp(Other);
}

FQuerySnapshotListenerHandle::FQuerySnapshotListenerHandle(const FQuerySnapshotListenerHandle& Other)
{
	*this = Other;
}

FQuerySnapshotListenerHandle& FQuerySnapshotListenerHandle::operator=(FQuerySnapshotListenerHandle&& Other)
{
#if WITH_FIREBASE_FIRESTORE
	Listener = MoveTemp(Other.Listener);
#endif
	return *this;
}

FQuerySnapshotListenerHandle& FQuerySnapshotListenerHandle::operator=(const FQuerySnapshotListenerHandle& Other)
{
#if WITH_FIREBASE_FIRESTORE
	Listener = Other.Listener;
#endif
	return *this;
}

#if WITH_FIREBASE_FIRESTORE
FQuerySnapshotListenerHandle::FQuerySnapshotListenerHandle(firebase::firestore::ListenerRegistration&& InListener)
	: Listener(MoveTemp(InListener))
{
}
#endif

void FQuerySnapshotListenerHandle::Remove()
{
#if WITH_FIREBASE_FIRESTORE
	Listener.Remove();
#endif
}

UFirestoreQuery::UFirestoreQuery()
#if WITH_FIREBASE_FIRESTORE 
	: Reference(new firebase::firestore::Query())
#endif
{

}

UFirestoreQuery::~UFirestoreQuery()
{
}

UFirestoreQuery* UFirestoreQuery::WhereEqualTo(const FString& Field, const FFirestoreFieldValue& Value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE
	if (Query->Reference && Reference)
	{
		auto Temp = Reference->WhereEqualTo(TCHAR_TO_UTF8(*Field), Value);
		*Query->Reference = Temp;
	}
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereEqualToFieldPath(const FFirestoreFieldPath& Field, const FFirestoreFieldValue& Value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereEqualTo(Field, Value);
#endif


	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereLessThan(const FString& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereLessThan(TCHAR_TO_UTF8(*field), value);
#endif 

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereLessThanFieldPath(const FFirestoreFieldPath& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereLessThan(field, value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereLessThanOrEqualTo(const FString& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereLessThanOrEqualTo(TCHAR_TO_UTF8(*field), value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereLessThanOrEqualToFieldPath(const FFirestoreFieldPath& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereLessThanOrEqualTo(field, value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereGreaterThan(const FString& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereGreaterThan(TCHAR_TO_UTF8(*field), value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereGreaterThanFieldPath(const FFirestoreFieldPath& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereGreaterThan(field, value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereGreaterThanOrEqualTo(const FString& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereGreaterThanOrEqualTo(TCHAR_TO_UTF8(*field), value);
#endif 

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereGreaterThanOrEqualToFielPath(const FFirestoreFieldPath& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereGreaterThanOrEqualTo(field, value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereArrayContains(const FString& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereArrayContains(TCHAR_TO_UTF8(*field), value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereArrayContainsFieldPath(const FFirestoreFieldPath& field, const FFirestoreFieldValue& value)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
		*Query->Reference = Reference->WhereArrayContains(field, value);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereArrayContainsAny(const FString& field, const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
	{
		std::vector<firebase::firestore::FieldValue> RawValues;

		RawValues.reserve(values.Num());

		for (const auto& Val : values)
		{
			RawValues.push_back(Val);
		}

		*Query->Reference = Reference->WhereArrayContainsAny(TCHAR_TO_UTF8(*field), RawValues);
	}
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereArrayContainsAnyFielPath(const FFirestoreFieldPath& field, const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
	{
		std::vector<firebase::firestore::FieldValue> RawValues;

		RawValues.reserve(values.Num());

		for (const auto& Val : values)
		{
			RawValues.push_back(Val);
		}

		*Query->Reference = Reference->WhereArrayContainsAny(field, MoveTemp(RawValues));
	}
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereIn(const FString& field, const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	std::vector<firebase::firestore::FieldValue> RawValues;

	RawValues.reserve(values.Num());

	for (const auto& Val : values)
	{
		RawValues.push_back(Val);
	}

	*Query->Reference = Reference->WhereIn(TCHAR_TO_UTF8(*field), MoveTemp(RawValues));
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::WhereInFieldPath(const FFirestoreFieldPath& field, const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	std::vector<firebase::firestore::FieldValue> RawValues;

	RawValues.reserve(values.Num());

	for (const auto& Val : values)
	{
		RawValues.push_back(Val);
	}

	*Query->Reference = Reference->WhereIn(field, MoveTemp(RawValues));
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::OrderBy(const FString& field, EFirestoreQueryDirection direction)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Query->Reference && Reference)
	{
		*Query->Reference = Reference->OrderBy(TCHAR_TO_UTF8(*field), (firebase::firestore::Query::Direction)direction);
	}
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::OrderByFieldPath(const FFirestoreFieldPath& field, EFirestoreQueryDirection direction)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	*Query->Reference = Reference->OrderBy(field, (firebase::firestore::Query::Direction)direction);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::Limit(int32 limit)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Reference && Query && Query->Reference)
	{
		*Query->Reference = Reference->Limit(FMath::Max(limit, 1));
	}
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::LimitToLast(int32 limit)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();
	
#if WITH_FIREBASE_FIRESTORE 
	if (Reference && Query->Reference)
		*Query->Reference = Reference->LimitToLast(limit);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::StartAt(const FFirestoreDocumentSnapshot& snapshot)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	if (Reference && Query->Reference)
		*Query->Reference = Reference->StartAt(snapshot);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::StartAtFieldValue(const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	std::vector<firebase::firestore::FieldValue> RawValues;

	RawValues.reserve(values.Num());

	for (const auto& Val : values)
	{
		RawValues.push_back(Val);
	}

	*Query->Reference = Reference->StartAt(RawValues);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::StartAfter(const FFirestoreDocumentSnapshot& snapshot)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();
	
#if WITH_FIREBASE_FIRESTORE 
	if (Reference && Query->Reference)
		*Query->Reference = Reference->StartAfter(snapshot);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::StartAfterFieldValue(const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	std::vector<firebase::firestore::FieldValue> RawValues;

	RawValues.reserve(values.Num());

	for (const auto& Val : values)
	{
		RawValues.push_back(Val);
	}

	*Query->Reference = Reference->StartAfter(RawValues);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::EndBefore(const FFirestoreDocumentSnapshot& snapshot)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	*Query->Reference = Reference->EndBefore(snapshot);
#endif

	return Query;
}

UFirestoreQuery* UFirestoreQuery::EndBeforeFieldValue(const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	std::vector<firebase::firestore::FieldValue> RawValues;

	RawValues.reserve(values.Num());

	for (const auto& Val : values)
	{
		RawValues.push_back(Val);
	}

	*Query->Reference = Reference->EndBefore(RawValues);
#endif
	
	return Query;
}

UFirestoreQuery* UFirestoreQuery::EndAt(const FFirestoreDocumentSnapshot& snapshot)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	*Query->Reference = Reference->EndAt(snapshot);
#endif
	
	return Query;
}

UFirestoreQuery* UFirestoreQuery::EndAtValues(const TArray<FFirestoreFieldValue>& values)
{
	UFirestoreQuery* const Query = NewObject<UFirestoreQuery>();

#if WITH_FIREBASE_FIRESTORE 
	std::vector<firebase::firestore::FieldValue> RawValues;

	RawValues.reserve(values.Num());

	for (const auto& Val : values)
	{
		RawValues.push_back(Val);
	}

	*Query->Reference = Reference->EndAt(RawValues);
#endif
	
	return Query;
}

void UFirestoreQuery::Get(const EFirestoreSource Source, FFirestoreQueryCallback Callback) const
{
#if WITH_FIREBASE_FIRESTORE 
	using namespace firebase;

	if (!Reference)
	{
		Callback.ExecuteIfBound(EFirestoreError::FailedPrecondition, {}, {});
		return;
	}

	// Performs a copy of the node to prevent being destroyed before the operation ends
	// because of UE's garbage collection through the parent object.
	firestore::Query* const Copy = new firestore::Query(*Reference);

	Copy->Get(static_cast<firestore::Source>(Source)).OnCompletion(
		[Callback = MoveTemp(Callback), Copy](const Future<firestore::QuerySnapshot>& Result) mutable -> void
	{
		const EFirestoreError Error = (EFirestoreError)Result.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to get Query. Reason: %s."), UTF8_TO_TCHAR(Result.error_message()));
		}

		TArray<FFirestoreDocumentSnapshot> Snapshots;

		std::vector<firebase::firestore::DocumentChange> Changes;

		if (const firestore::QuerySnapshot* const QuerySnap = Result.result())
		{
			Snapshots.Reserve(QuerySnap->documents().size());
			for (firestore::DocumentSnapshot& Document : QuerySnap->documents())
			{
				Snapshots.Emplace(FFirestoreDocumentSnapshot(Document));
			}

			Changes = QuerySnap->DocumentChanges();
		}

		AsyncTask(ENamedThreads::GameThread, [Copy, Changes = MoveTemp(Changes), Callback = MoveTemp(Callback), Snapshots = MoveTemp(Snapshots), Error]() mutable -> void
		{
			TArray<UFirestoreDocumentChange*> QueryChanges;
			QueryChanges.Reserve(Changes.size());

			for (auto& Change : Changes)
			{
				*(QueryChanges.Add_GetRef(NewObject<UFirestoreDocumentChange>()))->Internal = MoveTemp(Change);
			}

			Callback.ExecuteIfBound(Error, MoveTemp(Snapshots), MoveTemp(QueryChanges));

			delete Copy;
		});
	});
#endif // WITH_FIREBASE_FIRESTORE 
}

void UFirestoreQuery::Get(FFirestoreQueryCallback Callback) const
{
	Get(EFirestoreSource::Default, MoveTemp(Callback));
}

FQuerySnapshotListenerHandle UFirestoreQuery::AddSnapshotListener(FQuerySnapshotListener Listener)
{
	return AddSnapshotListener(FQuerySnapshotListenerCallback::CreateLambda(
		[Listener = MoveTemp(Listener)]
		(
			const EFirestoreError Error,
			const TArray<FFirestoreDocumentSnapshot>& Snapshots,
			const TArray<UFirestoreDocumentChange*>& Changes
		) -> void
		{
			Listener.ExecuteIfBound(Error, Snapshots, Changes);
		}
	));
}

FQuerySnapshotListenerHandle UFirestoreQuery::AddSnapshotListener(FQuerySnapshotListenerCallback Callback)
{
#if !PLATFORM_LINUX && WITH_FIREBASE_FIRESTORE && defined(FIREBASE_USE_STD_FUNCTION) && !PLATFORM_IOS
	return Reference->AddSnapshotListener([Listener = MoveTemp(Callback)]
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	(const firebase::firestore::QuerySnapshot& Result, firebase::firestore::Error Error) -> void
#else
	(const firebase::firestore::QuerySnapshot& Result, firebase::firestore::Error Error, const std::string& Message) -> void
#endif
	{
#if !FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
		if (Error != firebase::firestore::Error::kErrorOk)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to add snapshot listener: %s"), UTF8_TO_TCHAR(Message.c_str()));
		}
#endif
		TArray<FFirestoreDocumentSnapshot> Snapshots;

		Snapshots.Reserve(Result.documents().size());

		for (firebase::firestore::DocumentSnapshot& Document : Result.documents())
		{
			Snapshots.Emplace(FFirestoreDocumentSnapshot(Document));
		}

		AsyncTask(ENamedThreads::GameThread, [RawDocumentChanges = Result.DocumentChanges(), Listener, Snapshots = MoveTemp(Snapshots), Error]() -> void
		{
			TArray<UFirestoreDocumentChange*> Changes;
			Changes.Reserve(RawDocumentChanges.size());

			for (const firebase::firestore::DocumentChange& Change : RawDocumentChanges)
			{
				UFirestoreDocumentChange* const DocChange = Changes.Emplace_GetRef(NewObject<UFirestoreDocumentChange>());

				*DocChange->Internal = Change;
			}

			Listener.ExecuteIfBound((EFirestoreError)Error, Snapshots, Changes);
		});		
	});
#else 
	Callback.ExecuteIfBound(EFirestoreError::Unavailable, {}, {});
	return FQuerySnapshotListenerHandle();
#endif
}

bool UFirestoreQuery::IsValid() const
{
#if WITH_FIREBASE_FIRESTORE
	return !!Reference;
#else
	return false;
#endif
}
