// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Firestore/DocumentReference.h"
#include "Firestore/CollectionReference.h"
#include "Firestore/DocumentSnapshot.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/future.h"
#	include "firebase/firestore/document_reference.h"
#	include "firebase/firestore/collection_reference.h"
#	include "firebase/firestore/listener_registration.h"
THIRD_PARTY_INCLUDES_END

#include "Async/Async.h"

UFirestoreDocumentReference::UFirestoreDocumentReference(FVTableHelper& Helper) : Super(Helper)
{

}

UFirestoreDocumentReference::UFirestoreDocumentReference() : Super()
#if WITH_FIREBASE_FIRESTORE
	, Reference(MakeUnique<firebase::firestore::DocumentReference>())
#endif
{
}

UFirestoreDocumentReference::~UFirestoreDocumentReference()
{

}

FString UFirestoreDocumentReference::GetId() const
{
#if WITH_FIREBASE_FIRESTORE 
	return UTF8_TO_TCHAR(Reference->id().c_str());
#else 
	return TEXT("FIRESTORE_DISABLED");
#endif // WITH_FIREBASE_FIRESTORE 
}

FString UFirestoreDocumentReference::GetPath() const
{
#if WITH_FIREBASE_FIRESTORE 
	return UTF8_TO_TCHAR(Reference->path().c_str());
#else 
	return TEXT("FIRESTORE_DISABLED");
#endif // WITH_FIREBASE_FIRESTORE 
}

UFirestoreCollectionReference* UFirestoreDocumentReference::GetParent() const
{
	UFirestoreCollectionReference* const Collection = NewObject<UFirestoreCollectionReference>();

#if WITH_FIREBASE_FIRESTORE 
	*Collection->Reference = Reference->Parent();
#endif // WITH_FIREBASE_FIRESTORE 

	return Collection;
}

UFirestoreCollectionReference* UFirestoreDocumentReference::GetCollection(const FString& CollectionPath) const
{
	UFirestoreCollectionReference* const Collection = NewObject<UFirestoreCollectionReference>();

#if WITH_FIREBASE_FIRESTORE 
	*Collection->Reference = Reference->Collection(std::string(TCHAR_TO_UTF8(*CollectionPath)));
#endif // WITH_FIREBASE_FIRESTORE 

	return Collection;
}

void UFirestoreDocumentReference::Get(EFirestoreSource Source, FDocumentSnapshotCallback Callback) const
{
#if WITH_FIREBASE_FIRESTORE 
	check(Reference);

	using FNativeDocumentReference = firebase::firestore::DocumentReference;

	// Makes a shallow copy of the reference, deleting it only once the operation
	// is over to make sure it outlives the call.
	FNativeDocumentReference* const Copy = new FNativeDocumentReference(*Reference);

	Copy->Get((firebase::firestore::Source)Source).OnCompletion
	([Copy, Callback = MoveTemp(Callback)](const firebase::Future<firebase::firestore::DocumentSnapshot>& Future) mutable -> void
	{
		const EFirestoreError Error = (EFirestoreError) Future.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to get document Reference-> Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			firebase::firestore::DocumentSnapshot Snap = 
				Future.result() ? *Future.result() : firebase::firestore::DocumentSnapshot();
			AsyncTask(ENamedThreads::GameThread, [Copy, Callback = MoveTemp(Callback), Error, Snap = MoveTemp(Snap)]()mutable -> void
			{
				FFirestoreDocumentSnapshot Snapshot;
				Snapshot.Snapshot = MoveTemp(Snap);
				Callback.ExecuteIfBound(Error, Snapshot);
				delete Copy;
			});
		}
	});
#else
	Callback.ExecuteIfBound(EFirestoreError::Unavailable, {});
#endif // WITH_FIREBASE_FIRESTORE 
}

void UFirestoreDocumentReference::Get(FDocumentSnapshotCallback Callback) const
{
	Get(EFirestoreSource::Default, MoveTemp(Callback));
}

#define CreateVoidCallback(ErrorMessage)																\
	[Callback = MoveTemp(Callback)](const firebase::Future<void>& Future) mutable -> void				\
	{																									\
		const EFirestoreError Error = (EFirestoreError)Future.error();									\
		if (Error != EFirestoreError::Ok)																\
		{																								\
			UE_LOG(LogFirestore, Error, TEXT(ErrorMessage) TEXT(" Code: %d. Message: %s"),				\
				Error, UTF8_TO_TCHAR(Future.error_message()));											\
		}																								\
																										\
		if (Callback.IsBound())																			\
		{																								\
			AsyncTask(ENamedThreads::GameThread, [Error, Callback = MoveTemp(Callback)]() -> void		\
			{																							\
				Callback.ExecuteIfBound(Error);															\
			});																							\
		}																								\
	}

void UFirestoreDocumentReference::Set(const TMap<FString, FFirestoreFieldValue>& Data,
	const FFirestoreSetOptions& options, FFirestoreCallback Callback)
{
#if WITH_FIREBASE_FIRESTORE 
	std::unordered_map<std::string, firebase::firestore::FieldValue> RawData;

	RawData.reserve(Data.Num());

	for (const auto& DataElem : Data)
	{
		RawData.emplace(TCHAR_TO_UTF8(*DataElem.Key), DataElem.Value);
	}

	Reference->Set(RawData, options).OnCompletion(CreateVoidCallback("Failed to set document Reference->"));
#endif // WITH_FIREBASE_FIRESTORE 
}

void UFirestoreDocumentReference::Set(const TMap<FString, FFirestoreFieldValue>& Data, FFirestoreCallback Callback)
{
	Set(Data, {}, MoveTemp(Callback));
}

void UFirestoreDocumentReference::Update(const TMap<FString, FFirestoreFieldValue>& Data, FFirestoreCallback Callback)
{
#if WITH_FIREBASE_FIRESTORE 
	std::unordered_map<std::string, firebase::firestore::FieldValue> RawData;

	RawData.reserve(Data.Num());

	for (const auto& DataElem : Data)
	{
		RawData.emplace(TCHAR_TO_UTF8(*DataElem.Key), DataElem.Value);
	}

	Reference->Update(RawData).OnCompletion(CreateVoidCallback("Failed to update document Reference->"));
#endif // WITH_FIREBASE_FIRESTORE 
}

void UFirestoreDocumentReference::Update(const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data, FFirestoreCallback Callback)
{
#if WITH_FIREBASE_FIRESTORE 
	std::unordered_map<firebase::firestore::FieldPath, firebase::firestore::FieldValue> RawData;

	RawData.reserve(Data.Num());

	for (const auto& DataElem : Data)
	{
		RawData.emplace(DataElem.Key, DataElem.Value);
	}

	Reference->Update(RawData).OnCompletion(CreateVoidCallback("Failed to updaet document Reference->"));
#endif // WITH_FIREBASE_FIRESTORE 
}

void UFirestoreDocumentReference::Delete(FFirestoreCallback Callback)
{
#if WITH_FIREBASE_FIRESTORE 
	Reference->Delete().OnCompletion(CreateVoidCallback("Failed to delete document Reference->"));
#endif // WITH_FIREBASE_FIRESTORE 
}

bool UFirestoreDocumentReference::IsValid() const
{
#if WITH_FIREBASE_FIRESTORE 
	return Reference->is_valid();
#else
	return false;
#endif
}

FString UFirestoreDocumentReference::ToString() const
{
#if WITH_FIREBASE_FIRESTORE 
	return UTF8_TO_TCHAR(Reference->ToString().c_str());
#else
	return TEXT("FIRESTORE_DISABLED");
#endif
}

#undef CreateVoidCallback

void UFirestoreDocumentReference::AddSnapshotListener(FDocumentSnapshotListener Callback)
{
#if !PLATFORM_LINUX && WITH_FIREBASE_FIRESTORE && defined(FIREBASE_USE_STD_FUNCTION) && !PLATFORM_IOS
	Reference->AddSnapshotListener([Callback = MoveTemp(Callback)]
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
	(const firebase::firestore::DocumentSnapshot& Snapshot, firebase::firestore::Error NativeError) mutable -> void
#else
	(const firebase::firestore::DocumentSnapshot& Snapshot, firebase::firestore::Error NativeError, const std::string&) mutable -> void
#endif
	{
		if (Callback.IsBound())
		{
			const EFirestoreError Error = (EFirestoreError)NativeError;
			AsyncTask(ENamedThreads::GameThread, [Snapshot, Callback = MoveTemp(Callback), Error]() -> void
			{
				Callback.ExecuteIfBound(Error, Snapshot);
			});
		}
	});
#else // not LINUX || not WITH_FIREBASE_FIRESTORE || not defined(FIREBASE_USE_STD_FUNCTION) || PLATFORM_IOS
	UE_LOG(LogFirestore, Warning, TEXT("Function AddSnapshotListener is unavailable for this platform."));
#endif
}

void UFirestoreDocumentReference::AddSnapshotListener(FDocumentSnapshotListenerCallback Callback)
{
#if !PLATFORM_LINUX && WITH_FIREBASE_FIRESTORE && defined(FIREBASE_USE_STD_FUNCTION) && !PLATFORM_IOS
	Reference->AddSnapshotListener([Callback = MoveTemp(Callback)]
#if FIREBASE_SDK_SMALLER_THAN(8, 9, 0)
		(const firebase::firestore::DocumentSnapshot & Snapshot, firebase::firestore::Error NativeError) mutable -> void
#else
		(const firebase::firestore::DocumentSnapshot & Snapshot, firebase::firestore::Error NativeError, const std::string&) mutable -> void
#endif
	{
		if (Callback.IsBound())
		{
			const EFirestoreError Error = (EFirestoreError)NativeError;
			AsyncTask(ENamedThreads::GameThread, [Snapshot, Callback = MoveTemp(Callback), Error]() -> void
			{
				Callback.ExecuteIfBound(Error, Snapshot);
			});
		}
	});
#else // not LINUX || not WITH_FIREBASE_FIRESTORE || not defined(FIREBASE_USE_STD_FUNCTION) || PLATFORM_IOS
	UE_LOG(LogFirestore, Warning, TEXT("Function AddSnapshotListener is unavailable for this platform."));
#endif
}

firebase::firestore::DocumentReference* UFirestoreDocumentReference::GetInternal() const
{
#if WITH_FIREBASE_FIRESTORE
	return Reference.Get();
#else
	return nullptr;
#endif
}

