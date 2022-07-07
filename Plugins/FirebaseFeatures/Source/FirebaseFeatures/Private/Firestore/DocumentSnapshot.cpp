// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "Firestore/DocumentSnapshot.h"
#include "Firestore/DocumentReference.h"

#if WITH_FIREBASE_FIRESTORE
THIRD_PARTY_INCLUDES_START
#	include "firebase/firestore/document_reference.h"
THIRD_PARTY_INCLUDES_END
#endif // WITH_FIREBASE_FIRESTORE

#if WITH_FIREBASE_FIRESTORE
FFirestoreDocumentSnapshot::FFirestoreDocumentSnapshot(const firebase::firestore::DocumentSnapshot& InSnapshot)
    : Snapshot(InSnapshot)
{}
#endif

FString FFirestoreDocumentSnapshot::GetId() const
{
#if WITH_FIREBASE_FIRESTORE
    return UTF8_TO_TCHAR(Snapshot.id().c_str());
#else
    return TEXT("FIRESTORE_DISABLED");
#endif
}

UFirestoreDocumentReference* FFirestoreDocumentSnapshot::GetReference() const
{
    UFirestoreDocumentReference* const Ref = NewObject<UFirestoreDocumentReference>();
#if WITH_FIREBASE_FIRESTORE
    *Ref->Reference = Snapshot.reference();
#endif
    return Ref;
}

FFirestoreSnapshotMetadata FFirestoreDocumentSnapshot::GetMetadata() const
{
    FFirestoreSnapshotMetadata Metadata;
#if WITH_FIREBASE_FIRESTORE
    const auto& RawMetadata = Snapshot.metadata();

    Metadata.bHasPendingWrites  = RawMetadata.has_pending_writes();
    Metadata.bIsFromCache       = RawMetadata.is_from_cache();
#endif
    return Metadata;
}

bool FFirestoreDocumentSnapshot::Exists() const
{
#if WITH_FIREBASE_FIRESTORE
    return Snapshot.exists();
#else
    return false;
#endif
}

TMap<FString, FFirestoreFieldValue> FFirestoreDocumentSnapshot::GetData(
    EFirestoreServerTimestampBehavior ServerTimestampBehavior) const
{
    TMap<FString, FFirestoreFieldValue> Values;

#if WITH_FIREBASE_FIRESTORE
    const std::unordered_map<std::string, firebase::firestore::FieldValue>& RawValues =
        Snapshot.GetData(
            (firebase::firestore::DocumentSnapshot::ServerTimestampBehavior)
            ServerTimestampBehavior);


    Values.Reserve(RawValues.size());

    for (const auto& Value : RawValues)
    {
        Values.Add(UTF8_TO_TCHAR(Value.first.c_str()), Value.second);
    }
#endif

    return Values;
}

FFirestoreFieldValue FFirestoreDocumentSnapshot::Get(
    const FString& Field,
    EFirestoreServerTimestampBehavior ServerTimestampBehavior) const
{
#if WITH_FIREBASE_FIRESTORE
    return Snapshot.Get(TCHAR_TO_UTF8(*Field), (firebase::firestore::DocumentSnapshot::ServerTimestampBehavior)ServerTimestampBehavior);
#else
    return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreDocumentSnapshot::Get(
    const FFirestoreFieldPath& Field,
    EFirestoreServerTimestampBehavior ServerTimestampBehavior) const
{
#if WITH_FIREBASE_FIRESTORE
    return Snapshot.Get(Field, (firebase::firestore::DocumentSnapshot::ServerTimestampBehavior)ServerTimestampBehavior);
#else
    return FFirestoreFieldValue();
#endif
}

FString FFirestoreDocumentSnapshot::ToString() const
{
#if WITH_FIREBASE_FIRESTORE
    return UTF8_TO_TCHAR(Snapshot.ToString().c_str());
#else 
    return TEXT("FIRESTORE_DISABLED");
#endif
}

FFirestoreDocumentSnapshot::FFirestoreDocumentSnapshot(FFirestoreDocumentSnapshot&& Other)
#if WITH_FIREBASE_FIRESTORE
    : Snapshot(MoveTemp(Other.Snapshot))
#endif
{
}

FFirestoreDocumentSnapshot::FFirestoreDocumentSnapshot(const FFirestoreDocumentSnapshot& Other)
#if WITH_FIREBASE_FIRESTORE
    : Snapshot(Other.Snapshot)
#endif
{
}

FFirestoreDocumentSnapshot& FFirestoreDocumentSnapshot::operator=(const FFirestoreDocumentSnapshot& Other)
{
#if WITH_FIREBASE_FIRESTORE
    Snapshot = Other.Snapshot;
#endif
    return *this;
}

FFirestoreDocumentSnapshot& FFirestoreDocumentSnapshot::operator=(FFirestoreDocumentSnapshot&& Other)
{
#if WITH_FIREBASE_FIRESTORE
    Snapshot = MoveTemp(Other.Snapshot);
#endif
    return *this;
}

