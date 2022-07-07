
#include "Firestore/FieldValue.h"
#include "Firestore/DocumentReference.h"

#if WITH_FIREBASE_FIRESTORE
	THIRD_PARTY_INCLUDES_START
#		include "firebase/variant.h"
#		include "firebase/firestore/geo_point.h"
#		include "firebase/firestore/document_reference.h"
#		include "firebase/firestore/timestamp.h"
	THIRD_PARTY_INCLUDES_END
#endif // WITH_FIREBASE_FIRESTORE


FFirestoreGeoPoint::FFirestoreGeoPoint()
	: Latitude(0.f)
	, Longitude(0.f)
{
}

FFirestoreGeoPoint::FFirestoreGeoPoint(float InLatitude, float InLongitude)
	: Latitude(InLatitude)
	, Longitude(InLongitude)
{
}

FFirestoreTimestamp::FFirestoreTimestamp()
	: Seconds(0)
	, Nanoseconds(0)
{
}

FFirestoreTimestamp::FFirestoreTimestamp(int64 InSeconds, int32 InNanoseconds)
	: Seconds(InSeconds)
	, Nanoseconds(InNanoseconds)
{
}

FFirestoreTimestamp FFirestoreTimestamp::Now()
{
#if WITH_FIREBASE_FIRESTORE
	auto Time = firebase::Timestamp::Now();
	return FFirestoreTimestamp(Time.seconds(), Time.nanoseconds());
#else
	return FFirestoreTimestamp();
#endif
}

FDateTime FFirestoreTimestamp::ToDateTime() const
{
	return FDateTime::FromUnixTimestamp(Seconds);
}

FString FFirestoreTimestamp::ToString() const
{
	return FString::Printf(TEXT("%s:%d"), *ToDateTime().ToString(), Nanoseconds);
}

FFirestoreFieldValue::FFirestoreFieldValue()
#if WITH_FIREBASE_FIRESTORE
	: FieldValue(MakeUnique<firebase::firestore::FieldValue>())
#endif // WITH_FIREBASE_FIRESTORE
{
}
FFirestoreFieldValue::FFirestoreFieldValue(const FFirestoreFieldValue& Other) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	*FieldValue = *Other.FieldValue;
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue& FFirestoreFieldValue::operator=(const FFirestoreFieldValue& Other)
{
#if WITH_FIREBASE_FIRESTORE
	*FieldValue.Get() = *Other.FieldValue.Get();
#endif // WITH_FIREBASE_FIRESTORE
	return *this;
}

FFirestoreFieldValue::~FFirestoreFieldValue()
{
}

FFirestoreFieldValue::operator TArray<uint8>() const
{
#if WITH_FIREBASE_FIRESTORE
	return FieldValue->is_blob() ?
		TArray<uint8>(FieldValue->blob_value(), FieldValue->blob_size()) :
		TArray<uint8>();
#else
	return	TArray<uint8>();
#endif // WITH_FIREBASE_FIRESTORE

}

FFirestoreFieldValue::operator TArray<FFirestoreFieldValue>() const
{
	TArray<FFirestoreFieldValue> Array;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue->is_array())
	{
		const auto& ArrayRaw = FieldValue->array_value();

		for (const auto& ArrayElem : ArrayRaw)
		{
			Array.Emplace(ArrayElem);
		}
	}
#endif // WITH_FIREBASE_FIRESTORE

	return Array;
}

FFirestoreFieldValue::operator TMap<FString, FFirestoreFieldValue>() const
{
	TMap<FString, FFirestoreFieldValue> Map;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue->is_map())
	{
		const auto& MapRaw = FieldValue->map_value();

		for (const auto& MapElem : MapRaw)
		{
			Map.Emplace(UTF8_TO_TCHAR(MapElem.first.c_str()), MapElem.second);
		}
	}
#endif // WITH_FIREBASE_FIRESTORE

	return Map;
}

FFirestoreFieldValue::operator UFirestoreDocumentReference* () const
{
#if WITH_FIREBASE_FIRESTORE
	if (!FieldValue->is_reference())
	{
		return nullptr;
	}

	UFirestoreDocumentReference* const Reference = NewObject<UFirestoreDocumentReference>();

	*Reference->Reference = FieldValue->reference_value();

	return Reference;
#else
	return nullptr;
#endif // WITH_FIREBASE_FIRESTORE
}

#if WITH_FIREBASE_FIRESTORE
FFirestoreFieldValue::FFirestoreFieldValue(const firebase::firestore::FieldValue& Value) : FFirestoreFieldValue()
{
	*FieldValue = Value;
}
#endif // WITH_FIREBASE_FIRESTORE

FFirestoreFieldValue::FFirestoreFieldValue(const bool bValue) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Boolean(bValue));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const int32  Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Integer((int64_t)Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const int64  Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Integer(Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const float  Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Double((float)Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const double Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Double(Value));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const TCHAR* Value) : FFirestoreFieldValue(FString(Value))
{
}

FFirestoreFieldValue::FFirestoreFieldValue(const FString& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::String(TCHAR_TO_UTF8(*Value)));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const TArray<uint8>& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Blob(Value.GetData(), Value.Num()));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const TArray<FFirestoreFieldValue>& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	std::vector<firebase::firestore::FieldValue> Values;

	Values.reserve(Value.Num());

	for (const auto& Val : Value)
	{
		Values.push_back(Val);
	}

	SetValue(firebase::firestore::FieldValue::Array(Values));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(const FFirestoreGeoPoint& Value)
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::GeoPoint(firebase::firestore::GeoPoint(Value.Latitude, Value.Longitude)));
#endif
}

FFirestoreFieldValue::FFirestoreFieldValue(const FFirestoreTimestamp& Value)
{
#if WITH_FIREBASE_FIRESTORE
	SetValue(firebase::firestore::FieldValue::Timestamp(firebase::Timestamp(Value.Seconds, Value.Nanoseconds)));
#endif
}

FFirestoreFieldValue::FFirestoreFieldValue(const TMap<FString, FFirestoreFieldValue>& Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	std::unordered_map<std::string, firebase::firestore::FieldValue> Values;

	Values.reserve(Value.Num());

	for (const auto& Val : Value)
	{
		Values.emplace(TCHAR_TO_UTF8(*Val.Key), Val.Value);
	}

	SetValue(firebase::firestore::FieldValue::Map(Values));
#endif // WITH_FIREBASE_FIRESTORE
}

FFirestoreFieldValue::FFirestoreFieldValue(UFirestoreDocumentReference* const Value) : FFirestoreFieldValue()
{
#if WITH_FIREBASE_FIRESTORE
	if (Value)
	{
		SetValue(firebase::firestore::FieldValue::Reference(*Value->GetInternal()));
	}
#endif // WITH_FIREBASE_FIRESTORE
}

#if WITH_FIREBASE_FIRESTORE
void FFirestoreFieldValue::SetValue(const firebase::firestore::FieldValue& Value)
{
	*FieldValue.Get() = Value;
}
#endif // WITH_FIREBASE_FIRESTORE

EFirestoreFieldValueType FFirestoreFieldValue::GetType() const
{
#if WITH_FIREBASE_FIRESTORE
	return (EFirestoreFieldValueType)FieldValue->type();
#else
	return EFirestoreFieldValueType::Null;
#endif
}

#if WITH_FIREBASE_FIRESTORE
#	define RETURN_TYPE(TypeName, DefaultValue) \
		return FieldValue->is_ ## TypeName () ? FieldValue-> TypeName ## _value() : DefaultValue
#else
#	define RETURN_TYPE(TypeName, DefaultValue) return DefaultValue
#endif  // WITH_FIREBASE_FIRESTORE

FFirestoreFieldValue::operator int32() const
{
	RETURN_TYPE(integer, 0);
}

FFirestoreFieldValue::operator int64() const
{
	RETURN_TYPE(integer, 0);
}

FFirestoreFieldValue::operator bool() const
{
	RETURN_TYPE(boolean, false);
}

FFirestoreFieldValue::operator float() const
{
	RETURN_TYPE(double, 0.f);
}

FFirestoreFieldValue::operator double() const
{
	RETURN_TYPE(double, 0.);
}

#undef RETURN_TYPE

FFirestoreFieldValue::operator FFirestoreGeoPoint() const
{
	FFirestoreGeoPoint GeoPoint;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue->is_geo_point())
	{
		auto Point = FieldValue->geo_point_value();

		GeoPoint.Latitude  = (float)Point.latitude();
		GeoPoint.Longitude = (float)Point.longitude();
	}
	else
	{
		UE_LOG(LogFirestore, Warning, TEXT("Tried to get a field value as Geo Point but it isn't one."));
	}
#endif

	return GeoPoint;
}

FFirestoreFieldValue::operator FFirestoreTimestamp() const
{
	FFirestoreTimestamp Timestamp;

#if WITH_FIREBASE_FIRESTORE
	if (FieldValue->is_timestamp())
	{
		auto Time = FieldValue->timestamp_value();

		Timestamp.Seconds     = Time.seconds();
		Timestamp.Nanoseconds = Time.nanoseconds();
	}
#endif

	return Timestamp;
}

FFirestoreFieldValue::operator FString() const
{
#if WITH_FIREBASE_FIRESTORE
	return FieldValue->is_string() ? UTF8_TO_TCHAR(FieldValue->string_value().c_str()) : TEXT("");
#else
	return TEXT("FIRESTORE_DISABLED");
#endif
}

bool FFirestoreFieldValue::ToBool() const
{
	return *this;
}

double FFirestoreFieldValue::ToDouble() const
{
	return *this;
}

float FFirestoreFieldValue::ToFloat() const
{
	return *this;
}

int64 FFirestoreFieldValue::ToInt64() const
{
	return *this;
}

int32 FFirestoreFieldValue::ToInt32() const
{
	return *this;
}

FString FFirestoreFieldValue::ToString() const
{
	return *this;
}

TArray<uint8> FFirestoreFieldValue::ToBinary() const
{
	return *this;
}

UFirestoreDocumentReference* FFirestoreFieldValue::ToDocumentReference() const
{
	return *this;
}

TMap<FString, FFirestoreFieldValue> FFirestoreFieldValue::ToMap() const
{
	return *this;
}

TArray<FFirestoreFieldValue> FFirestoreFieldValue::ToArray() const
{
	return *this;
}

FFirestoreGeoPoint FFirestoreFieldValue::ToGeoPoint() const
{
	return *this;
}

FFirestoreTimestamp FFirestoreFieldValue::ToTimestamp() const
{
	return *this;
}

bool    FFirestoreFieldValue::IsNull()   const
{
#if WITH_FIREBASE_FIRESTORE
	return FieldValue->is_null();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::Delete()
{
#if WITH_FIREBASE_FIRESTORE
	return firebase::firestore::FieldValue::Delete();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::ServerTimestamp()
{
#if WITH_FIREBASE_FIRESTORE
	return firebase::firestore::FieldValue::ServerTimestamp();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::Null()
{
#if WITH_FIREBASE_FIRESTORE
	return firebase::firestore::FieldValue::Null();
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::ArrayUnion(const TArray<FFirestoreFieldValue>& Elements)
{
#if WITH_FIREBASE_FIRESTORE
	std::vector<firebase::firestore::FieldValue> elements;

	elements.reserve(Elements.Num());

	for (const FFirestoreFieldValue& Value : Elements)
	{
		elements.push_back(*Value.FieldValue);
	}

	return firebase::firestore::FieldValue::ArrayUnion(MoveTemp(elements));
#else 
	return FFirestoreFieldValue();
#endif
}

FFirestoreFieldValue FFirestoreFieldValue::ArrayRemove(const TArray<FFirestoreFieldValue>& Elements)
{
#if WITH_FIREBASE_FIRESTORE
	std::vector<firebase::firestore::FieldValue> elements;

	elements.reserve(Elements.Num());

	for (const FFirestoreFieldValue& Value : Elements)
	{
		elements.push_back(*Value.FieldValue);
	}

	return firebase::firestore::FieldValue::ArrayRemove(MoveTemp(elements));
#else 
	return FFirestoreFieldValue();
#endif
}


