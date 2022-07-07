// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if !defined(WITH_FIREBASE_FIRESTORE)
#	define WITH_FIREBASE_FIRESTORE 0
#endif

#if WITH_FIREBASE_FIRESTORE 
THIRD_PARTY_INCLUDES_START
#	include "firebase/firestore/query.h"
#	include "firebase/firestore/listener_registration.h"
THIRD_PARTY_INCLUDES_END
#endif // WITH_FIREBASE_FIRESTORE 

#include "Firestore/DocumentSnapshot.h"
#include "Firestore/FieldValue.h"
#include "Firestore/FieldPath.h"
#include "Firestore/Firestore.h"
#include "Query.generated.h"

DECLARE_DELEGATE_ThreeParams(FFirestoreQueryCallback, EFirestoreError, TArray<FFirestoreDocumentSnapshot>, TArray<class UFirestoreDocumentChange*>);

class UFirestoreDocumentChange;

UDELEGATE()
DECLARE_DYNAMIC_DELEGATE_ThreeParams(
	FQuerySnapshotListener, 
		const EFirestoreError, Error, 
		const TArray<FFirestoreDocumentSnapshot>&, DocumentSnapshots,
		const TArray<UFirestoreDocumentChange*>&, DocumentChanges
);

DECLARE_DELEGATE_ThreeParams(
	FQuerySnapshotListenerCallback, 
		const EFirestoreError, 
		const TArray<FFirestoreDocumentSnapshot>&,
		const TArray<UFirestoreDocumentChange*>&
);

UENUM(BlueprintType)
enum class EFirestoreQueryDirection : uint8
{
	Ascending,
	Descending,
};

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FQuerySnapshotListenerHandle
{
	GENERATED_BODY()
public:
	/**
	 * Constructs an invalid listener handle.
	*/
	FQuerySnapshotListenerHandle() = default;

	/**
	 * Copies the listener. Calling Remove() on a copy will remove
	 * the listener on the original as well.
	*/
	FQuerySnapshotListenerHandle(const FQuerySnapshotListenerHandle&);
	FQuerySnapshotListenerHandle(FQuerySnapshotListenerHandle&&);

	FQuerySnapshotListenerHandle& operator=(FQuerySnapshotListenerHandle&&);
	FQuerySnapshotListenerHandle& operator=(const FQuerySnapshotListenerHandle&);

#if WITH_FIREBASE_FIRESTORE
	FQuerySnapshotListenerHandle(firebase::firestore::ListenerRegistration&& InListener);
#endif

	/**
	 * Removes this listener.
	*/
	void Remove();

private:
#if WITH_FIREBASE_FIRESTORE 
	firebase::firestore::ListenerRegistration Listener;
#endif
};

UCLASS(BlueprintType)
class FIREBASEFEATURES_API UFirestoreQuery : public UObject
{
	GENERATED_BODY()
private:
	friend class UFirestore;

public:
	UFirestoreQuery();
	~UFirestoreQuery();

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be equal to
	 * the specified value.
	 *
	 * @param Field The name of the field to compare.
	 * @param Value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereEqualTo(const FString& Field, const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be equal to
	 * the specified value.
	 *
	 * @param Field The path of the field to compare.
	 * @param Value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereEqualToFieldPath(const FFirestoreFieldPath& Field, const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be less
	 * than the specified value.
	 *
	 * @param[in] field The name of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereLessThan(const FString& Field, const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be less
	 * than the specified value.
	 *
	 * @param[in] field The path of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereLessThanFieldPath(const FFirestoreFieldPath& Field, const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be less
	 * than or equal to the specified value.
	 *
	 * @param[in] field The name of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereLessThanOrEqualTo(const FString& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be less
	 * than or equal to the specified value.
	 *
	 * @param[in] field The path of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereLessThanOrEqualToFieldPath(const FFirestoreFieldPath& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be greater
	 * than the specified value.
	 *
	 * @param[in] field The name of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereGreaterThan(const FString& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be greater
	 * than the specified value.
	 *
	 * @param[in] field The path of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereGreaterThanFieldPath(const FFirestoreFieldPath& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be greater
	 * than or equal to the specified value.
	 *
	 * @param[in] field The name of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereGreaterThanOrEqualTo(const FString& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value should be greater
	 * than or equal to the specified value.
	 *
	 * @param[in] field The path of the field to compare.
	 * @param[in] value The value for comparison.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereGreaterThanOrEqualToFielPath(const FFirestoreFieldPath& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field, the value must be an array, and
	 * that the array must contain the provided value.
	 *
	 * A Query can have only one `WhereArrayContains()` filter and it cannot be
	 * combined with `WhereArrayContainsAny()` or `WhereIn()`.
	 *
	 * @param[in] field The name of the field containing an array to search.
	 * @param[in] value The value that must be contained in the array.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereArrayContains(const FString& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field, the value must be an array, and
	 * that the array must contain the provided value.
	 *
	 * A Query can have only one `WhereArrayContains()` filter and it cannot be
	 * combined with `WhereArrayContainsAny()` or `WhereIn()`.
	 *
	 * @param[in] field The path of the field containing an array to search.
	 * @param[in] value The value that must be contained in the array.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereArrayContainsFieldPath(const FFirestoreFieldPath& Field,
		const FFirestoreFieldValue& Value);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field, the value must be an array, and
	 * that the array must contain at least one value from the provided list.
	 *
	 * A Query can have only one `WhereArrayContainsAny()` filter and it cannot be
	 * combined with `WhereArrayContains()` or `WhereIn()`.
	 *
	 * @param[in] field The name of the field containing an array to search.
	 * @param[in] values The list that contains the values to match.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereArrayContainsAny(const FString& Field,
		const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field, the value must be an array, and
	 * that the array must contain at least one value from the provided list.
	 *
	 * A Query can have only one `WhereArrayContainsAny()` filter and it cannot be
	 * combined with` WhereArrayContains()` or `WhereIn()`.
	 *
	 * @param[in] field The path of the field containing an array to search.
	 * @param[in] values The list that contains the values to match.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereArrayContainsAnyFielPath(const FFirestoreFieldPath& Field,
		const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value must equal one of
	 * the values from the provided list.
	 *
	 * A Query can have only one `WhereIn()` filter and it cannot be
	 * combined with `WhereArrayContainsAny()`.
	 *
	 * @param[in] field The name of the field containing an array to search.
	 * @param[in] values The list that contains the values to match.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereIn(const FString& Field,
		const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Creates and returns a new Query with the additional filter that
	 * documents must contain the specified field and the value must equal one of
	 * the values from the provided list.
	 *
	 * A Query can have only one `WhereIn()` filter and it cannot be
	 * combined with `WhereArrayContainsAny()`.
	 *
	 * @param[in] field The path of the field containing an array to search.
	 * @param[in] values The list that contains the values to match.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* WhereInFieldPath(const FFirestoreFieldPath& Field,
		const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Creates and returns a new Query that's additionally sorted by the
	 * specified field.
	 *
	 * @param[in] field The field to sort by.
	 * @param[in] direction The direction to sort (optional). If not specified,
	 * order will be ascending.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* OrderBy(const FString& Field,
		EFirestoreQueryDirection Direction = EFirestoreQueryDirection::Ascending);

	/**
	 * Creates and returns a new Query that's additionally sorted by the
	 * specified field.
	 *
	 * @param[in] field The field to sort by.
	 * @param[in] direction The direction to sort (optional). If not specified,
	 * order will be ascending.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* OrderByFieldPath(const FFirestoreFieldPath& Field,
		EFirestoreQueryDirection Direction = EFirestoreQueryDirection::Ascending);

	/**
	 * Creates and returns a new Query that only returns the first matching
	 * documents up to the specified number.
	 *
	 * @param[in] limit A non-negative integer to specify the maximum number of
	 * items to return.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* Limit(int32 Limit);

	/**
	 * Creates and returns a new Query that only returns the last matching
	 * documents up to the specified number.
	 *
	 * @param[in] limit A non-negative integer to specify the maximum number of
	 * items to return.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* LimitToLast(int32 Limit);

	/**
	 * Creates and returns a new Query that starts at the provided document
	 * (inclusive). The starting position is relative to the order of the query.
	 * The document must contain all of the fields provided in the order by of
	 * this query.
	 *
	 * @param[in] snapshot The snapshot of the document to start at.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* StartAt(const FFirestoreDocumentSnapshot& Snapshot);

	/**
	 * Creates and returns a new Query that starts at the provided fields
	 * relative to the order of the query. The order of the field values must
	 * match the order of the order by clauses of the query.
	 *
	 * @param[in] values The field values to start this query at, in order of the
	 * query's order by.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* StartAtFieldValue(const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Creates and returns a new Query that starts after the provided
	 * document (inclusive). The starting position is relative to the order of the
	 * query. The document must contain all of the fields provided in the order by
	 * of this query.
	 *
	 * @param[in] snapshot The snapshot of the document to start after.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* StartAfter(const FFirestoreDocumentSnapshot& Snapshot);

	/**
	 * Creates and returns a new Query that starts after the provided
	 * fields relative to the order of the query. The order of the field values
	 * must match the order of the order by clauses of the query.
	 *
	 * @param[in] values The field values to start this query after, in order of
	 * the query's order by.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* StartAfterFieldValue(const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Creates and returns a new Query that ends before the provided
	 * document (inclusive). The end position is relative to the order of the
	 * query. The document must contain all of the fields provided in the order by
	 * of this query.
	 *
	 * @param[in] snapshot The snapshot of the document to end before.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* EndBefore(const FFirestoreDocumentSnapshot& Snapshot);

	/**
	 * Creates and returns a new Query that ends before the provided fields
	 * relative to the order of the query. The order of the field values must
	 * match the order of the order by clauses of the query.
	 *
	 * @param[in] values The field values to end this query before, in order of
	 * the query's order by.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* EndBeforeFieldValue(const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Creates and returns a new Query that ends at the provided document
	 * (inclusive). The end position is relative to the order of the query. The
	 * document must contain all of the fields provided in the order by of this
	 * query.
	 *
	 * @param[in] snapshot The snapshot of the document to end at.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* EndAt(const FFirestoreDocumentSnapshot& Snapshot);

	/**
	 * Creates and returns a new Query that ends at the provided fields
	 * relative to the order of the query. The order of the field values must
	 * match the order of the order by clauses of the query.
	 *
	 * @param[in] values The field values to end this query at, in order of the
	 * query's order by.
	 *
	 * @return The created Query.
	 */
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Query") UFirestoreQuery* EndAtValues(const TArray<FFirestoreFieldValue>& Values);

	/**
	 * Executes the query and returns the results as a QuerySnapshot.
	 *
	 * By default, Get() attempts to provide up-to-date data when possible by
	 * waiting for data from the server, but it may return cached data or fail if
	 * you are offline and the server cannot be reached. This behavior can be
	 * altered via the Source parameter.
	 *
	 * @param Source A value to configure the get behavior (optional).
	 */
	void Get(const EFirestoreSource Source, FFirestoreQueryCallback Callback) const;
	void Get(FFirestoreQueryCallback Callback) const;

	/**
	 * Adds a snapshot listener for this query.
	 * @param Listener The listener.
	 * @return An handle to remove the listener.
	*/
	UFUNCTION(BlueprintCallable, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Handle") FQuerySnapshotListenerHandle AddSnapshotListener(FQuerySnapshotListener Listener);
	FQuerySnapshotListenerHandle AddSnapshotListener(FQuerySnapshotListenerCallback Callback);

	UFUNCTION(BlueprintPure, Category = "Firebase|Firestore|Query")
	UPARAM(DisplayName = "Is Valid") bool IsValid() const;

protected:

#if WITH_FIREBASE_FIRESTORE 
	TUniquePtr<firebase::firestore::Query> Reference;
#endif 
};


