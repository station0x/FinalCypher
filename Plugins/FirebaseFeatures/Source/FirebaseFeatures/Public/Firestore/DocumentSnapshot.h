// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if !defined(WITH_FIREBASE_FIRESTORE)
#	define WITH_FIREBASE_FIRESTORE 0
#endif 

#if WITH_FIREBASE_FIRESTORE
    THIRD_PARTY_INCLUDES_START
#       include "firebase/firestore/document_snapshot.h"
    THIRD_PARTY_INCLUDES_END
#endif // WITH_FIREBASE_FIRESTORE

#include "Firestore/FieldValue.h"
#include "Firestore/FieldPath.h"
#include "DocumentSnapshot.generated.h"

class UFirestoreDocumentReference;

UENUM(BlueprintType)
enum class EFirestoreServerTimestampBehavior : uint8
{
    /**
     * Return Null for server timestamps that have not yet been set to their
     * final value.
     * Use Default for this value.
     */
    // None = 0,

    /**
     * Return local estimates for server timestamps that have not yet been set
     * to their final value. This estimate will likely differ from the final
     * value and may cause these pending values to change once the server result
     * becomes available.
     */
    Estimate = 1,

     /**
      * Return the previous value for server timestamps that have not yet been
      * set to their final value.
      */
    Previous,

      /** The default behavior, which is equivalent to specifying None. */
    Default = 0,
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreSnapshotMetadata
{
    GENERATED_BODY()
public:
    /**
     * Whether the snapshot contains the result of local writes.
     * Is true if the snapshot contains the result of local writes (for
     * example, Set() or Update() calls) that have not yet been committed to the
     * backend. 
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Firestore|DocumentSnapshot")
    bool bHasPendingWrites = false;

    /**
     * Whether the snapshot was created from cached data.
     * Is true if the snapshot was created from cached data rather than
     * guaranteed up-to-date server data. 
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Firestore|DocumentSnapshot")
    bool bIsFromCache = false;
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreDocumentSnapshot
{
	GENERATED_BODY()
private:
    friend class UFirestoreDocumentReference;
    friend class UFirestoreQuery;
    friend class UFirestoreDocumentChange;
    friend struct FFirestoreTransaction;

#if WITH_FIREBASE_FIRESTORE
    FFirestoreDocumentSnapshot(const firebase::firestore::DocumentSnapshot& InSnapshot);
#endif

public:
    FFirestoreDocumentSnapshot() {};
    FFirestoreDocumentSnapshot(FFirestoreDocumentSnapshot&&);
    FFirestoreDocumentSnapshot(const FFirestoreDocumentSnapshot&);

    FFirestoreDocumentSnapshot& operator=(const FFirestoreDocumentSnapshot&);
    FFirestoreDocumentSnapshot& operator=(FFirestoreDocumentSnapshot&&);

    /**
     * @brief Returns the string ID of the document for which this
     * DocumentSnapshot contains data.
     *
     * @return String ID of this document location.
    */
    FString GetId() const;

    /**
    * @brief Returns the document location for which this DocumentSnapshot
    * contains data.
    *
    * @return DocumentReference of this document location.
    */
    UFirestoreDocumentReference* GetReference() const;

    /**
     * @brief Returns the metadata about this snapshot concerning its source and
     * if it has local modifications.
     *
     * @return SnapshotMetadata about this snapshot.
     */
    FFirestoreSnapshotMetadata GetMetadata() const;

    /**
     * @brief Explicitly verify a document's existence.
     *
     * @return True if the document exists in this snapshot.
     */
    bool Exists() const;

    /**
     * @brief Retrieves all fields in the document as a map of FieldValues.
     *
     * @param stb Configures how server timestamps that have not yet
     * been set to their final value are returned from the snapshot (optional).
     *
     * @return A map containing all fields in the document, or an empty map if the
     * document doesn't exist.
     */
    TMap<FString, FFirestoreFieldValue> GetData(
        EFirestoreServerTimestampBehavior ServerTimestampBehavior = EFirestoreServerTimestampBehavior::Default) const;

    /**
     * @brief Retrieves a specific field from the document.
     *
     * @param field String ID of the field to retrieve.
     * @param stb Configures how server timestamps that have not yet been set to
     * their final value are returned from the snapshot (optional).
     *
     * @return The value contained in the field. If the field does not exist in
     * the document, then a `FieldValue` instance with `is_valid() == false` will
     * be returned.
     */
    FFirestoreFieldValue Get(
        const FString& Field,
        EFirestoreServerTimestampBehavior ServerTimestampBehavior = EFirestoreServerTimestampBehavior::Default) const;

    /**
     * @brief Retrieves a specific field from the document.
     *
     * @param field Path of the field to retrieve.
     * @param stb Configures how server timestamps that have not yet been set to
     * their final value are returned from the snapshot (optional).
     *
     * @return The value contained in the field. If the field does not exist in
     * the document, then a `FieldValue` instance with `is_valid() == false` will
     * be returned.
     */
    FFirestoreFieldValue Get(
        const FFirestoreFieldPath& Field,
        EFirestoreServerTimestampBehavior ServerTimestampBehavior = EFirestoreServerTimestampBehavior::Default) const;

    /**
     * Returns a string representation of this `DocumentSnapshot` for
     * logging/debugging purposes.
     *
     * @note the exact string representation is unspecified and subject to
     * change; don't rely on the format of the string.
     */
    FString ToString() const;


 #if WITH_FIREBASE_FIRESTORE
    FORCEINLINE operator firebase::firestore::DocumentSnapshot& ()
    {
        return Snapshot;
    }

    FORCEINLINE operator const firebase::firestore::DocumentSnapshot& () const
    {
        return Snapshot;
    }

private:
    firebase::firestore::DocumentSnapshot Snapshot;
#endif // WITH_FIREBASE_FIRESTORE

};
