// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

#if !defined(WITH_FIREBASE_FIRESTORE)
#	define WITH_FIREBASE_FIRESTORE 0
#endif 

#if WITH_FIREBASE_FIRESTORE
	THIRD_PARTY_INCLUDES_START
#		include "firebase/firestore/field_value.h"
	THIRD_PARTY_INCLUDES_END
#endif // WITH_FIREBASE_FIRESTORE

#include "FieldValue.generated.h"

class UFirestoreDocumentReference;

UENUM(BlueprintType)
enum class EFirestoreFieldValueType : uint8
{
	Null,
	Boolean,
	Integer,
	Double,
	Timestamp,
	String,
	Blob,
	Reference,
	GeoPoint,
	Array,
	Map,
};

/**
 * A Timestamp represents a point in time independent of any time zone or
 * calendar, represented as seconds and fractions of seconds at nanosecond
 * resolution in UTC Epoch time. It is encoded using the Proleptic Gregorian
 * Calendar which extends the Gregorian calendar backwards to year one. It is
 * encoded assuming all minutes are 60 seconds long, i.e. leap seconds are
 * "smeared" so that no leap second table is needed for interpretation. Range is
 * from 0001-01-01T00:00:00Z to 9999-12-31T23:59:59.999999999Z.
 *
 * @see
 * https://github.com/google/protobuf/blob/master/src/google/protobuf/timestamp.proto
 */
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreTimestamp
{
	GENERATED_BODY()
public:
	FFirestoreTimestamp();
	FFirestoreTimestamp(int64 InSeconds, int32 InNanoseconds);


	/**
	 * The number of seconds of UTC time since Unix epoch 1970-01-01T00:00:00Z.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timestamp")
	int64 Seconds;

	/**
	 * The non-negative fractions of a second at nanosecond resolution. Negative
	 * second values with fractions still have non-negative nanoseconds values
	 * that count forward in time.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Timestamp")
	int32 Nanoseconds;

public:
	static FFirestoreTimestamp Now();

	/**
	 * Converts this timestamp to a date time, losing nanoseconds value in the process.
	 * @return A date time.
	*/
	FDateTime ToDateTime() const;

	/**
	 * Converts this timestamp to a human-readable string.
	 * @return A human-readable string
	*/
	FString ToString() const;
};

/**
 * An immutable object representing a geographical point in Firestore. The point
 * is represented as a latitude/longitude pair.
 *
 * Latitude values are in the range of [-90, 90].
 * Longitude values are in the range of [-180, 180].
 */
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreGeoPoint
{
	GENERATED_BODY()
public:
	FFirestoreGeoPoint();
	FFirestoreGeoPoint(float InLatitude, float InLongitude);

	/** The latitude value of this `GeoPoint`. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoPoint")
	float Latitude;
	
	/** The latitude value of this `GeoPoint`. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GeoPoint")
	float Longitude;
};

/**
 * A field value represents variant datatypes as stored by Firestore.
 *
 * FieldValue can be used when reading a particular field with
 * DocumentSnapshot::Get() or fields with DocumentSnapshot::GetData(). When
 * writing document fields with DocumentReference::Set() or
 * DocumentReference::Update(), it can also represent sentinel values in
 * addition to real data values.
 */
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirestoreFieldValue
{
	GENERATED_BODY()
public:
	FFirestoreFieldValue();
	FFirestoreFieldValue(const FFirestoreFieldValue& Other);

#if WITH_FIREBASE_FIRESTORE
	FFirestoreFieldValue(const firebase::firestore::FieldValue& Value);
#endif // WITH_FIREBASE_FIRESTORE

	FFirestoreFieldValue(const bool  bValue);
	FFirestoreFieldValue(const int32  Value);
	FFirestoreFieldValue(const int64  Value);
	FFirestoreFieldValue(const float  Value);
	FFirestoreFieldValue(const double Value);

	FFirestoreFieldValue(const TCHAR*		  Value);
	FFirestoreFieldValue(const FString&		  Value);
	FFirestoreFieldValue(const TArray<uint8>& Value);

	FFirestoreFieldValue(const FFirestoreGeoPoint&	Value);
	FFirestoreFieldValue(const FFirestoreTimestamp& Value);

	FFirestoreFieldValue(const TArray<FFirestoreFieldValue>&		Value);
	FFirestoreFieldValue(const TMap<FString, FFirestoreFieldValue>& Value);
	FFirestoreFieldValue(UFirestoreDocumentReference* const			Value);

	~FFirestoreFieldValue();
	
	FFirestoreFieldValue& operator=(const FFirestoreFieldValue& Other);

	EFirestoreFieldValueType GetType() const;

#if WITH_FIREBASE_FIRESTORE
	FORCEINLINE operator firebase::firestore::FieldValue& ()
	{
		return *FieldValue.Get();
	}

	FORCEINLINE operator firebase::firestore::FieldValue& () const
	{
		return *FieldValue.Get();
	}
#endif  // WITH_FIREBASE_FIRESTORE

	operator int32()   const;
	operator int64()   const;
	operator bool()    const;
	operator float()   const;
	operator double()  const;
	operator FString() const;
	operator FFirestoreGeoPoint() const;
	operator FFirestoreTimestamp() const;

	operator TArray<uint8>()						const;
	operator TArray<FFirestoreFieldValue>()			const;
	operator TMap<FString, FFirestoreFieldValue>()	const;
	operator UFirestoreDocumentReference* ()		const;

public:
	bool    IsNull()   const;
	bool    ToBool()   const;
	double  ToDouble() const;
	float   ToFloat()  const;
	int64   ToInt64()  const;
	int32   ToInt32()  const;
	FString ToString() const;

	FFirestoreGeoPoint ToGeoPoint() const;
	FFirestoreTimestamp ToTimestamp() const;

	TArray<uint8> ToBinary() const;
	UFirestoreDocumentReference*		ToDocumentReference()	const;
	TMap<FString, FFirestoreFieldValue> ToMap()					const;
	TArray<FFirestoreFieldValue>		ToArray()				const;

	/**
	 * @brief Returns a sentinel for use with Update() to mark a field for
	 * deletion.
	 */
	static FFirestoreFieldValue Delete();

	/**
	 * Returns a sentinel that can be used with Set() or Update() to include
	 * a server-generated timestamp in the written data.
	 */
	static FFirestoreFieldValue ServerTimestamp();
	
	/** @brief Constructs a null. */
	static FFirestoreFieldValue Null();

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
	static FFirestoreFieldValue ArrayUnion(const TArray<FFirestoreFieldValue>& Elements);

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
	static FFirestoreFieldValue ArrayRemove(const TArray<FFirestoreFieldValue>& Elements);

private:
#if WITH_FIREBASE_FIRESTORE
	void SetValue(const firebase::firestore::FieldValue& Value);
	TUniquePtr<firebase::firestore::FieldValue> FieldValue;
#endif
};


