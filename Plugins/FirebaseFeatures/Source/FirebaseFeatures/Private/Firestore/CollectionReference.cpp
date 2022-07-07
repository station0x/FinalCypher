// Copyright Pandores Marketplace 2022. All Rights Reserved.
#include "Firestore/CollectionReference.h"

THIRD_PARTY_INCLUDES_START
#	include "firebase/future.h"
#	if WITH_FIREBASE_FIRESTORE
#		include "firebase/firestore/query.h"
#		include "firebase/firestore/collection_reference.h"
#		include "firebase/firestore/document_reference.h"
#	endif // WITH_FIREBASE_FIRESTORE
THIRD_PARTY_INCLUDES_END

#include "Firestore/DocumentReference.h"

#include "Async/Async.h"

#if WITH_FIREBASE_FIRESTORE
firebase::firestore::CollectionReference* UFirestoreCollectionReference::GetQuery()
{
	return static_cast<firebase::firestore::CollectionReference*>(Reference.Get());
}
#endif // WITH_FIREBASE_FIRESTORE

#if WITH_FIREBASE_FIRESTORE
const firebase::firestore::CollectionReference* UFirestoreCollectionReference::GetQuery() const
{
	return static_cast<firebase::firestore::CollectionReference*>(Reference.Get());
}
#endif // WITH_FIREBASE_FIRESTORE

UFirestoreCollectionReference::UFirestoreCollectionReference()
{
#if WITH_FIREBASE_FIRESTORE
	Reference.Reset(new firebase::firestore::CollectionReference());
#endif // WITH_FIREBASE_FIRESTORE
}

FString UFirestoreCollectionReference::GetId() const
{
#if WITH_FIREBASE_FIRESTORE
	return UTF8_TO_TCHAR(GetQuery()->id().c_str());
#else 
	return TEXT("FIRESTORE_DISABLED");
#endif
}

FString UFirestoreCollectionReference::GetPath() const
{
#if WITH_FIREBASE_FIRESTORE
	return UTF8_TO_TCHAR(GetQuery()->path().c_str());
#else 
	return TEXT("FIRESTORE_DISABLED");
#endif
}

UFirestoreDocumentReference* UFirestoreCollectionReference::GetParent() const
{
	UFirestoreDocumentReference* const Document = NewObject<UFirestoreDocumentReference>();

#if WITH_FIREBASE_FIRESTORE
	*Document->Reference = GetQuery()->Parent();
#endif

	return Document;
}

UFirestoreDocumentReference* UFirestoreCollectionReference::GetDocument() const
{
	UFirestoreDocumentReference* const Document = NewObject<UFirestoreDocumentReference>();

#if WITH_FIREBASE_FIRESTORE
	*Document->Reference = GetQuery()->Document();
#endif

	return Document;
}

UFirestoreDocumentReference* UFirestoreCollectionReference::GetDocumentFromPath(const FString& DocumentPath) const
{
	UFirestoreDocumentReference* const Document = NewObject<UFirestoreDocumentReference>();

#if WITH_FIREBASE_FIRESTORE
	if (!DocumentPath.IsEmpty())
	{
		*Document->Reference = GetQuery()->Document(TCHAR_TO_UTF8(*DocumentPath));
	}
#endif

	return Document;
}

void UFirestoreCollectionReference::Add(const TMap<FString, FFirestoreFieldValue>& Data, const FFirestoreDocumentCallback& Callback)
{
#if WITH_FIREBASE_FIRESTORE
	std::unordered_map<std::string, firebase::firestore::FieldValue> RawData;

	for (const auto& DataElem : Data)
	{
		RawData.emplace(TCHAR_TO_UTF8(*DataElem.Key), DataElem.Value);
	}

	GetQuery()->Add(MoveTemp(RawData)).OnCompletion([Callback](const firebase::Future<firebase::firestore::DocumentReference>& Future) -> void
	{
		const EFirestoreError Error = (EFirestoreError)Future.error();
		if (Error != EFirestoreError::Ok)
		{
			UE_LOG(LogFirestore, Error, TEXT("Failed to add data to collection. Code: %d. Message: %s"),
				Error, UTF8_TO_TCHAR(Future.error_message()));
		}

		if (Callback.IsBound())
		{
			firebase::firestore::DocumentReference Reference = Future.result() ? *Future.result() : firebase::firestore::DocumentReference();
			AsyncTask(ENamedThreads::GameThread, [Callback, Error, Reference]() -> void
			{
				UFirestoreDocumentReference* const Document = NewObject<UFirestoreDocumentReference>();

				*Document->Reference = Reference;

				Callback.ExecuteIfBound(Error, Document);
			});
		}
	});
#endif
}


