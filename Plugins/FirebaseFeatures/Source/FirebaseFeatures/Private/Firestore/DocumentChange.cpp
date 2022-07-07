// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Firestore/DocumentChange.h"

#if WITH_FIREBASE_FIRESTORE
#	include "firebase/firestore/document_change.h"
#endif


UFirestoreDocumentChange::UFirestoreDocumentChange()
#if WITH_FIREBASE_FIRESTORE
	: Internal(MakeUnique<firebase::firestore::DocumentChange>())
#endif
{
}

UFirestoreDocumentChange::UFirestoreDocumentChange(FVTableHelper& Helper) 
	: Super(Helper)
{
}

UFirestoreDocumentChange::~UFirestoreDocumentChange() = default;

EDocumentChangeType UFirestoreDocumentChange::GetType() const
{
#if WITH_FIREBASE_FIRESTORE
	return (EDocumentChangeType)Internal->type();
#else
	return EDocumentChangeType::Added;
#endif
}

FFirestoreDocumentSnapshot UFirestoreDocumentChange::GetDocument() const
{
#if WITH_FIREBASE_FIRESTORE
	return Internal->document();
#else
	return {};
#endif
}

int64 UFirestoreDocumentChange::GetOldIndex() const
{
#if WITH_FIREBASE_FIRESTORE
	return Internal->old_index();
#else
	return -1;
#endif
}

int64 UFirestoreDocumentChange::GetNewIndex() const
{
#if WITH_FIREBASE_FIRESTORE
	return Internal->new_index();
#else
	return -1;
#endif
}




