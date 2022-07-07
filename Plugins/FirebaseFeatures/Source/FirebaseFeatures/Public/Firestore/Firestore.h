// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Firestore/DocumentSnapshot.h"
#include "Firestore/FieldPath.h"
#include "Firestore/FieldValue.h"
#include "FirebaseFeatures.h"
#include "FirebaseSdk/FirebaseErrors.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Firestore.generated.h"

#if !defined(WITH_FIREBASE_FIRESTORE)
#	define WITH_FIREBASE_FIRESTORE 0
#endif 

DECLARE_DELEGATE_OneParam(FFirestoreCallback, const EFirestoreError);

namespace firebase { namespace firestore { 
	class Firestore; 
	class WriteBatch; 
	class FieldValue; 
	class SetOptions;
	class FieldPath;
	class Transaction;
}; };

class UFirestoreDocumentReference;
class UFirestoreCollectionReference;

UENUM(BlueprintType)
enum class EFirestoreSetOptionsType : uint8
{
	/** Overwrites the whole document. */
	Overwrite,

	/**
	 * Replaces the values specified in the call parameter while leaves omitted
	 * fields untouched.
	 */
	MergeAll,

	/**
	 * Replaces the values of the fields explicitly specified in the call
	 * parameter.
	 */
	MergeSpecific,
};


/**
 * @brief Configures the behavior of DocumentReference::Get() and Query::Get().
 *
 * By providing a Source value, these methods can be configured to fetch results
 * only from the server, only from the local cache, or attempt to fetch results
 * from the server and fall back to the cache (which is the default).
 */
UENUM(BlueprintType)
enum class EFirestoreSource : uint8
{
	/**
	 * Causes Firestore to try to retrieve an up-to-date (server-retrieved)
	 * snapshot, but fall back to returning cached data if the server can't be
	 * reached.
	 */
	Default,

	/**
	 * Causes Firestore to avoid the cache, generating an error if the server
	 * cannot be reached. Note that the cache will still be updated if the server
	 * request succeeds. Also note that latency-compensation still takes effect,
	 * so any pending write operations will be visible in the returned data
	 * (merged into the server-provided data).
	 */
	 Server,

	 /**
	  * Causes Firestore to immediately return a value from the cache, ignoring the
	  * server completely (implying that the returned value may be stale with
	  * respect to the value on the server). If there is no data in the cache to
	  * satisfy the DocumentReference::Get() call will return an error and
	  * Query::Get() will return an empty QuerySnapshot with no documents.
	  */
	  Cache,
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreSettings
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Firestore")
	FString Host;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Firestore")
	bool bSslEnabled = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Firebase|Firestore")
	bool bPersistenceEnabled = false;
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreSetOptions
{
	GENERATED_BODY()
public:
	FFirestoreSetOptions();
	FFirestoreSetOptions(const FFirestoreSetOptions& Other);
	FFirestoreSetOptions(FFirestoreSetOptions&& Other);

	~FFirestoreSetOptions();

	FFirestoreSetOptions& operator=(const FFirestoreSetOptions& Other);

	/**
	 * Returns an instance that can be used to change the behavior of Set() calls
	 * to only replace the values specified in its data argument. Fields omitted
	 * from the Set() call will remain untouched.
	 */
	static FFirestoreSetOptions Merge();

	/**
	 * Returns an instance that can be used to change the behavior of Set() calls
	 * to only replace the given fields. Any field that is not specified in
	 * `fields` is ignored and remains untouched.
	 *
	 * It is an error to pass a SetOptions object to a Set() call that is missing
	 * a value for any of the fields specified here.
	 *
	 * @param fields The list of fields to merge. Fields can contain dots to
	 * reference nested fields within the document.
	 */
	static FFirestoreSetOptions MergeFields(const TArray<FString>& Fields);

	/**
	 * Returns an instance that can be used to change the behavior of Set() calls
	 * to only replace the given fields. Any field that is not specified in
	 * `fields` is ignored and remains untouched.
	 *
	 * It is an error to pass a SetOptions object to a Set() call that is missing
	 * a value for any of the fields specified here in its to data argument.
	 *
	 * @param fields The list of fields to merge.
	 */
	static FFirestoreSetOptions MergeFieldPaths(const TArray<FFirestoreFieldPath>& Fields);


#if WITH_FIREBASE_FIRESTORE
	FORCEINLINE operator firebase::firestore::SetOptions&()
	{
		return *Options;
	}

	FORCEINLINE operator firebase::firestore::SetOptions&() const
	{
		return *Options;
	}
#endif

private:
#if WITH_FIREBASE_FIRESTORE
	TUniquePtr<firebase::firestore::SetOptions> Options;
#endif
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreTransaction
{
	GENERATED_BODY()
public:

	FFirestoreTransaction();
	FFirestoreTransaction(const FFirestoreTransaction&);
	~FFirestoreTransaction();

#if WITH_FIREBASE_FIRESTORE
	FFirestoreTransaction(firebase::firestore::Transaction* const InTransaction);
#endif

	/**
	 * @brief Writes to the document referred to by the provided reference.
	 *
	 * If the document does not yet exist, it will be created. If you pass
	 * SetOptions, the provided data can be merged into an existing document.
	 *
	 * @param[in] document The DocumentReference to overwrite.
	 * @param[in] Data A map of the fields and values to write to the document.
	 * @param[in] Options An object to configure the Set() behavior (optional).
	 */
	void Set(UFirestoreDocumentReference* const Document, const TMap<FString, FFirestoreFieldValue>& Data,
		const FFirestoreSetOptions& Options = FFirestoreSetOptions());

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param[in] document The DocumentReference to update.
	 * @param[in] data A map of field / value pairs to update. Fields can contain
	 * dots to reference nested fields within the document.
	 */
	void Update(UFirestoreDocumentReference* const Document,
		const TMap<FString, FFirestoreFieldValue>& Data);

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param[in] document The DocumentReference to update.
	 * @param[in] data A map from FieldPath to FieldValue to update.
	 */
	void Update(UFirestoreDocumentReference* const Document,
		const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data);

	/**
	 * Deletes the document referred to by the provided reference.
	 *
	 * @param[in] document The DocumentReference to delete.
	 */
	void Delete(UFirestoreDocumentReference* const Document);

	/**
	 * Reads the document referred by the provided reference.
	 *
	 * @param[in] document The DocumentReference to read.
	 * @param[out] ErrorCode An out parameter to capture an error, if one
	 * occurred.
	 * @param[out] ErrorMessage An out parameter to capture error message, if
	 * any.
	 * @return The contents of the document at this DocumentReference or invalid
	 * DocumentSnapshot if there is any error.
	 */
	FFirestoreDocumentSnapshot Get(UFirestoreDocumentReference* const Document,
		EFirestoreError& ErrorCode, FString& ErrorMessage);

private:
#if WITH_FIREBASE_FIRESTORE
	firebase::firestore::Transaction* Transaction;
#endif

};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FWriteBatch
{
	GENERATED_BODY()
public:
	FWriteBatch();
	~FWriteBatch();

	FWriteBatch& operator=(const FWriteBatch& Other);

	/**
	 * @brief Writes to the document referred to by the provided reference.
	 *
	 * If the document does not yet exist, it will be created. If you pass
	 * SetOptions, the provided data can be merged into an existing document.
	 *
	 * @param document The DocumentReference to write to.
	 * @param data A map of the fields and values to write to the document.
	 * @param[in] options An object to configure the Set() behavior (optional).
	 *
	 * @return This WriteBatch instance. Used for chaining method calls.
	 */
	FWriteBatch& Set(UFirestoreDocumentReference* const Document, 
		const TMap<FString, FFirestoreFieldValue>& Data, 
		const FFirestoreSetOptions& Options = FFirestoreSetOptions());

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param document The DocumentReference to update.
	 * @param data A map of field / value pairs to update. Fields can contain dots
	 * to reference nested fields within the document.
	 * @return This WriteBatch instance. Used for chaining method calls.
	 */
	FWriteBatch& Update(UFirestoreDocumentReference* const Document,
		const TMap<FString, FFirestoreFieldValue>& Data);

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param document The DocumentReference to update.
	 * @param data A map from FieldPath to FieldValue to update.
	 * @return This WriteBatch instance. Used for chaining method calls.
	 */
	FWriteBatch& Update(UFirestoreDocumentReference* const Document,
		const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data);

	/**
	 * Deletes the document referred to by the provided reference.
	 *
	 * @param document The DocumentReference to delete.
	 * @return This WriteBatch instance. Used for chaining method calls.
	 */
	FWriteBatch& Delete(UFirestoreDocumentReference* const Document);

	/**
	 * Commits all of the writes in this write batch as a single atomic unit.
	 */
	void Commit(const FFirestoreCallback& Callback);

private:
	TUniquePtr<firebase::firestore::WriteBatch> Batch;
};

DECLARE_DELEGATE_RetVal_TwoParams(EFirestoreError, FFirestoreTransactionCallback, const FFirestoreTransaction&, FString&);

UCLASS()
class FIREBASEFEATURES_API UFirestore : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * @brief Returns a CollectionReference instance that refers to the
	 * collection at the specified path within the database.
	 *
	 * @param CollectionPath A slash-separated path to a collection.
	 *
	 * @return The CollectionReference instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore")
	static UPARAM(DisplayName = "Collection") UFirestoreCollectionReference* GetCollection(const FString& CollectionPath);

	/**
	 * @brief Returns a DocumentReference instance that refers to the document at
	 * the specified path within the database.
	 *
	 * @param DocumentPath A slash-separated path to a document.
	 *
	 * @return The DocumentReference instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore")
	static UPARAM(DisplayName = "Document") UFirestoreDocumentReference* GetDocument(const FString& DocumentPath);


	/**
	 * @brief Returns a Query instance that includes all documents in the
	 * database that are contained in a collection or subcollection with the
	 * given CollectionId.
	 *
	 * @param CollectionId Identifies the collections to query over. Every
	 * collection or subcollection with this ID as the last segment of its path
	 * will be included. Cannot contain a slash.
	 *
	 * @return The Query instance.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore")
	static UPARAM(DisplayName = "Query") class UFirestoreQuery* CollectionGroup(const FString& CollectionId);

	/** Returns the settings used by this Firestore object. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore")
	static UPARAM(DisplayName = "Settings") FFirestoreSettings GetSettings();

	/** Sets any custom settings used to configure this Firestore object. */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore")
	static void SetSettings(const FFirestoreSettings& Settings);


	/**
	 * Executes the given update and then attempts to commit the changes applied
	 * within the transaction. If any document read within the transaction has
	 * changed, the update function will be retried. If it fails to commit after
	 * 5 attempts, the transaction will fail.
	 *
	 * @param update function or lambda to execute within the transaction context.
	 * The string reference parameter can be used to set the error message.
	 *
	 * @return A Future that will be resolved when the transaction finishes.
	 *
	 * @note This method is not available when using the STLPort C++ runtime
	 * library.
	 */
	static void RunTransaction(const FFirestoreTransactionCallback& Transaction, const FFirestoreCallback& Callback);

	/**
	 * Sets the log verbosity of all Firestore instances.
	 *
	 * The default verbosity level is `kLogLevelInfo`.
	 *
	 * @param[in] log_level The desired verbosity.
	 */
	//UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore")
	//static void set_log_level(EFirebaseLogLevel log_level);

	/**
	 * Disables network access for this instance. While the network is disabled,
	 * any snapshot listeners or Get() calls will return results from cache, and
	 * any write operations will be queued until network usage is re-enabled via a
	 * call to EnableNetwork().
	 *
	 * If the network was already disabled, calling `DisableNetwork()` again is
	 * a no-op.
	 */
	static void DisableNetwork(const FFirestoreCallback& Callback);

	/**
	 * Re-enables network usage for this instance after a prior call to
	 * DisableNetwork().
	 *
	 * If the network is currently enabled, calling `EnableNetwork()` is a no-op.
	 */
	static void EnableNetwork(const FFirestoreCallback& Callback);

	/**
	 * Waits until all currently pending writes for the active user have been
	 * acknowledged by the backend.
	 *
	 * The returned future is resolved immediately without error if there are no
	 * outstanding writes. Otherwise, the future is resolved when all previously
	 * issued writes (including those written in a previous app session) have been
	 * acknowledged by the backend. The future does not wait for writes that were
	 * added after the method is called. If you wish to wait for additional
	 * writes, you have to call `WaitForPendingWrites` again.
	 *
	 * Any outstanding `WaitForPendingWrites` futures are resolved with an
	 * error during user change.
	 */
	static void WaitForPendingWrites(const FFirestoreCallback& Callback);

	/**
	 * Sets if persistence is enabled or not. 
	 * This is the same as calling SetSettings() with PersistenceEnabled set to bEnabled.
	*/
	// Disabled for now as the Desktop Firestore SDK crashes when using Firestore with persistence disabled.
	//UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore")
	//static void SetPersistenceEnabled(const bool bEnabled);

private:
#if WITH_FIREBASE_FIRESTORE
	static firebase::firestore::Firestore* GetFirestore();
#endif // WITH_FIREBASE_FIRESTORE
};


