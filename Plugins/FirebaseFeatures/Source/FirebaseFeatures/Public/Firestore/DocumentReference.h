// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Firestore.h"
#include "FirebaseFeatures.h"
#include "Firestore/DocumentSnapshot.h"
#include "DocumentReference.generated.h"

class UFirestoreCollectionReference;

namespace firebase { namespace firestore { class DocumentReference; } }

DECLARE_DELEGATE_TwoParams(FDocumentSnapshotCallback, const EFirestoreError, const FFirestoreDocumentSnapshot&);

DECLARE_DYNAMIC_DELEGATE_TwoParams(FDocumentSnapshotListener, const EFirestoreError, Error, const FFirestoreDocumentSnapshot&, DocumentSnapshot);
DECLARE_DELEGATE_TwoParams(FDocumentSnapshotListenerCallback, const EFirestoreError, const FFirestoreDocumentSnapshot&);

/**
 *	A Firestore Document Reference.
 **/
UCLASS(BlueprintType)
class FIREBASEFEATURES_API UFirestoreDocumentReference : public UObject
{
	GENERATED_BODY()
private:
	friend class UFirestore;
	friend class UFirestoreCollectionReference;
	friend struct FFirestoreDocumentSnapshot;
	friend struct FFirestoreFieldValue;

public:
	UFirestoreDocumentReference(FVTableHelper& Helper);
	UFirestoreDocumentReference();
	~UFirestoreDocumentReference();

public:

	/**
	 * @brief Returns the string ID of this document location.
	 *
	 * @return String ID of this document location.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentReference")
	UPARAM(DisplayName = "ID") FString GetId() const;

	/**
	 * @brief Returns the path of this document (relative to the root of the
	 * database) as a slash-separated string.
	 *
	 * @return String path of this document location.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentReference")
	UPARAM(DisplayName = "Path") FString GetPath() const;

	/**
	 * @brief Returns a CollectionReference to the collection that contains this
	 * document.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentReference")
	UPARAM(DisplayName = "Collection") UFirestoreCollectionReference* GetParent() const;

	/**
	 * @brief Returns a CollectionReference instance that refers to the
	 * subcollection at the specified path relative to this document.
	 *
	 * @param[in] collection_path A slash-separated relative path to a
	 * subcollection. 
	 *
	 * @return The CollectionReference instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentReference")
	UPARAM(DisplayName = "Collection") UFirestoreCollectionReference* GetCollection(const FString& CollectionPath) const;

	/**
	 * @brief Reads the document referenced by this DocumentReference.
	 *
	 * By default, Get() attempts to provide up-to-date data when possible by
	 * waiting for data from the server, but it may return cached data or fail if
	 * you are offline and the server cannot be reached. This behavior can be
	 * altered via the Source parameter.
	 *
	 * @param Source A value to configure the get behavior.
	 *
	 * @param Callback A Callback that will be resolved with the contents of the Document at
	 * this DocumentReference.
	 */
	void Get(EFirestoreSource Source, FDocumentSnapshotCallback Callback) const;
	void Get(FDocumentSnapshotCallback Callback) const;

	/**
	 * @brief Writes to the document referred to by this DocumentReference.
	 *
	 * If the document does not yet exist, it will be created. If you pass
	 * SetOptions, the provided data can be merged into an existing document.
	 *
	 * @param[in] data A map of the fields and values to write to the document.
	 * @param[in] options An object to configure the Set() behavior (optional).
	 *
	 * @return A Callback that will be resolved when the write finishes.
	 */
	void Set(const TMap<FString, FFirestoreFieldValue>& Data,
		const FFirestoreSetOptions& Options = FFirestoreSetOptions(), FFirestoreCallback Callback = FFirestoreCallback());
	void Set(const TMap<FString, FFirestoreFieldValue>& Data, FFirestoreCallback Callback = FFirestoreCallback());

	/**
	 * @brief Updates fields in the document referred to by this
	 * DocumentReference.
	 *
	 * If no document exists yet, the update will fail.
	 *
	 * @param[in] data A map of field / value pairs to update. Fields can contain
	 * dots to reference nested fields within the document.
	 *	
	 * @param Callback A Callback that will be resolved when the client is online and the
	 * commit has completed against the server. The Callback will not resolve when
	 * the device is offline, though local changes will be visible immediately.
	 */
	void Update(const TMap<FString, FFirestoreFieldValue>& Data, FFirestoreCallback Callback = FFirestoreCallback());

	/**
	 * @brief Updates fields in the document referred to by this
	 * DocumentReference.
	 *
	 * If no document exists yet, the update will fail.
	 *
	 * @param[in] data A map from FieldPath to FieldValue to update.
	 *
	 * @return A Callback that will be resolved when the client is online and the
	 * commit has completed against the server. The Callback will not resolve when
	 * the device is offline, though local changes will be visible immediately.
	 */
	void Update(const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data, FFirestoreCallback Callback = FFirestoreCallback());

	/**
	 * @brief Removes the document referred to by this DocumentReference.
	 */
	void Delete(FFirestoreCallback Callback = FFirestoreCallback());

	/**
	 * @brief Returns true if this DocumentReference is valid, false if it is not
	 * valid. An invalid DocumentReference could be the result of:
	 *   - Creating a DocumentReference with the default constructor.
	 *   - Calling CollectionReference::Parent() on a CollectionReference that is
	 *     not a subcollection.
	 *   - Deleting your Firestore instance, which will invalidate all
	 *     DocumentReference instances associated with it.
	 *
	 * @return true if this DocumentReference is valid, false if this
	 * DocumentReference is invalid.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentReference")
	UPARAM(DisplayName = "Is Valid") bool IsValid() const;

	/**
	 * Returns a string representation of this `DocumentReference` for
	 * logging/debugging purposes.
	 *
	 * @note the exact string representation is unspecified and subject to
	 * change; don't rely on the format of the string.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentReference")
	UPARAM(DisplayName = "String") FString ToString() const;

	/**
     * Starts listening to the document referenced by this
     * DocumentReference.
     *
     * @param callback The callback to call. When this function is
     * called, snapshot value is valid if and only if error is Ok.
     *
     * @note This method is not available when using the STLPort C++ runtime
     * library.
     */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|DocumentReference", meta = (Keywords = "listener add setup snapshot"))
	void AddSnapshotListener(UPARAM(DisplayName = "Listener") FDocumentSnapshotListener Callback);
	void AddSnapshotListener(FDocumentSnapshotListenerCallback Callback);

	firebase::firestore::DocumentReference* GetInternal() const;

private:
	TUniquePtr<firebase::firestore::DocumentReference> Reference;
};


