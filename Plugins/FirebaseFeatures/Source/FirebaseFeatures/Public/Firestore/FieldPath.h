// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if !defined(WITH_FIREBASE_FIRESTORE)
#	define WITH_FIREBASE_FIRESTORE 0
#endif

#if WITH_FIREBASE_FIRESTORE
	THIRD_PARTY_INCLUDES_START
#		include "firebase/firestore/field_path.h"
	THIRD_PARTY_INCLUDES_END
#endif

#include "FieldPath.generated.h"

USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreFieldPath
{
	GENERATED_BODY()
public:

	/**
	 * Creates a FieldPath from the provided field names. If more than one field
	 * name is provided, the path will point to a nested field in a document.
	 *
	 * @param field_names A vector of field names.
	 */
	FFirestoreFieldPath();
	FFirestoreFieldPath(const FFirestoreFieldPath& Other);
	FFirestoreFieldPath(const FFirestoreFieldPath&& Other);

	FFirestoreFieldPath(const TArray<FString>& FieldNames);

	~FFirestoreFieldPath();

	FFirestoreFieldPath& operator=(const FFirestoreFieldPath& Other);

	/**
	 * A special sentinel FieldPath to refer to the ID of a document. It can be
	 * used in queries to sort or filter by the document ID.
	 */
	static FFirestoreFieldPath DocumentId();

	/**
	 * Returns a string representation of this `FieldPath` for
	 * logging/debugging purposes.
	 *
	 * @note the exact string representation is unspecified and subject to
	 * change; don't rely on the format of the string.
	 */
	FString ToString() const;

#if WITH_FIREBASE_FIRESTORE
	FORCEINLINE operator firebase::firestore::FieldPath& ()
	{
		return *Path;
	}

	FORCEINLINE operator firebase::firestore::FieldPath& () const
	{
		return *Path;
	}
#endif // WITH_FIREBASE_FIRESTORE


	friend uint32 FIREBASEFEATURES_API GetTypeHash(const FFirestoreFieldPath& Value);

private:
#if WITH_FIREBASE_FIRESTORE
	TUniquePtr<firebase::firestore::FieldPath> Path;
#endif // WITH_FIREBASE_FIRESTORE
};

// TMap support for FFirestoreFieldPath.
uint32 FIREBASEFEATURES_API GetTypeHash(const FFirestoreFieldPath& Value);
