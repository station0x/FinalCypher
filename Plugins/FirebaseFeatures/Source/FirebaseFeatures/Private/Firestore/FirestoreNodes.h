// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Firestore/Firestore.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "Firestore/Query.h"
#include "FirestoreNodes.generated.h"

/**
 *	This library contains methods for Firestore.
 *  It is meant for Blueprints only, that's why its symbols are not exported and
 *  it is located into the private directory of the plugin.
 *  C++ should directly call those methods from their object.
 **/
UCLASS()
class UFirestoreLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/**
	 * Returns an instance that can be used to change the behavior of Set() calls
	 * to only replace the values specified in its data argument. Fields omitted
	 * from the Set() call will remain untouched.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|SetOptions")
	static UPARAM(DisplayName = "Set Options") FFirestoreSetOptions Merge();

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|SetOptions")
	static UPARAM(DisplayName = "Set Options") FFirestoreSetOptions MergeFields(const TArray<FString>& Fields);

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
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|SetOptions")
	static UPARAM(DisplayName = "Set Options") FFirestoreSetOptions MergeFieldPaths(const TArray<FFirestoreFieldPath>& Fields);

	/**
	 * Writes to the document referred to by the provided reference.
	 *
	 * If the document does not yet exist, it will be created. If you pass
	 * SetOptions, the provided data can be merged into an existing document.
	 *
	 * @param[in] Document The DocumentReference to overwrite.
	 * @param[in] Data A map of the fields and values to write to the document.
	 * @param[in] Options An object to configure the Set() behavior (optional).
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Transaction", meta = (AutoCreateRefTerm = "Options"))
	static void Set(UPARAM(ref) FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document, 
		const TMap<FString, FFirestoreFieldValue>& Data, const FFirestoreSetOptions& Options);

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param[in] document The DocumentReference to update.
	 * @param[in] data A map of field / value pairs to update. Fields can contain
	 * dots to reference nested fields within the document.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Transaction")
	static void Update(UPARAM(ref) FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document,
		const TMap<FString, FFirestoreFieldValue>& Data);

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param[in] document The DocumentReference to update.
	 * @param[in] data A map from FieldPath to FieldValue to update.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Transaction")
	static void UpdateFieldPath(UPARAM(ref) FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document,
		const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data);

	/**
	 * Deletes the document referred to by the provided reference.
	 *
	 * @param[in] document The DocumentReference to delete.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Transaction")
	static void Delete(UPARAM(ref) FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document);

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
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Transaction")
	static UPARAM(DisplayName = "Snapshot") FFirestoreDocumentSnapshot Get(UPARAM(ref) FFirestoreTransaction& Transaction, UFirestoreDocumentReference* const Document,
		EFirestoreError& ErrorCode, FString& ErrorMessage);

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
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|WriteBatch", meta =(AutoCreateRefTerm="Options", DisplayName ="Set"))
	static UPARAM(DisplayName = "Write Batch") FWriteBatch& WriteBatch_Set(UPARAM(ref) FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document,
		const TMap<FString, FFirestoreFieldValue>& Data,
		const FFirestoreSetOptions& Options);

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param document The DocumentReference to update.
	 * @param data A map of field / value pairs to update. Fields can contain dots
	 * to reference nested fields within the document.
	 * @return This WriteBatch instance. Used for chaining method calls.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|WriteBatch", meta =(AutoCreateRefTerm="Options", DisplayName ="Update"))
	static UPARAM(DisplayName = "Write Batch") FWriteBatch& WriteBatch_Update(UPARAM(ref) FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document,
		const TMap<FString, FFirestoreFieldValue>& Data);

	/**
	 * Updates fields in the document referred to by the provided reference. If no
	 * document exists yet, the update will fail.
	 *
	 * @param document The DocumentReference to update.
	 * @param data A map from FieldPath to FieldValue to update.
	 * @return This WriteBatch instance. Used for chaining method calls.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|WriteBatch", meta =(AutoCreateRefTerm="Options", DisplayName ="Update (Field Path)"))
	static UPARAM(DisplayName = "Write Batch") FWriteBatch& WriteBatch_UpdateFieldPath(UPARAM(ref) FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document,
		const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data);

	/**
	 * Deletes the document referred to by the provided reference.
	 *
	 * @param document The DocumentReference to delete.
	 * @return This WriteBatch instance. Used for chaining method calls.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|WriteBatch", meta =(AutoCreateRefTerm="Options", DisplayName ="Delete"))
	static UPARAM(DisplayName = "Write Batch") FWriteBatch& WriteBatch_Delete(UPARAM(ref) FWriteBatch& WriteBatch, UFirestoreDocumentReference* const Document);

	/**
	 * A special sentinel FieldPath to refer to the ID of a document. It can be
	 * used in queries to sort or filter by the document ID.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldPath")
	static UPARAM(DisplayName = "Field Path")  FFirestoreFieldPath DocumentId();

	/**
	 * Returns a string representation of this `FieldPath` for
	 * logging/debugging purposes.
	 *
	 * @note the exact string representation is unspecified and subject to
	 * change; don't rely on the format of the string.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldPath")
	static UPARAM(DisplayName = "String Value") FString ToString(UPARAM(ref) const FFirestoreFieldPath& FieldPath);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldPath")
	static UPARAM(DisplayName = "Field Path") FFirestoreFieldPath MakeFirestoreFieldPath(const TArray<FString>& FieldNames);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue")
	static UPARAM(DisplayName = "Type") EFirestoreFieldValueType GetType(UPARAM(ref) const FFirestoreFieldValue& FieldValue) { return FieldValue.GetType();  }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static bool FieldValue_ToBool(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static int32 FieldValue_ToInt32(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static int64 FieldValue_ToInt64(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static float FieldValue_ToFloat(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FString FieldValue_ToString(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "TIMESTAMP", BlueprintAutocast))
	static FFirestoreTimestamp FieldValue_ToTimestamp(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "GEO POINT", BlueprintAutocast))
	static FFirestoreGeoPoint FieldValue_ToGeoPoint(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static TArray<uint8> FieldValue_ToBinary(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static TArray<FFirestoreFieldValue> FieldValue_ToArray(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static TMap<FString, FFirestoreFieldValue> FieldValue_ToMap(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static UFirestoreDocumentReference* FieldValue_ToReference(FFirestoreFieldValue Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromBool(bool bValue) { return bValue; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromInt32(int32 Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromInt64(int64 Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromFloat(float Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromString(const FString& Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromBinary(const TArray<uint8>& Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromArray(const TArray<FFirestoreFieldValue>& Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromMap(const TMap<FString, FFirestoreFieldValue>& Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromReference(UFirestoreDocumentReference* Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromGeoPoint(const FFirestoreGeoPoint&  Value) { return Value; }
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "->", BlueprintAutocast))
	static FFirestoreFieldValue FieldValue_FromTimestamp(const FFirestoreTimestamp& Value) { return Value; }

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot")
	static UPARAM(DisplayName = "ID") FString GetId(const FFirestoreDocumentSnapshot& DocumentSnapshot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot")
	static UPARAM(DisplayName = "DocumentReference") UFirestoreDocumentReference* GetReference(const FFirestoreDocumentSnapshot& DocumentSnapshot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot")
	static UPARAM(DisplayName = "Metadata") FFirestoreSnapshotMetadata GetMetadata(const FFirestoreDocumentSnapshot& DocumentSnapshot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot")
	static UPARAM(DisplayName = "Exists") bool Exists(const FFirestoreDocumentSnapshot& DocumentSnapshot);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot")
	static UPARAM(DisplayName = "Data") TMap<FString, FFirestoreFieldValue> GetData(
		const FFirestoreDocumentSnapshot& DocumentSnapshot, 
		EFirestoreServerTimestampBehavior ServerTimestampBehavior = EFirestoreServerTimestampBehavior::Default);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot", meta=(DisplayName = "Get"))
	static UPARAM(DisplayName = "Value") FFirestoreFieldValue Get_Snapshot(
		const FFirestoreDocumentSnapshot& DocumentSnapshot,
		const FString& Field,
		EFirestoreServerTimestampBehavior ServerTimestampBehavior = EFirestoreServerTimestampBehavior::Default);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot")
	static UPARAM(DisplayName = "Value") FFirestoreFieldValue GetWithFieldPath(
		const FFirestoreDocumentSnapshot& DocumentSnapshot,
		const FFirestoreFieldPath& Field,
		EFirestoreServerTimestampBehavior ServerTimestampBehavior = EFirestoreServerTimestampBehavior::Default);

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot", meta = (DisplayName = "To String"))
	static UPARAM(DisplayName = "String") FString ToString_Snapshot(const FFirestoreDocumentSnapshot& DocumentSnapshot);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Timestamp", meta = (DisplayName = "Now", CompactNodeTitle = "NOW"))
	static FFirestoreTimestamp Now() { return FFirestoreTimestamp::Now(); };

	/**
	 * Converts this timestamp to a date time, losing nanoseconds value in the process.
	 * @return A date time.
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category ="Firebase|Firestore|Timestamp", meta = (DisplayName = "To DateTime", CompactNodeTitle = "DATE", BlueprintAutocast))
	static UPARAM(DisplayName = "Date") FDateTime ToDateTime(const FFirestoreTimestamp& Timestamp) { return Timestamp.ToDateTime(); }

	/**
	 * Converts this timestamp to a human-readable string.
	 * @return A human-readable string
	*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Timestamp", meta = (DisplayName = "To String", CompactNodeTitle = "->", BlueprintAutocast))
	static FString FirestoreTimestamp_ToString(const FFirestoreTimestamp& Timestamp) { return Timestamp.ToString(); }

	/**
	 * @brief Returns a sentinel for use with Update() to mark a field for
	 * deletion.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (DisplayName = "Delete", CompactNodeTitle = "DELETE"))
	static UPARAM(DisplayName = "Deleter") FFirestoreFieldValue FieldValue_Delete() { return FFirestoreFieldValue::Delete(); }

	/**
	 * Returns a sentinel that can be used with Set() or Update() to include
	 * a server-generated timestamp in the written data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "SERVER TIMESTAMP"))
	static UPARAM(DisplayName = "Timestamp") FFirestoreFieldValue ServerTimestamp() { return FFirestoreFieldValue::ServerTimestamp(); }

	/** @brief Constructs a null. */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "NULL"))
	static UPARAM(DisplayName = "Null") FFirestoreFieldValue Null() { return FFirestoreFieldValue::Null(); }

	/**
	 * Returns a special value that can be used with Set() or Update() that tells
	 * the server to union the given elements with any array value that already
	 * exists on the server. Each specified element that doesn't already exist in
	 * the array will be added to the end. If the field being modified is not
	 * already an array, it will be overwritten with an array containing exactly
	 * the specified elements.
	 *
	 * @param elements The elements to union into the array.
	 * @return The FieldValue sentinel for use in a call to Set() or Update().
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "ARRAY UNION"))
	static FFirestoreFieldValue ArrayUnion(const TArray<FFirestoreFieldValue>& Elements) { return FFirestoreFieldValue::ArrayUnion(Elements);}

	/**
	 * Returns a special value that can be used with Set() or Update() that tells
	 * the server to remove the given elements from any array value that already
	 * exists on the server. All instances of each element specified will be
	 * removed from the array. If the field being modified is not already an
	 * array, it will be overwritten with an empty array.
	 *
	 * @param elements The elements to remove from the array.
	 * @return The FieldValue sentinel for use in a call to Set() or Update().
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|FieldValue", meta = (CompactNodeTitle = "ARRAY REMOVE"))
	static FFirestoreFieldValue ArrayRemove(const TArray<FFirestoreFieldValue>& Elements) { return FFirestoreFieldValue::ArrayRemove(Elements); }

	/**
	 * Removes the listener being tracked by this ListenerRegistration. After the
	 * initial call, subsequent calls have no effect.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Query|Listener")
	static void Remove(UPARAM(Ref) FQuerySnapshotListenerHandle& ListenerHandle) { ListenerHandle.Remove(); };

	/**
	 * Whether the snapshot contains the result of local writes.
	 * Is true if the snapshot contains the result of local writes (for
	 * example, Set() or Update() calls) that have not yet been committed to the
	 * backend.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Metadata")
	static UPARAM(DisplayName = "Has Changes") bool HasPendingChanges(UPARAM(ref) FFirestoreSnapshotMetadata& Metadata) { return Metadata.bHasPendingWrites; }
	
	/**
	 * Whether the snapshot contains the result of local writes.
	 * Is true if the snapshot contains the result of local writes (for
	 * example, Set() or Update() calls) that have not yet been committed to the
	 * backend.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot", meta = (DisplayName = "Has Pending Changes"))
	static UPARAM(DisplayName = "Has Changes") bool HasDocumentPendingChanges(UPARAM(ref) FFirestoreDocumentSnapshot& Document) { return Document.GetMetadata().bHasPendingWrites; }

	/**
	 * Whether the snapshot was created from cached data.
	 * Is true if the snapshot was created from cached data rather than
	 * guaranteed up-to-date server data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|Metadata")
	static UPARAM(DisplayName = "From Cache") bool IsFromCache(UPARAM(ref) FFirestoreSnapshotMetadata& Metadata) { return Metadata.bIsFromCache; }

	/**
	 * Whether the snapshot was created from cached data.
	 * Is true if the snapshot was created from cached data rather than
	 * guaranteed up-to-date server data.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Firestore|DocumentSnapshot", meta = (DisplayName = "Is From Cache"))
	static UPARAM(DisplayName = "From Cache") bool IsDocumentFromCache(UPARAM(ref) FFirestoreDocumentSnapshot& Document) { return Document.GetMetadata().bIsFromCache; }
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynMultVoid, const EFirestoreError, Error);

UCLASS()
class UWriteBatchCommitProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnCommitted;

	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

	/**
	 * Commits all of the writes in this write batch as a single atomic unit.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|WriteBatch", meta = (BlueprintInternalUseOnly = "true"))
	static UWriteBatchCommitProxy* Commit(UPARAM(ref) FWriteBatch& WriteBatch);

private:
	void OnActionOver(const EFirestoreError Error);
};

DECLARE_DYNAMIC_DELEGATE_RetVal_TwoParams(EFirestoreError, FDynamicTransactionCallback, const FFirestoreTransaction&, Transaction, FString&, ErrorMessage);

UCLASS()
class UFirestoreRunTransactionProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnRun;

	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

	/**
	 * Commits all of the writes in this write batch as a single atomic unit.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore", meta = (BlueprintInternalUseOnly = "true"))
	static UFirestoreRunTransactionProxy* RunTransaction(const FDynamicTransactionCallback& TransactionFunction);

private:
	void OnActionOver(const EFirestoreError Error);
	EFirestoreError TransactionMethod(const FFirestoreTransaction& Transaction, FString& ErrorMessage);

	FDynamicTransactionCallback TransactionCallback;
};

UCLASS()
class UDisableNetworkProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnDisabled;

	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

	/**
	 * Disables network access for this instance. While the network is disabled,
	 * any snapshot listeners or Get() calls will return results from cache, and
	 * any write operations will be queued until network usage is re-enabled via a
	 * call to EnableNetwork().
	 *
	 * If the network was already disabled, calling `DisableNetwork()` again is
	 * a no-op.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore", meta = (BlueprintInternalUseOnly = "true"))
	static UDisableNetworkProxy* DisableNetwork();

private:
	void OnActionOver(const EFirestoreError Error);
};


UCLASS()
class UEnableNetworkProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnEnabled;

	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

	/**
	 * Re-enables network usage for this instance after a prior call to
	 * DisableNetwork().
	 *
	 * If the network is currently enabled, calling `EnableNetwork()` is a no-op.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore", meta = (BlueprintInternalUseOnly = "true"))
	static UEnableNetworkProxy* DisableNetwork();

private:
	void OnActionOver(const EFirestoreError Error);
};

UCLASS()
class UWaitForPendingWritesProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnPendingWritesOver;

	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

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
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore", meta = (BlueprintInternalUseOnly = "true"))
	static UWaitForPendingWritesProxy* DisableNetwork();

private:
	void OnActionOver(const EFirestoreError Error);
};


UCLASS(Abstract)
class UDocumentReferenceAsyncBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY()
	UFirestoreDocumentReference* Reference;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultDocSnapshot, const EFirestoreError, Error, const FFirestoreDocumentSnapshot&, DocumentSnapshot);

UCLASS()
class UDocumentReferenceGetProxy final : public UDocumentReferenceAsyncBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultDocSnapshot OnGot;
	UPROPERTY(BlueprintAssignable)
	FDynMultDocSnapshot OnError;
public:
	virtual void Activate();
	/**
	 * @brief Reads the document referenced by this DocumentReference.
	 *
	 * By default, Get() attempts to provide up-to-date data when possible by
	 * waiting for data from the server, but it may return cached data or fail if
	 * you are offline and the server cannot be reached. This behavior can be
	 * altered via the Source parameter.
	 *
	 * @param Source A value to configure the get behavior.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Document", meta = (BlueprintInternalUseOnly = "true"))
	static UDocumentReferenceGetProxy* Get(UFirestoreDocumentReference* const DocumentReference, EFirestoreSource Source);

private:
	void OnActionOver(const EFirestoreError Error, const FFirestoreDocumentSnapshot& Snapshot);

	EFirestoreSource Src;
};


UCLASS()
class UDocumentReferenceSetProxy final : public UDocumentReferenceAsyncBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnSet;
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();
	/**
	 * @brief Reads the document referenced by this DocumentReference.
	 *
	 * By default, Set() attempts to provide up-to-date data when possible by
	 * waiting for data from the server, but it may return cached data or fail if
	 * you are offline and the server cannot be reached. This behavior can be
	 * altered via the Source parameter.
	 *
	 * @param Source A value to configure the get behavior.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Document", meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Options"))
	static UDocumentReferenceSetProxy* Set(UFirestoreDocumentReference* const DocumentReference, const TMap<FString, FFirestoreFieldValue>& Data,
			const FFirestoreSetOptions& Options);

private:
	void OnActionOver(const EFirestoreError Error);

	EFirestoreSource Src;
	TMap<FString, FFirestoreFieldValue> SetData;
	FFirestoreSetOptions SetOptions;
};


UCLASS()
class UDocumentReferenceUpdateProxy final : public UDocumentReferenceAsyncBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnUpdated;
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

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
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Document", meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Options"))
	static UDocumentReferenceUpdateProxy* Update(UFirestoreDocumentReference* const DocumentReference, const TMap<FString, FFirestoreFieldValue>& Data);

private:
	void OnActionOver(const EFirestoreError Error);

	EFirestoreSource Src;
	TMap<FString, FFirestoreFieldValue> UpdateData;
};


UCLASS()
class UDocumentReferenceUpdateWithPathProxy final : public UDocumentReferenceAsyncBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnUpdated;
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

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
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Document", meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Options"))
	static UDocumentReferenceUpdateWithPathProxy* UpdateWithPath(UFirestoreDocumentReference* const DocumentReference, const TMap<FFirestoreFieldPath, FFirestoreFieldValue>& Data);

private:
	void OnActionOver(const EFirestoreError Error);

	EFirestoreSource Src;
	TMap<FFirestoreFieldPath, FFirestoreFieldValue> UpdateData;
};


UCLASS()
class UDocumentReferenceDeleteProxy final : public UDocumentReferenceAsyncBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnDeleted;
	UPROPERTY(BlueprintAssignable)
	FDynMultVoid OnError;
public:
	virtual void Activate();

	/**
	 * @brief Removes the document referred to by this DocumentReference.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Document", meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Options"))
	static UDocumentReferenceDeleteProxy* Delete(UFirestoreDocumentReference* const DocumentReference);

private:
	void OnActionOver(const EFirestoreError Error);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultDocumentCallback, const EFirestoreError, Error, UFirestoreDocumentReference* const, DocumentReference);

UCLASS()
class UCollectionReferenceAddProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	/**
	 * Called with the DocumentReference of the newly created document.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultDocumentCallback OnAdded;

	/**
	 * Called if an error occurred.
	*/
	UPROPERTY(BlueprintAssignable)
	FDynMultDocumentCallback OnError;
public:
	virtual void Activate();

	/**
	 * @brief Adds a new document to this collection with the specified data,
	 * assigning it a document ID automatically.
	 * @param data A map containing the data for the new document.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Collection", meta = (BlueprintInternalUseOnly = "true", AutoCreateRefTerm = "Options"))
	static UCollectionReferenceAddProxy* Add(UFirestoreCollectionReference* const DocumentReference, TMap<FString, FFirestoreFieldValue> Data);

private:
	void OnActionOver(const EFirestoreError Error, UFirestoreDocumentReference* const Document);

	UPROPERTY()
	UFirestoreCollectionReference* Reference;

	TMap<FString, FFirestoreFieldValue> DataAdd;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FDynMultDocumentsCallback, const EFirestoreError, Error, 
	const TArray<FFirestoreDocumentSnapshot>&, DocumentSnapshots, const TArray<class UFirestoreDocumentChange*>&, DocumentChanges);

UCLASS()
class UQueryGetProxy final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultDocumentsCallback OnGot;

	UPROPERTY(BlueprintAssignable)
	FDynMultDocumentsCallback OnError;

public:
	virtual void Activate();

	/**
	 * Executes the query and returns the results as a list of DocumentSnapshots.
	 *
	 * By default, Get() attempts to provide up-to-date data when possible by
	 * waiting for data from the server, but it may return cached data or fail if
	 * you are offline and the server cannot be reached. This behavior can be
	 * altered via the Source parameter.
	 *
	 * @param Source A value to configure the get behavior (optional).
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query", meta = (BlueprintInternalUseOnly = "true"))
	static UQueryGetProxy* Get(UFirestoreQuery* Query, const EFirestoreSource Source = EFirestoreSource::Default);

private:
	void OnTaskOver(const EFirestoreError Error, TArray<FFirestoreDocumentSnapshot> DocumentSnapshots, TArray<class UFirestoreDocumentChange*> Changes);

	UPROPERTY()
	UFirestoreQuery* Query;

	EFirestoreSource Source;
};




