// Copyright Pandores Marketplace 2022. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"

THIRD_PARTY_INCLUDES_START
#   include "firebase/variant.h"
THIRD_PARTY_INCLUDES_END

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FirebaseVariant.generated.h"

/// Type of data that this variant object contains.
UENUM(BlueprintType)
enum class EFirebaseVariantType  : uint8
{
    /// Null, or no data.
    Null,
    /// A 64-bit integer.
    Int64,
    /// A double-precision floating point number.
    Double,
    /// A boolean value.
    Bool,
    /// A statically-allocated string we point to.
    StaticString,
    /// A FString.
    MutableString,
    /// A TArray of Variant.
    Vector,
    /// A TMap , mapping Variant to Variant.
    Map,
    /// A statically-allocated blob of data that we point to.
    StaticBlob,
    /// A blob of data that the Variant holds. Never constructed by default.
    MutableBlob,
};

namespace FVariantHelper
{
    FString FIREBASEFEATURES_API VariantTypeToString(const EFirebaseVariantType& Type);
};

namespace firebase { class Variant; }

/**
 * A variant type that can contain one of the following types:
 *      Null, FString, double, float, int64, int32, bool, TArray<FFirebaseVariant>, TMap<FFirebaseVariant, FFirebaseVariant>
*/
USTRUCT(BlueprintType)
struct FIREBASEFEATURES_API FFirebaseVariant
{
	GENERATED_BODY()

public: // Constructors
    FFirebaseVariant();
    FFirebaseVariant(FFirebaseVariant&& Other);
    FFirebaseVariant(const FFirebaseVariant&);

    FFirebaseVariant(const firebase::Variant& InVariant);
    FFirebaseVariant(const FString& Value);
    FFirebaseVariant(const TCHAR*   Value);
    FFirebaseVariant(const ANSICHAR*Value);
    FFirebaseVariant(const double & Value);
    FFirebaseVariant(const float  & Value);
    FFirebaseVariant(const int64  & Value);
    FFirebaseVariant(const int32  & Value);
    FFirebaseVariant(const bool    bValue);
    FFirebaseVariant(const TArray<FFirebaseVariant>& Value);
    FFirebaseVariant(const TMap<FFirebaseVariant, FFirebaseVariant>& Value);

    ~FFirebaseVariant();

    static FFirebaseVariant ServerTimestamp();

public: // Type helpers
    bool IsBinary()  const;
    bool IsString()  const;

public: // Converters
    bool                                     IsNull()    const;
	bool                                     AsBool()    const;
    int64                                    AsInt64()   const;
    int32                                    AsInt32()   const;
    double                                   AsDouble()  const;
    float                                    AsFloat()   const;
    FString                                  AsString()  const;
    TArray<uint8>                            AsBinary()  const;
    TMap<FFirebaseVariant, FFirebaseVariant> AsMap()     const;
    TArray<FFirebaseVariant>                 AsArray()   const;
    
public: // Getters
    EFirebaseVariantType GetType() const;
    
public: // Operators
    FFirebaseVariant& operator=  (const FFirebaseVariant& Other);
    FFirebaseVariant& operator=  (FFirebaseVariant&& Other);

    bool operator== (const FFirebaseVariant& Other) const;
    bool operator!= (const FFirebaseVariant& Other) const;
    bool operator<  (const FFirebaseVariant& Other) const;
    bool operator>  (const FFirebaseVariant& Other) const;
    bool operator>= (const FFirebaseVariant& Other) const;
    bool operator<= (const FFirebaseVariant& Other) const;

    FORCEINLINE operator firebase::Variant() const
    {
        return Variant;
    }

    FORCEINLINE operator int32() const
    {
        return AsInt64();
    }

    FORCEINLINE operator float() const
    {
        return AsFloat();
    }

    FORCEINLINE operator double() const
    {
        return AsDouble();
    }

    FORCEINLINE operator FString() const
    {
        return AsString();
    }

    const firebase::Variant& GetRawVariant() const
    {
        return Variant;
    }

    firebase::Variant& GetRawVariant()
    {
        return Variant;
    }

public: // For TMap support
    friend uint64 FIREBASEFEATURES_API GetTypeHash(const FFirebaseVariant& Var);

private:
    firebase::Variant Variant;
};

/**
 *  Not exported, meant for Blueprints.
 **/
UCLASS()
class UVariantLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()
public:
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "NULL", BlueprintAutocast))
    static FFirebaseVariant FromNull();

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromBool(const bool bValue);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromInt32(int32 Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromInt64(int64 Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromFloat(float Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromString(const FString& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromMap(const TMap<FFirebaseVariant, FFirebaseVariant>& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromStringMap(const TMap<FString, FFirebaseVariant>& Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FFirebaseVariant FromArray(const TArray<FFirebaseVariant>& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static FString ToString(UPARAM(ref) const FFirebaseVariant& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static bool ToBool(UPARAM(ref) const FFirebaseVariant& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static int32 ToInt32(UPARAM(ref) const FFirebaseVariant& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static int64 ToInt64(UPARAM(ref) const FFirebaseVariant& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static float ToFloat(UPARAM(ref) const FFirebaseVariant& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static TArray<uint8> ToBinary(UPARAM(ref) const FFirebaseVariant& Value);
    
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static TMap<FFirebaseVariant, FFirebaseVariant> ToMap(UPARAM(ref) const FFirebaseVariant& Value);

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "->", BlueprintAutocast))
    static TArray<FFirebaseVariant> ToArray(UPARAM(ref) const FFirebaseVariant& Value);

    /// Get a server-populated value corresponding to the current
    /// timestamp.
    ///
    /// When inserting values into the database, you can use the special value
    /// ServerTimestamp() to have the server auto-populate the current timestamp, 
    /// which is represented as milliseconds since the Unix epoch, into the field.
    ///
    /// @returns A special value that tells the server to use the current timestamp.
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "SERVER TIMESTAMP", BlueprintAutocast))
    static FFirebaseVariant ServerTimestamp() { return FFirebaseVariant::ServerTimestamp(); }

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Firebase|Misc|Variant", meta = (CompactNodeTitle = "MAKE", BlueprintAutocast))
    static TMap<FFirebaseVariant, FFirebaseVariant> MakeVariantMap() { return TMap<FFirebaseVariant, FFirebaseVariant>(); };
};
