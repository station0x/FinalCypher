// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "DocumentSnapshot.h"
#include "DocumentChange.generated.h"

#ifndef WITH_FIREBASE_FIRESTORE
#define WITH_FIREBASE_FIRESTORE 0
#endif

#if WITH_FIREBASE_FIRESTORE
namespace firebase { namespace firestore { class DocumentChange; } }
#endif

/**
 * An enumeration of snapshot diff types.
 */
UENUM(BlueprintType)
enum class EDocumentChangeType : uint8
{
    /**
     * Indicates a new document was added to the set of documents matching the
     * query.
     */
    Added,

    /**
     * Indicates a document within the query was modified.
     */
     Modified,

     /**
      * Indicates a document within the query was removed (either deleted or no
      * longer matches the query).
      */
      Removed,
};

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UFirestoreDocumentChange : public UObject
{
    GENERATED_BODY()
public:
    UFirestoreDocumentChange();
    UFirestoreDocumentChange(FVTableHelper& Helper);
    ~UFirestoreDocumentChange();

    /**
     * Returns the type of change that occurred (added, modified, or removed).
     * @return The type of change that occurred (added, modified, or removed).
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentChange")
    UPARAM(DisplayName = "Type") EDocumentChangeType GetType() const;

    /**
     * The document affected by this change.
     *
     * @return The newly added or modified document if this DocumentChange is for
     * an updated document. Returns the deleted document if this document change
     * represents a removal.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentChange")
    UPARAM(DisplayName = "Document") FFirestoreDocumentSnapshot GetDocument() const;

    /**
     * The index of the changed document in the result set immediately prior to
     * this DocumentChange (that is, supposing that all prior DocumentChange
     * objects have been applied). Returns DocumentChange::npos for 'added'
     * events.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentChange")
    UPARAM(DisplayName = "Old Index") int64 GetOldIndex() const;


    /**
     * The index of the changed document in the result set immediately after this
     * DocumentChange (that is, supposing that all prior DocumentChange objects
     * and the current DocumentChange object have been applied). Returns
     * DocumentChange::npos for 'removed' events.
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentChange")
    UPARAM(DisplayName = "New Index") int64 GetNewIndex() const;

private:
    friend class UFirestoreQuery;
#if WITH_FIREBASE_FIRESTORE
    TUniquePtr<firebase::firestore::DocumentChange> Internal;
#endif
};