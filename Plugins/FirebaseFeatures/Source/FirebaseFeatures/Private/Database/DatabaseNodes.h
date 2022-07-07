// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "Database/Database.h"
#include "Database/DatabaseReference.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Kismet/BlueprintAsyncActionBase.h"
#include "DatabaseNodes.generated.h"

UCLASS()
class UFirebaseDatabaseLibrary final : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:

	/// @brief Used to obtain a MutableData instance that encapsulates
	/// the data and priority at the given relative path.
	///
	/// @param[in] Path Path relative to this snapshot's location.
	///
	/// @returns MutableData for the Child relative to this location. The memory
	/// will be freed when the Transaction is finished.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Get Child"))
	static UPARAM(DisplayName = "Mutable") FMutableData Mutable_GetChild(UPARAM(ref) FMutableData& MutableData, const FString& Path);

	/// @brief Get all the immediate children of this location.
	///
	/// @returns The immediate children of this location.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Get Children"))
	static UPARAM(DisplayName = "Mutables") TArray<FMutableData> Mutable_GetChildren(UPARAM(ref) FMutableData& MutableData);

	/// @brief Get the number of children of this location.
	///
	/// @returns The number of immediate children of this location.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Get Children Count"))
	static UPARAM(DisplayName = "Count") int64 Mutable_GetChildrenCount(UPARAM(ref) FMutableData& MutableData);

	/// @brief Get the key name of the source location of this data.
	///
	/// @returns Key name of the source location of this data.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Get Key"))
	static UPARAM(DisplayName = "Key") FString Mutable_GetKey(UPARAM(ref) const FMutableData& MutableData);

	/// @brief Get the value of the data contained at this location.
	///
	/// @returns The value of the data contained at this location.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Get Value"))
	static UPARAM(DisplayName = "Value") FFirebaseVariant Mutable_GetValue(UPARAM(ref) const FMutableData& MutableData);

	/// @brief Get the priority of the data contained at this snapshot.
	///
	/// @returns The value of this location's Priority relative to its siblings.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Get Priority"))
	static UPARAM(DisplayName = "Priority") FFirebaseVariant Mutable_GetPriority(UPARAM(ref) FMutableData& MutableData);

	/// @brief Does this MutableData have data at a particular location?
	///
	/// @param[in] path Path relative to this data's location.
	///
	/// @returns True if there is data at the specified location, false if not.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Has Child"))
	static UPARAM(DisplayName = "Has Child") bool Mutable_HasChild(UPARAM(ref) const FMutableData& MutableData, const FString& Path);

	/// @brief Sets the data at this location to the given value.
	///
	/// @param[in] value The value to set this location to. The Variant's type
	/// corresponds to the types accepted by the database JSON:
	/// Null: Deletes this location from the database.
	/// Int64: Inserts an integer value into this location.
	/// Double: Inserts a floating point value into this location.
	/// String: Inserts a string into this location.
	///         (Accepts both Mutable and Static strings)
	/// Vector: Inserts a JSON array into this location. The elements can be any
	///         Variant type, including Vector and Map.
	/// Map: Inserts a JSON associative array into this location. The keys must
	///      be of type String (or Int64/Double which are converted to String).
	///      The values can be any Variant type, including Vector and Map.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Set Value"))
	static void Mutable_SetValue(UPARAM(ref) FMutableData& MutableData, const FFirebaseVariant& Value);

	/// @brief Sets the priority of this field, which controls its sort
	/// order relative to its siblings.
	///
	/// @see UDatabaseReference::SetPriority() for information
	/// on how Priority affects the ordering of a node's children.
	///
	/// @param[in] Priority Sort priority for this child relative to its siblings.
	/// The Variant types accepted are Null, Int64, Double, and String. Other
	/// types will return kErrorInvalidVariantType.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Mutable", meta = (DisplayName = "Set Priority"))
	static void Mutable_SetPriority(UPARAM(ref) FMutableData& MutableData, const FFirebaseVariant& Priority);

};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDynMultFutureVoidCallback, const EFirebaseDatabaseError, Error);


UCLASS(Abstract, MinimalAPI)
class UDisconnectionHandlerProxybase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:

protected:
	UPROPERTY()
	UDisconnectionHandler* Handler;
};

UCLASS(MinimalAPI)
class UDisconnectionCancelProxy final : public UDisconnectionHandlerProxybase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnCanceled;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:
	virtual void Activate();

	/// @brief Cancel any Disconnection operations that are queued up by this
	/// handler.  When the Future returns, if its Error is kErrorNone, the queue
	/// has been cleared on the server.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference|Disconnection", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Cancel"))
	static UDisconnectionCancelProxy* Cancel(UDisconnectionHandler* const DisconnectionHandler);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);
};

UCLASS(MinimalAPI)
class UDisconnectionRemoveValueProxy final : public UDisconnectionHandlerProxybase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnRemoved;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:
	virtual void Activate();

	/// @brief Remove the value at the current location when the client
	/// disconnects. When the Future returns, if its Error is kErrorNone, the
	/// RemoveValue operation has been successfully queued up on the server.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference|Disconnection", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Remove Value"))
	static UDisconnectionRemoveValueProxy* RemoveValue(UDisconnectionHandler* const DisconnectionHandler);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);
};


UCLASS(MinimalAPI)
class UDisconnectionSetValueProxy final : public UDisconnectionHandlerProxybase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnSet;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:
	virtual void Activate();

	/// @brief Set the value of the data at the current location when the client
	/// disconnects. When the callback is called, if its Error is kErrorNone, the
	/// SetValue operation has been successfully queued up on the server.
	///
	/// @param Value The value to set this location to when the client
	/// disconnects. For information on how the Variant types are used,
	/// see UDatabaseReference::SetValue().
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference|Disconnection", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Set Value"))
	static UDisconnectionSetValueProxy* SetValue(UDisconnectionHandler* const DisconnectionHandler, const FFirebaseVariant& Value);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);

private:
	FFirebaseVariant Val;
};

UCLASS(MinimalAPI)
class UDisconnectionSetValueAndPriorityProxy final : public UDisconnectionHandlerProxybase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnSet;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:
	virtual void Activate();

	/// @brief Set the value and priority of the data at the current location when
	/// the client disconnects. When the callback is called, if its Error is 
	/// kErrorNone, the SetValue operation has been successfully queued up on the
	/// server.
	///
	/// @param[in] Value The value to set this location to when the client
	/// disconnects. For information on how the Variant types are used,
	/// see UDatabaseReference::SetValue().
	/// @param[in] Priority The priority to set this location to when the client
	/// disconnects. The Variant types accepted are Null, Int64, Double, and
	/// String. For information about how priority is used, see
	/// UDatabaseReference::SetPriority().
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference|Disconnection", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Set Value and Priority"))
	static UDisconnectionSetValueAndPriorityProxy* SetValueAndPriority(UDisconnectionHandler* const DisconnectionHandler, const FFirebaseVariant& Value, const FFirebaseVariant& Priority);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);

private:
	FFirebaseVariant Val;
	FFirebaseVariant Prio;
};

UCLASS(MinimalAPI)
class UDisconnectionUpdateChildrenProxy final : public UDisconnectionHandlerProxybase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:
	virtual void Activate();

	/// @brief Updates the specified child keys to the given values when the
	/// client disconnects. When the callback is called, if its Error is kErrorNone, 
	/// the UpdateChildren operation has been successfully queued up by the
	/// server.
	///
	/// @param[in] values A variant of type Map. The keys are the paths to update
	/// and must be of type String (or Int64/Double which are converted to
	/// String). The values can be any Variant type. A value of Variant type Null
	/// will delete the child.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference|Disconnection", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Set Value and Priority"))
	static UDisconnectionUpdateChildrenProxy* UpdateChildren(UDisconnectionHandler* const DisconnectionHandler, const FFirebaseVariant& Value);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);

private:
	FFirebaseVariant Val;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDynMultSnapshot, const EFirebaseDatabaseError, Error, UDataSnapshot* const, Snapshot);

UCLASS(MinimalAPI)
class UDatabaseQueryGetValue final : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSnapshot OnDataReceived;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSnapshot OnError;

public:
	virtual void Activate();
	
	/// @brief Gets the value of the query for the given location a single time.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Get Value"))
	static UDatabaseQueryGetValue* GetValue(UDatabaseQuery* const DatabaseQuery);

private:
	void OnActionOver(const EFirebaseDatabaseError Error, UDataSnapshot* const Snapshot);

private:
	UPROPERTY()
	UDatabaseQuery* Query;
};


UCLASS(Abstract)
class UDatabaseReferenceProxyBase : public UBlueprintAsyncActionBase
{
	GENERATED_BODY()
protected:
	UPROPERTY()
	UDatabaseReference* Reference;
};

UCLASS()
class URemoveValueProxy final : public UDatabaseReferenceProxyBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnRemoved;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:

	virtual void Activate();

	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Remove Value"))
	static URemoveValueProxy* RemoveValue(UDatabaseReference* const DatabaseReference);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);
};

DECLARE_DYNAMIC_DELEGATE_RetVal_OneParam(ETransactionResult, FRunTransactionCallback, const FMutableData&, Data);

UCLASS()
class URunTransactionProxy final : public UDatabaseReferenceProxyBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultSnapshot OnTrasactionOver;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultSnapshot OnError;

public:

	virtual void Activate();

	/// @brief Run a user-supplied callback function (passing in a context),
	/// possibly multiple times, to perform an atomic transaction on the database.
	///
	/// @param[in] TransactionFunction The user-supplied function that will be
	/// called, possibly multiple times, to perform the database transaction.
	/// @param[in] bTriggerLocalEvents If true, events will be triggered for
	/// intermediate state changes during the transaction. If false, only the
	/// final state will cause events to be triggered.
	///
	/// @returns A Future result, which will complete when the transaction either
	/// succeeds or fails. When the Future is completed, if its Error is
	/// kErrorNone, the operation succeeded and the transaction was committed, and
	/// the new value of the data will be returned in the DataSnapshot result. If
	/// the Error is kErrorTransactionAbortedByUser, the transaction was aborted
	/// because the transaction function returned kTransactionResultAbort, and the
	/// old value will be returned in DataSnapshot. Otherwise, if some other error
	/// occurred, Error and ErrorMessage will be set and DataSnapshot will be
	/// invalid.
	///
	/// @note Only one RunTransaction() should be running on a given database
	/// location at the same time.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Run Transaction"))
	static URunTransactionProxy* RunTransaction(UDatabaseReference* const DatabaseReference, const bool bTriggerLocalEvents, const FRunTransactionCallback& TransactionFunction);

private:
	void OnActionOver(const EFirebaseDatabaseError Error, UDataSnapshot* const Snapshot);
	ETransactionResult OnTransaction(const FMutableData& Data);

private:
	bool bTriggerLocalEvents;
	FRunTransactionCallback TransactionFunction;
};

UCLASS()
class USetPriorityProxy final : public UDatabaseReferenceProxyBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnSet;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:

	virtual void Activate();

	/// @brief Sets the priority of this field, which controls its sort
	/// order relative to its siblings.
	///
	/// In Firebase, children are sorted in the following order:
	/// 1. First, children with no priority.
	/// 2. Then, children with numerical priority, sorted numerically in
	///    ascending order.
	/// 3. Then, remaining children, sorted lexicographically in ascending order
	///    of their text priority.
	///
	/// Children with the same priority (including no priority) are sorted by
	/// key:
	/// A. First, children with keys that can be parsed as 32-bit integers,
	///    sorted in ascending numerical order of their keys.
	/// B. Then, remaining children, sorted in ascending lexicographical order
	///    of their keys.
	///
	/// This is an asynchronous operation which takes time to execute, and uses
	/// a callback to return its result.
	///
	/// @param[in] Priority Sort priority for this child relative to its siblings.
	/// The Variant types accepted are Null, Int64, Double, and String. Other
	/// types will return kErrorInvalidVariantType.
	///
	/// @note Only one SetPriority() should be running on a given database
	/// location
	/// at the same time. If you need to run multiple operations at once, use
	/// RunTransaction().
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Set Priority"))
	static USetPriorityProxy* SetPriority(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Priority);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);

private:
	FFirebaseVariant Prio;
};

UCLASS()
class USetValueProxy final : public UDatabaseReferenceProxyBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnSet;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:

	virtual void Activate();

	/// @brief Sets the data at this location to the given value.
	///
	/// This is an asynchronous operation which takes time to execute, and uses
	/// a Callback to return its result.
	///
	/// @param[in] value The value to set this location to. The Variant's type
	/// corresponds to the types accepted by the database JSON:
	/// Null: Deletes this location from the database.
	/// Int64: Inserts an integer value into this location.
	/// Double: Inserts a floating point value into this location.
	/// String: Inserts a string into this location.
	///         (Accepts both Mutable and Static strings)
	/// Vector: Inserts a JSON array into this location. The elements can be any
	///         Variant type, including Vector and Map.
	/// Map: Inserts a JSON associative array into this location. The keys must
	///      be of type String (or Int64/Double which are converted to String).
	///      The values can be any Variant type, including Vector and Map.
	///
	/// @note Only one SetValue() should be running on a given database location
	/// at the same time. If you need to run multiple operations at once, use
	/// RunTransaction(). 
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Set Value"))
	static USetValueProxy* SetValue(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Value);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);

private:
	FFirebaseVariant Val;
};

UCLASS()
class USetValueAndPriorityProxy final : public UDatabaseReferenceProxyBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnSet;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:

	virtual void Activate();

	/// @brief Sets both the data and priority of this location. See SetValue()
	/// and SetPriority() for context on the parameters.
	///
	/// This is an asynchronous operation which takes time to execute, and uses
	/// a callback to return its result.
	///
	/// @param[in] value The value to set this location to. See SetValue() for
	/// information on the types accepted.
	/// @param[in] priority The priority to set this location to. See
	/// SetPriority() for information on the types accepted.7
	///
	/// @note Only one SetValueAndPriority() should be running on a given database
	/// location at the same time. SetValueAndPriority() can't be used on the same
	/// location at the same time as either SetValue() or SetPriority(), and will
	/// return kErrorConflictingOperationInProgress if you try. If you need to run
	/// multiple operations at once, use RunTransaction().
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Set Value and Priority"))
	static USetValueAndPriorityProxy* SetValueAndPriority(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Priority, const FFirebaseVariant& Value);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);

private:
	FFirebaseVariant Prio;
	FFirebaseVariant Val;
};

UCLASS()
class UUpdateChildrenProxy final : public UDatabaseReferenceProxyBase
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnUpdated;
	
	UPROPERTY(BlueprintAssignable)
	FDynMultFutureVoidCallback OnError;

public:

	virtual void Activate();

	/// @brief Updates the specified child keys to the given values.
	///
	/// @param[in] values A variant of type Map. The keys are the paths to update
	/// and must be of type String (or Int64/Double which are converted to
	/// String). The values can be any Variant type. A value of Variant type Null
	/// will delete the child.
	///
	/// @note This method will return kErrorConflictingOperationInProgress if it
	/// is run at the same time as SetValue(), SetValueAndPriority(), or
	/// RemoveValue() in the same location.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference", meta = (BlueprintInternalUseOnly = "true", DisplayName = "Update Children"))
	static UUpdateChildrenProxy* UpdateChildren(UDatabaseReference* const DatabaseReference, const FFirebaseVariant& Value);

private:
	void OnActionOver(const EFirebaseDatabaseError Error);

private:
	FFirebaseVariant Val;
};




