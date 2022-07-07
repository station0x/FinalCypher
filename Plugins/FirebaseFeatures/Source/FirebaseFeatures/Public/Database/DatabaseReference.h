// Copyright Pandores Marketplace 2022. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"

#include "FirebaseFeatures.h"

#if WITH_FIREBASE_DATABASE
THIRD_PARTY_INCLUDES_START
#	include "firebase/database/database_reference.h"
THIRD_PARTY_INCLUDES_END
#endif

#include "FirebaseSdk/FirebaseVariant.h"
#include "Database.h"
#include "DatabaseReference.generated.h"

class UDatabase;
class UDataSnapshot;


/// Specifies whether the transaction succeeded or not.
UENUM(BlueprintType)
enum class ETransactionResult : uint8
{
	/// The transaction was successful, the MutableData was updated.
	Success,
	/// The transaction did not succeed. Any changes to the MutableData
	/// will be discarded.
	Abort,
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FMutableData
{
	GENERATED_BODY()
private:
	friend class UDatabaseReference;

public:
#if WITH_FIREBASE_DATABASE
	FMutableData(firebase::database::MutableData&& InMutable);
#endif

#if WITH_FIREBASE_DATABASE
	FMutableData();
	FMutableData(const FMutableData& Other);

	~FMutableData();

	/// @brief Used to obtain a MutableData instance that encapsulates
	/// the data and priority at the given relative path.
	///
	/// @param[in] path Path relative to this snapshot's location.
	///
	/// @returns MutableData for the Child relative to this location. The memory
	/// will be freed when the Transaction is finished.
	FMutableData GetChild(const FString& Path);
	
	/// @brief Get all the immediate children of this location.
	///
	/// @returns The immediate children of this location.
	TArray<FMutableData> GetChildren();

	/// @brief Get the number of children of this location.
	///
	/// @returns The number of immediate children of this location.
	int64 GetChildrenCount();

	/// @brief Get the key name of the source location of this data.
	///
	/// @returns Key name of the source location of this data.
	FString GetKey() const;

	/// @brief Get the value of the data contained at this location.
	///
	/// @returns The value of the data contained at this location.
	FFirebaseVariant GetValue() const;

	/// @brief Get the priority of the data contained at this snapshot.
	///
	/// @returns The value of this location's Priority relative to its siblings.
	FFirebaseVariant GetPriority();

	/// @brief Does this MutableData have data at a particular location?
	///
	/// @param[in] path Path relative to this data's location.
	///
	/// @returns True if there is data at the specified location, false if not.
	bool HasChild(const FString& Path) const;

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
	void SetValue(const FFirebaseVariant& Value);

	/// @brief Sets the priority of this field, which controls its sort
	/// order relative to its siblings.
	///
	/// @see UDatabaseReference::SetPriority() for information
	/// on how Priority affects the ordering of a node's children.
	///
	/// @param[in] Priority Sort priority for this child relative to its siblings.
	/// The Variant types accepted are Null, Int64, Double, and String. Other
	/// types will return kErrorInvalidVariantType.
	void SetPriority(const FFirebaseVariant& Priority);

public:
	FMutableData& operator=(const FMutableData& Other);

private:
	TSharedPtr<firebase::database::MutableData> Mutable;
#endif
};

DECLARE_DELEGATE_OneParam	(FDatabaseCallback, const EFirebaseDatabaseError /* Error */);
DECLARE_DELEGATE_TwoParams	(FSnapshotCallback, const EFirebaseDatabaseError /* Error */, UDataSnapshot* const /* Snapshot */);
DECLARE_DELEGATE_RetVal_OneParam(ETransactionResult, FTransactionCallback, const FMutableData&/* Data */);

/// A DataSnapshot instance contains data from a Firebase Database location. Any
/// time you read Database data, you receive the data as a DataSnapshot. These
/// are efficiently-generated and cannot be changed. To modify data,
/// use UDatabaseReference::SetValue() or UDatabaseReference::RunTransaction().
UCLASS(BlueprintType)
class FIREBASEFEATURES_API UDataSnapshot : public UObject
{
	GENERATED_BODY()
private:
	friend class FValueListener;
	friend class FChildListener;
	friend class UDataSnapshot;
	friend class UDatabaseReference;
	friend class UDatabaseQuery;

public:

	/// @brief Returns true if the data is non-empty.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Exists") bool Exists() const;

	/// @brief Get a DataSnapshot for the location at the specified relative path.
	/// @param[in] path Path relative to this snapshot's location.
	/// @returns A DataSnapshot corresponding to specified child location.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Child") UDataSnapshot* GetChild(const FString& Path) const;

	/// @brief Get all the immediate children of this location.
	///
	/// @returns The immediate children of this snapshot.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Children") TArray<UDataSnapshot*> GetChildren() const;

	/// @brief Get all the immediate children of this location.
	/// @returns The immediate children of this snapshot.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Count") int64 ChildrenCount() const;

	/// @brief Does this DataSnapshot have any children at all?
	///
	/// @returns True if the snapshot has any children, false otherwise.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Has Childrend") bool HasChildren() const;

	/// @brief Get the key name of the source location of this snapshot.
	///
	/// @note The returned pointer is only guaranteed to be valid while the
	/// DataSnapshot is still in memory.
	///
	/// @returns Key name of the source location of this snapshot.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Key") FString GetKey() const;

	/// @brief Get the value of the data contained in this snapshot.
	///
	/// @returns The value of the data contained in this location.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Value") FFirebaseVariant GetValue() const;

	/// @brief Get the priority of the data contained in this snapshot.
	///
	/// @returns The value of this location's Priority relative to its siblings.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Priority") FFirebaseVariant GetPriority() const;

	/// @brief Obtain a DatabaseReference to the source location for this
	/// snapshot.
	///
	/// @returns A DatabaseReference corresponding to same location as
	/// this snapshot.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Reference") UDatabaseReference* GetReference() const;

	/// @brief Does this DataSnapshot have data at a particular location?
	///
	/// @param[in] path Path relative to this snapshot's location.
	///
	/// @returns True if the snapshot has data at the specified location, false if
	/// not.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Has Child") bool HasChild(const FString& Path) const;

	/// @brief Returns true if this snapshot is valid, false if it is not
	/// valid. An invalid snapshot could be returned by a transaction where an
	/// error has occurred.
	///
	/// @returns true if this snapshot is valid, false if this snapshot is
	/// invalid.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Snapshot")
	UPARAM(DisplayName = "Is Valid") bool IsValid() const;


private:
#if WITH_FIREBASE_DATABASE
	firebase::database::DataSnapshot Snapshot;
#endif
};

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UDisconnectionHandler : public UObject
{
	GENERATED_BODY()
private:
	friend class UDatabaseReference;

public:
	UDisconnectionHandler();

#if WITH_FIREBASE_DATABASE
	/// @brief Cancel any Disconnection operations that are queued up by this
	/// handler.  When the Future returns, if its Error is kErrorNone, the queue
	/// has been cleared on the server.
	void Cancel(const FDatabaseCallback& Callback);

	/// @brief Remove the value at the current location when the client
	/// disconnects. When the callback is called, if its Error is kErrorNone, the
	/// RemoveValue operation has been successfully queued up on the server.
	void RemoveValue(const FDatabaseCallback& Callback);

	/// @brief Set the value of the data at the current location when the client
	/// disconnects. When the callback is called, if its Error is kErrorNone, the
	/// SetValue operation has been successfully queued up on the server.
	///
	/// @param[in] value The value to set this location to when the client
	/// disconnects. For information on how the Variant types are used,
	/// see UDatabaseReference::SetValue().
	void SetValue(const FFirebaseVariant& Value, const FDatabaseCallback& Callback);

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
	void SetValueAndPriority(const FFirebaseVariant& Value, const FFirebaseVariant& Priority, const FDatabaseCallback& Callback);

	/// @brief Updates the specified child keys to the given values when the
	/// client disconnects. When the callback is called, if its Error is kErrorNone, 
	/// the UpdateChildren operation has been successfully queued up by the
	/// server.
	///
	/// @param[in] values A variant of type Map. The keys are the paths to update
	/// and must be of type String (or Int64/Double which are converted to
	/// String). The values can be any Variant type. A value of Variant type Null
	/// will delete the child.
	void UpdateChildren(const FFirebaseVariant& Values, const FDatabaseCallback& Callback);

private:
	firebase::database::DisconnectionHandler* Handler;
#endif
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQueryChildEvent, class UDataSnapshot*, Snapshot, const FString&, PreviousSiblingKey);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FQueryCancelEvent, const EFirebaseDatabaseError, Error, const FString&, ErrorMessage);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FQueryValueEvent, class UDataSnapshot*, Snapshot);

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UDatabaseQuery : public UObject
{
	GENERATED_BODY()
private:
	friend class FChildListener;

public:
	UPROPERTY(BlueprintAssignable, Category = "Firebase|Database|Query")
	FQueryChildEvent OnChildAdded;
	
	UPROPERTY(BlueprintAssignable, Category = "Firebase|Database|Query")
	FQueryChildEvent OnChildChanged;
	
	UPROPERTY(BlueprintAssignable, Category = "Firebase|Database|Query")
	FQueryChildEvent OnChildMoved;
	
	UPROPERTY(BlueprintAssignable, Category = "Firebase|Database|Query")
	FQueryChildEvent OnChildRemoved;

	UPROPERTY(BlueprintAssignable, Category = "Firebase|Database|Query")
	FQueryCancelEvent OnCancelled;

	UPROPERTY(BlueprintAssignable, Category = "Firebase|Database|Query")
	FQueryValueEvent OnValueChanged;

public:
	UDatabaseQuery();
	UDatabaseQuery(FVTableHelper& Helper);
	virtual ~UDatabaseQuery();

	/// @brief Gets the value of the query for the given location a single time.
	void GetValue(const FSnapshotCallback& Callback);

	/// @brief Gets a DatabaseReference corresponding to the given location.
	///
	/// @returns A DatabaseReference corresponding to the same location as the
	/// Query, but without any of the ordering or filtering parameters.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseReference* GetReference() const;

	/// @brief Gets a query in which child nodes are ordered by the values of the
	/// specified path. Any previous OrderBy directive will be replaced in the
	/// returned Query.
	///
	/// @param Path Path to a child node. The value of this node will be used
	/// for sorting this query.
	///
	/// @returns A Query in this same location, with the children are sorted by
	/// the value of their own child specified here.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* OrderByChild(const FString& Path);

	/// @brief Gets a query in which child nodes are ordered by their keys. Any
	/// previous OrderBy directive will be replaced in the returned Query.
	///
	/// @returns A Query in this same location, with the children are sorted by
	/// their key.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* OrderByKey();

	/// @brief Gets a query in which child nodes are ordered by their priority.
	/// Any previous OrderBy directive will be replaced in the returned Query.
	///
	/// @returns A Query in this same location, with the children are sorted by
	/// their priority.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* OrderByPriority();

	/// @brief Create a query in which nodes are ordered by their value.
	///
	/// @returns A Query in this same location, with the children are sorted by
	/// their value.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* OrderByValue();

	/// @brief Get a Query constrained to nodes with the given sort value or
	/// higher.
	///
	/// This method is used to generate a reference to a limited view of the data
	/// at this location. The Query returned will only refer to child nodes with a
	/// value greater than or equal to the given value, using the given OrderBy
	/// directive (or priority as the default).
	///
	/// @param[in] order_value The lowest sort value the Query should include.
	///
	/// @returns A Query in this same location, filtering out child nodes that
	/// have a lower sort value than the sort value specified.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* StartAt(const FFirebaseVariant& OrderValue);


	/// @brief Get a Query constrained to nodes with the given sort value or
	/// higher, and the given key or higher.
	///
	/// This method is used to generate a reference to a limited view of the data
	/// at this location. The Query returned will only refer to child nodes with a
	/// value greater than or equal to the given value, using the given OrderBy
	/// directive (or priority as default), and additionally only child nodes with
	/// a key greater than or equal to the given key.
	///
	/// <b>Known issue</b> This currently does not work properly on all platforms.
	/// Please use StartAt(Variant order_value) instead.
	///
	/// @param[in] order_value The lowest sort value the Query should include.
	/// @param[in] child_key The lowest key the Query should include.
	///
	/// @returns A Query in this same location, filtering out child nodes that
	/// have a lower sort value than the sort value specified, or a lower key than
	/// the key specified.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* StartAtKey(FFirebaseVariant OrderValue, const FString& ChildKey);

	/// @brief Get a Query constrained to nodes with the given sort value or
	/// lower.
	///
	/// This method is used to generate a reference to a limited view of the data
	/// at this location. The Query returned will only refer to child nodes with a
	/// value less than or equal to the given value, using the given OrderBy
	/// directive (or priority as default).
	///
	/// @param[in] order_value The highest sort value the Query should refer
	/// to.
	///
	/// @returns A Query in this same location, filtering out child nodes that
	/// have a higher sort value or key than the sort value or key specified.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* EndAt(const FFirebaseVariant& OrderValue);

	/// @brief Get a Query constrained to nodes with the given sort value or
	/// lower, and the given key or lower.
	///
	/// This method is used to generate a reference to a limited view of
	/// the data at this location. The Query returned will only refer to child
	/// nodes with a value less than or equal to the given value, using the given
	/// OrderBy directive (or priority as default), and additionally only child
	/// nodes with a key less than or equal to the given key.
	///
	/// <b>Known issue</b> This currently does not work properly on all platforms.
	/// Please use EndAt(Variant order_value) instead.
	///
	/// @param[in] order_value The highest sort value the Query should include.
	/// @param[in] child_key The highest key the Query should include.
	///
	/// @returns A Query in this same location, filtering out child nodes that
	/// have a higher sort value than the sort value specified, or a higher key
	/// than the key specified.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* EndAtKey(const FFirebaseVariant& OrderValue, const FString& ChildKey);

	/// @brief Get a Query constrained to nodes with the exact given sort value.
	///
	/// This method is used to create a query constrained to only return child
	/// nodes with the given value, using the given OrderBy directive (or priority
	/// as default).
	///
	/// @param[in] order_value The exact sort value the Query should include.
	///
	/// @returns A Query in this same location, filtering out child nodes that
	/// have a different sort value than the sort value specified.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* EqualTo(const FFirebaseVariant& OrderValue);

	/// @brief Get a Query constrained to nodes with the exact given sort value,
	/// and the exact given key.
	///
	/// This method is used to create a query constrained to only return the child
	/// node with the given value, using the given OrderBy directive (or priority
	/// as default), and the given key. Note that there is at most one such child
	/// as child key names are unique.
	///
	/// <b>Known issue</b> This currently does not work properly on iOS and
	/// desktop. Please use EqualTo(Variant order_value) instead.
	///
	/// @param[in] order_value The exact sort value the Query should include.
	/// @param[in] child_key The exact key the Query should include.
	///
	/// @returns A Query in this same location, filtering out child nodes that
	/// have a different sort value than the sort value specified, and containing
	/// at most one child with the exact key specified.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* EqualToKey(const FFirebaseVariant& OrderValue, const FString& ChildKey);

	/// @brief Gets a Query limited to only the first results.
	///
	/// Limits the query to reference only the first N child nodes, using the
	/// given OrderBy directive (or priority as default).
	///
	/// @param[in] limit Number of children to limit the Query to.
	///
	/// @returns A Query in this same location, limited to the specified number of
	/// children (taken from the beginning of the sorted list).
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* LimitToFirst(const int64 Limit);

	/// @brief Gets a Query limited to only the last results.
	///
	/// @param[in] limit Number of children to limit the Query to.
	///
	/// @returns A Query in this same location, limited to the specified number of
	/// children (taken from the end of the sorted list).
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	UPARAM(DisplayName = "Query") UDatabaseQuery* LimitToLast(const int64 Limit);

	/// @brief Returns true if this query is valid, false if it is not valid. An
	/// invalid query could be returned by, say, attempting to OrderBy two
	/// different items, or calling OrderByChild() with an empty path, or by
	/// constructing a Query with the default constructor. If a Query
	/// is invalid, attempting to add more constraints will also result in an
	/// invalid Query.
	///
	/// @returns true if this query is valid, false if this query is
	/// invalid.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Query")
	virtual UPARAM(DisplayName = "Is Valid") bool IsValid() const;

	/// @brief Sets whether this location's data should be kept in sync even if
	/// there are no active Listeners.
	///
	/// By calling SetKeepSynchronized(true) on a given database location, the
	/// data for that location will automatically be downloaded and kept in sync,
	/// even when no listeners are attached for that location. Additionally, while
	/// a location is kept synced, it will not be evicted from the persistent disk
	/// cache.
	///
	/// @param[in] bKeepSync If true, set this location to be synchronized. If
	/// false, set it to not be synchronized.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	void SetKeepSynchronized(const bool bKeepSync);

	/**
	 * Setups the child and value listeners. Events won't be called unless you
	 * call this function first.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	void SetupListeners();

	/**
	 * Removes the privously setup listeners.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Query")
	void ClearListeners();

protected:
#if WITH_FIREBASE_DATABASE
	virtual firebase::database::Query& GetQuery()
	{
		return Query;
	}

	virtual const firebase::database::Query& GetQuery() const
	{
		return Query;
	}

	firebase::database::Query Query;

	TUniquePtr<class FValueListener> ValueListener;
	TUniquePtr<class FChildListener> ChildListener;

#if WITH_EDITOR
	void RemoveListenersEndPIE(const bool);
#endif

private:
	// Creates a new query copied from the one passed as parameter.
	void SetQuery(const firebase::database::Query& InQuery);
#endif
};

/// DatabaseReference represents a particular location in your Database and can
/// be used for reading or writing data to that Database location.
/// This class is the starting point for all Database operations. After you've
/// initialized it with a URL, you can use it to read data, write data, and to
/// create new DatabaseReference instances.
UCLASS(BlueprintType)
class FIREBASEFEATURES_API UDatabaseReference final : public UDatabaseQuery
{
	GENERATED_BODY()
private:
	friend class UDatabase;
	friend class UDataSnapshot;
	friend class UDatabaseQuery;

public:
	UDatabaseReference();
	virtual ~UDatabaseReference();

	/// @brief Gets the database to which we refer.
	/// @returns Firebase Database instance that this DatabaseReference refers to.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "Database") UDatabase* GetDatabase() const;

	/// @brief Gets the string key of this database location.
	///
	/// The pointer is only valid while the DatabaseReference remains in memory.
	///
	/// @returns String key of this database location, which will remain valid in
	/// memory until the DatabaseReference itself goes away.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "Key") FString GetKey() const;

	/// @brief Returns true if this reference refers to the root of the database.
	///
	/// @returns true if this reference refers to the root of the database, false
	/// otherwise.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "Is Root") bool IsRoot() const;

	/// @brief Returns true if this reference is valid, false if it is not
	/// valid. DatabaseReferences constructed with the default constructor
	/// are considered invalid. An invalid reference could be returned by
	/// Database::GetReference() or Database::GetReferenceFromUrl() if you specify
	/// an incorrect location, or calling Query::GetReference() on an invalid
	/// query.
	///
	/// @returns true if this reference is valid, false if this reference is
	/// invalid.
	virtual bool IsValid() const;

	/// @brief Gets the parent of this location, or get this location again if
	/// IsRoot().
	///
	/// @returns Parent of this location in the database, unless this location is
	/// the root, in which case it returns this same location again.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "Reference") UDatabaseReference* GetParent() const;

	/// @brief Gets the root of the database.
	///
	/// @returns Root of the database.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "Reference") UDatabaseReference* GetRoot() const;

	/// @brief Gets a reference to a location relative to this one.
	/// @param[in] path Path relative to this snapshot's location.
	/// @returns Child relative to this location.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference", meta = (Keywords="get child children"))
	UPARAM(DisplayName = "Reference") UDatabaseReference* Child(const FString& Path) const;

	/// Gets a reference to a location relative to this one.
	/// Efficient method to travel across several children without creating a UE-based class for each child.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference", meta = (Keywords="get child children path"))
	UPARAM(DisplayName = "Reference") UDatabaseReference* ChildFromPaths(const TArray<FString>& Paths) const;

	/// @brief Automatically generates a child location, create a reference to it,
	/// and returns that reference to it.
	/// @returns A newly created child, with a unique key.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "Reference") UDatabaseReference* PushChild() const;

	/// @brief Removes the value at this location from the database.
	///
	/// This is an asynchronous operation which takes time to execute, and uses
	/// a callback to return its result.
	///
	/// @returns A Future result, which will complete when the operation either
	/// succeeds or fails. When the Future is completed, if its Error is
	/// kErrorNone, the operation succeeded.
	///
	/// @note Only one RemoveValue() should be running on a given database
	/// location at the same time. If you need to run multiple operations at once,
	/// use RunTransaction().
	void RemoveValue(const FDatabaseCallback& Callback);

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
	void RunTransaction(const bool bTriggerLocalEvents, const FTransactionCallback& TransactionFunction, const FSnapshotCallback& OnTransactionOver);

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
	void SetPriority(const FFirebaseVariant& Priority, const FDatabaseCallback& Callback);

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
	void SetValue(const FFirebaseVariant& Value, const FDatabaseCallback& Callback);

	/// @brief Sets both the data and priority of this location. See SetValue()
	/// and SetPriority() for context on the parameters.
	///
	/// This is an asynchronous operation which takes time to execute, and uses
	/// firebase::Future to return its result.
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
	void SetValueAndPriority(const FFirebaseVariant&  Value, const FFirebaseVariant& Priority, const FDatabaseCallback& Callback);

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
	void UpdateChildren(const FFirebaseVariant& Values, const FDatabaseCallback& Callback);

	/// @brief Get the absolute URL of this reference.
	///
	/// @returns The absolute URL of the location this reference refers to.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "URL") FString GetUrl() const;

	/// @brief Manually disconnect Firebase Realtime Database from the server, and
	/// disable automatic reconnection. This will affect all other instances of
	/// DatabaseReference as well.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference")
	void GoOffline();

	/// @brief Manually reestablish connection to the Firebase Realtime Database
	/// server and enable automatic reconnection. This will affect all other
	/// instances of DatabaseReference as well.
	UFUNCTION(BlueprintCallable, Category = "Firebase|Database|Reference")
	void GoOnline();

	/// @brief Get the disconnect handler, which controls what actions the server
	/// will perform to this location's data when this client disconnects.
	///
	/// @returns Disconnection handler for this location. You can use this to
	/// queue up operations on the server to be performed when the client
	/// disconnects.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Database|Reference")
	UPARAM(DisplayName = "Handler") UDisconnectionHandler* GetDisconnectionHandler();

protected:
#if WITH_FIREBASE_DATABASE
	virtual firebase::database::Query& GetQuery()
	{
		return Reference;
	}

	virtual const firebase::database::Query& GetQuery() const
	{
		return Reference;
	}

private:
	static firebase::database::TransactionResult DoTransactionWithContext(firebase::database::MutableData* data, void* context);

private:
	firebase::database::DatabaseReference Reference;
#endif
};

