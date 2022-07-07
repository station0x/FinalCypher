// Copyright Pandores Marketplace 2022. All Rights Reserved.

#include "FirebaseSdk/FirebaseVariant.h"

#include "FirebaseFeatures.h"

THIRD_PARTY_INCLUDES_START
#   include "firebase/variant.h"
#	if WITH_FIREBASE_DATABASE
#		include "firebase/database/common.h"
#	endif
THIRD_PARTY_INCLUDES_END

DECLARE_LOG_CATEGORY_CLASS(LogFirebaseVariant, Log, All);

#define FIREBASE_VARIANT_TYPE_ERROR(TypeName)						\
	UE_LOG															\
	(																\
		LogFirebaseVariant,											\
		Warning,													\
		TEXT("Tried to get a variant as \"") TEXT(#TypeName)		\
		TEXT("\" but it was a \"%s\""),								\
		*FVariantHelper::VariantTypeToString(GetType())				\
	);

FString FVariantHelper::VariantTypeToString(const EFirebaseVariantType& Type)
{
#define ___CASE(x) case EFirebaseVariantType:: x: return TEXT(#x);

	switch (Type)
	{
		___CASE(Null);
		___CASE(Int64);
		___CASE(Double);
		___CASE(Bool);
		___CASE(StaticString);
		___CASE(MutableString);
		___CASE(Vector);
		___CASE(Map);
		___CASE(StaticBlob);
		___CASE(MutableBlob);
	}

#undef ___CASE

	return TEXT("Unknown");
}

FFirebaseVariant FFirebaseVariant::ServerTimestamp()
{
#if WITH_FIREBASE_DATABASE
	return firebase::database::ServerTimestamp();
#else
	return {};
#endif
}

FFirebaseVariant::FFirebaseVariant()
{}

FFirebaseVariant::~FFirebaseVariant()
{}

FFirebaseVariant::FFirebaseVariant(FFirebaseVariant&& Other)
	: Variant(MoveTemp(Other.Variant))
{

}

FFirebaseVariant::FFirebaseVariant(const FFirebaseVariant& Other) : FFirebaseVariant()
{
	Variant = Other.Variant;
}

FFirebaseVariant::FFirebaseVariant(const firebase::Variant& InVariant) : FFirebaseVariant()
{
	Variant = InVariant;
}

FFirebaseVariant::FFirebaseVariant(const FString& Value) : FFirebaseVariant()
{
	Variant = TCHAR_TO_UTF8(*Value);
}

FFirebaseVariant::FFirebaseVariant(const TCHAR* Value) : FFirebaseVariant()
{
	Variant = TCHAR_TO_UTF8(Value);
}

FFirebaseVariant::FFirebaseVariant(const ANSICHAR* Value) : FFirebaseVariant()
{
	FString Temp(Value);
	Variant = TCHAR_TO_UTF8(*Temp);
}

FFirebaseVariant::FFirebaseVariant(const double& Value) : FFirebaseVariant()
{
	Variant = Value;
}

FFirebaseVariant::FFirebaseVariant(const float& Value) : FFirebaseVariant()
{
	Variant = Value;
}

FFirebaseVariant::FFirebaseVariant(const int64& Value) : FFirebaseVariant()
{
	Variant = static_cast<int64_t>(Value);
}

FFirebaseVariant::FFirebaseVariant(const int32& Value) : FFirebaseVariant()
{
	Variant = Value;
}

FFirebaseVariant::FFirebaseVariant(const bool bValue) : FFirebaseVariant()
{
	Variant = bValue;
}

FFirebaseVariant::FFirebaseVariant(const TArray<FFirebaseVariant>& Value) : FFirebaseVariant()
{
	typedef std::vector<firebase::Variant> TVariantVector;

	TVariantVector VariantVector;

	for (const auto& Val : Value)
	{
		VariantVector.push_back(Val.Variant);
	}

	(Variant).set_vector(VariantVector);
}

FFirebaseVariant::FFirebaseVariant(const TMap<FFirebaseVariant, FFirebaseVariant>& Value) : FFirebaseVariant()
{
	typedef std::map<firebase::Variant, firebase::Variant> TVariantMap;
	
	TVariantMap* VariantMap = new TVariantMap();

	for (const auto& Val : Value)
	{
		VariantMap->emplace(Val.Key.Variant, Val.Value.Variant);
	}

	Variant.AssignMap(&VariantMap);
}

bool FFirebaseVariant::IsNull() const
{
	return Variant.is_null();
}

bool FFirebaseVariant::IsString() const
{
	return Variant.is_string();
}

bool FFirebaseVariant::IsBinary()  const
{
	return Variant.is_blob();
}

bool FFirebaseVariant::AsBool() const
{
	if (!Variant.is_bool())
	{
		FIREBASE_VARIANT_TYPE_ERROR(bool);
		return false;
	}

	return Variant.bool_value();
}

int64 FFirebaseVariant::AsInt64() const
{
	if (Variant.is_double())
	{
		UE_LOG(LogFirebaseVariant, Warning, TEXT("Tried to get a double as int64. Value will be truncated."));
		return static_cast<int64>(Variant.double_value());
	}

	if (!Variant.is_int64())
	{
		FIREBASE_VARIANT_TYPE_ERROR(int64);
		return 0;
	}

	return (int64)Variant.int64_value();
}

int32 FFirebaseVariant::AsInt32() const
{
	if (Variant.is_double())
	{
		UE_LOG(LogFirebaseVariant, Warning, TEXT("Tried to get a double as int32. Value will be truncated."));
		return static_cast<int64>(Variant.double_value());
	}

	if (!Variant.is_int64())
	{
		FIREBASE_VARIANT_TYPE_ERROR(int64);
		return 0;
	}

	return (int64)Variant.int64_value();
}

double FFirebaseVariant::AsDouble() const
{
	if (Variant.is_int64())
	{
		return static_cast<double>(Variant.int64_value());
	}

	if (!Variant.is_double())
	{
		FIREBASE_VARIANT_TYPE_ERROR(double);
		return 0.;
	}

	return Variant.double_value();
}

float FFirebaseVariant::AsFloat() const
{
	if (Variant.is_int64())
	{
		return static_cast<float>(Variant.int64_value());
	}

	if (!Variant.is_double())
	{
		FIREBASE_VARIANT_TYPE_ERROR(float);
		return 0.f;
	}

	return (float)Variant.double_value();
}

FString FFirebaseVariant::AsString() const
{
	if (!Variant.is_string())
	{
		FIREBASE_VARIANT_TYPE_ERROR(FString);
		return TEXT("");
	}

	return UTF8_TO_TCHAR(Variant.string_value());
}

TMap<FFirebaseVariant, FFirebaseVariant> FFirebaseVariant::AsMap() const
{
	TMap<FFirebaseVariant, FFirebaseVariant> Variants;

	if (!Variant.is_map())
	{
		FIREBASE_VARIANT_TYPE_ERROR(TMap);
		return Variants;
	}

	for (const auto& Var : Variant.map())
	{
		Variants.Add(FFirebaseVariant(Var.first), FFirebaseVariant(Var.second));
	}

	return Variants;
}

TArray<uint8> FFirebaseVariant::AsBinary() const
{
	if (!Variant.is_blob())
	{
		FIREBASE_VARIANT_TYPE_ERROR(Binary);
		return TArray<uint8>();
	}

	return TArray<uint8>(Variant.blob_data(), Variant.blob_size());
}

TArray<FFirebaseVariant> FFirebaseVariant::AsArray() const
{
	TArray<FFirebaseVariant> Variants;

	if (!Variant.is_vector())
	{
		FIREBASE_VARIANT_TYPE_ERROR(TArray);
		return Variants;
	}

	for (const auto& Var : Variant.vector())
	{
		Variants.Emplace(FFirebaseVariant(Var));
	}

	return Variants;
}

uint64 GetTypeHash(const FFirebaseVariant& Var)
{
	// Containers not supported as keys.
	if (Var.Variant.is_container_type())
	{
		UE_LOG(LogFirebaseVariant, Warning, TEXT("Container as Map key detected. Containers can't be used as key type."));
		return 0xFFFFFFFFFFFFFFFF;
	}

	if (Var.Variant.is_string())
	{
		return FCrc::StrCrc32(*Var.AsString());
	}

	if (Var.Variant.is_blob())
	{
		return FCrc::MemCrc32(Var.Variant.blob_data(), Var.Variant.blob_size());
	}

	union
	{
		uint64  Uint64;
		int64   Int64;
		double  Double;
	} Converter;

	switch (Var.GetType())
	{
	case EFirebaseVariantType::Null:   Converter.Uint64 = 0xFFFFFFFFFFFFFFFE; break;
	case EFirebaseVariantType::Int64:  Converter.Int64  = static_cast<int64>(Var.AsInt64()); break;
	case EFirebaseVariantType::Bool:   Converter.Int64  = Var.AsBool() ? 0xFFFFFFFFFFFFFFFD : 0xFFFFFFFFFFFFFFFC; break;
	case EFirebaseVariantType::Double: Converter.Double = Var.AsDouble(); break;
	default:
		UE_LOG(LogFirebaseVariant, Error, TEXT("Unhandled Variant type for hashing: %s."), *FVariantHelper::VariantTypeToString(Var.GetType()));
		Converter.Int64 = 0;
	}

	return Converter.Uint64;
}

FFirebaseVariant UVariantLibrary::FromInt32(int32 Value)
{
	return Value;
}

FFirebaseVariant UVariantLibrary::FromInt64(int64 Value)
{
	return Value;
}

FFirebaseVariant UVariantLibrary::FromFloat(float Value)
{
	return Value;
}

FFirebaseVariant UVariantLibrary::FromString(const FString& Value)
{
	return Value;
}

FFirebaseVariant UVariantLibrary::FromMap(const TMap<FFirebaseVariant, FFirebaseVariant>& Value)
{
	return FFirebaseVariant(Value);
}

FFirebaseVariant UVariantLibrary::FromBool(const bool bValue)
{
	return bValue;
}

FFirebaseVariant UVariantLibrary::FromNull()
{
	return FFirebaseVariant(firebase::Variant::Null());
}

FFirebaseVariant UVariantLibrary::FromArray(const TArray<FFirebaseVariant>& Value)
{
	return Value;
}

FString UVariantLibrary::ToString(const FFirebaseVariant& Value)
{
	return Value.AsString();
}

bool UVariantLibrary::ToBool(const FFirebaseVariant& Value)
{
	return Value.AsBool();
}

int32 UVariantLibrary::ToInt32(const FFirebaseVariant& Value)
{
	return Value.AsInt32();
}

float UVariantLibrary::ToFloat(const FFirebaseVariant& Value)
{
	return (float)Value.AsDouble();
}

int64 UVariantLibrary::ToInt64(const FFirebaseVariant& Value)
{
	return Value.AsInt64();
}

TArray<uint8> UVariantLibrary::ToBinary(const FFirebaseVariant& Value)
{
	return Value.AsBinary();
}

TMap<FFirebaseVariant, FFirebaseVariant> UVariantLibrary::ToMap(const FFirebaseVariant& Value)
{
	return Value.AsMap();
}

TArray<FFirebaseVariant> UVariantLibrary::ToArray(const FFirebaseVariant& Value)
{
	return Value.AsArray();
}

#undef FIREBASE_VARIANT_TYPE_ERROR

EFirebaseVariantType FFirebaseVariant::GetType() const
{
	return static_cast<EFirebaseVariantType>(Variant.type());
}

FFirebaseVariant& FFirebaseVariant::operator=  (const FFirebaseVariant& Other)
{
	Variant = Other.Variant;
	return *this;
}

FFirebaseVariant& FFirebaseVariant::operator=  (FFirebaseVariant&& Other)
{
	Variant = MoveTemp(Other.Variant);
	return *this;
}

bool FFirebaseVariant::operator== (const FFirebaseVariant& Other) const
{
	return Variant == Other.Variant;
}

bool FFirebaseVariant::operator!= (const FFirebaseVariant& Other) const
{
	return Variant != Other.Variant;
}

bool FFirebaseVariant::operator<= (const FFirebaseVariant& Other) const
{
	return Variant <= Other.Variant;
}

bool FFirebaseVariant::operator>= (const FFirebaseVariant& Other) const
{
	return Variant >= Other.Variant;
}

bool FFirebaseVariant::operator> (const FFirebaseVariant& Other) const
{
	return Variant > Other.Variant;
}

bool FFirebaseVariant::operator< (const FFirebaseVariant& Other) const
{
	return Variant < Other.Variant;
}

FFirebaseVariant UVariantLibrary::FromStringMap(const TMap<FString, FFirebaseVariant>& Value)
{
	TMap<FFirebaseVariant, FFirebaseVariant> Converted;
	Converted.Reserve(Value.Num());

	for (const auto& Val : Value)
	{
		Converted.Add(Val.Key, Val.Value);
	}

	return Converted;
}
