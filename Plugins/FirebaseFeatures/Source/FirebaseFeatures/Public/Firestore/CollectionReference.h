// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "FirebaseSdk/FirebaseErrors.h"
#include "Firestore/FieldValue.h"
#include "Firestore/Query.h"
#include "CollectionReference.generated.h"

#if !defined(WITH_FIREBASE_FIRESTORE)
#	define WITH_FIREBASE_FIRESTORE 0
#endif

namespace firebase { namespace firestore { class Query; class CollectionReference; } }

class UFirestoreDocumentReference;

DECLARE_DELEGATE_TwoParams(FFirestoreDocumentCallback, const EFirestoreError, UFirestoreDocumentReference*);

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UFirestoreCollectionReference : public UFirestoreQuery
{
	GENERATED_BODY()
private:
	friend class UFirestore;
	friend class UFirestoreDocumentReference;

public:
	UFirestoreCollectionReference();

	/**
	 * @brief Gets the ID of the referenced collection.
	 *
	 * @return The ID as a FString.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Collection")
	UPARAM(DisplayName = "ID") FString GetId() const;

	/**
	 * @brief Returns the path of this collection (relative to the root of the
	 * database) as a slash-separated string.
	 *
	 * @return The path as a FString.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Collection")
	UPARAM(DisplayName = "Path") FString GetPath() const;

	/**
	 * @brief Gets a DocumentReference to the document that contains this
	 * collection.
	 *
	 * @return The DocumentReference that contains this collection if this is a
	 * subcollection. If this is a root collection, returns an invalid
	 * DocumentReference (`DocumentReference::is_valid()` will return false).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Collection")
	UPARAM(DisplayName = "Document") UFirestoreDocumentReference* GetParent() const;

	/**
	 * @brief Returns a DocumentReference that points to a new document with an
	 * auto-generated ID within this collection.
	 *
	 * @return A DocumentReference pointing to the new document.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Collection")
	UPARAM(DisplayName = "Document") UFirestoreDocumentReference* GetDocument() const;

	/**
	 * @brief Gets a DocumentReference instance that refers to the document at the
	 * specified path within this collection.
	 *
	 * @param[in] DocumentPath A slash-separated relative path to a document.
	 *
	 * @return The DocumentReference instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Collection")
	UPARAM(DisplayName = "Document") UFirestoreDocumentReference* GetDocumentFromPath(const FString& DocumentPath) const;

	/**
	 * @brief Adds a new document to this collection with the specified data,
	 * assigning it a document ID automatically.
	 * @param data A map containing the data for the new document.
	 * @param Callback A callback that will be resolved with the DocumentReference of the
	 * newly created document.
	 */
	void Add(const TMap<FString, FFirestoreFieldValue> & Data, const FFirestoreDocumentCallback& Callback = FFirestoreDocumentCallback());

private:
#if WITH_FIREBASE_FIRESTORE
	firebase::firestore::CollectionReference* GetQuery();
	const firebase::firestore::CollectionReference* GetQuery() const;
#endif // WITH_FIREBASE_FIRESTORE
};


