//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPOffset.generated.h"

/**Simple offset struct. Contains values used for animated offsets.*/
USTRUCT(BlueprintType)
struct LOWPOLYSHOOTERPACK_API FLPSPOffset
{
	GENERATED_BODY()

public:
	/**Default constructor.*/
	explicit FLPSPOffset(const FVector Loc = FVector::ZeroVector, const FRotator Rot = FRotator::ZeroRotator,
	                 const FVector Scale = FVector::ZeroVector);

	/**Location offset.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector Location;

	/**Rotation offset.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FRotator Rotation;

	/**Scale offset.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector Scale;

	/**Addition.*/
	FLPSPOffset Add(const FLPSPOffset &Other) const;

	/**Addition.*/
	FLPSPOffset operator+(const FLPSPOffset &Other) const { return Add(Other); }
};