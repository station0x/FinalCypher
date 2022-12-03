//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPCharacter.h"
#include "LPSPCharacterAnimInstance.h"
#include "LPSPLagValues.h"
#include "LPSPOffset.h"
#include "Kismet/KismetMathLibrary.h"
#include "LPSPViewmodelAnimInstance.generated.h"

UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ULPSPViewmodelAnimInstance final : public ULPSPCharacterAnimInstance
{
	GENERATED_BODY()

public:
	/**Animation value update.*/
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

protected:
	/**Weapon offset applied while standing. (Idle Pose)*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Viewmodel | Offsets", meta = (DisplayName = "Offset Standing"))
	FLPSPOffset OffsetStanding;

	/**Weapon offset applied while crouching.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Viewmodel | Offsets", meta = (DisplayName = "Offset Crouching"))
	FLPSPOffset OffsetCrouching;

	/**Weapon offset applied while running.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Viewmodel | Offsets", meta = (DisplayName = "Offset Running"))
	FLPSPOffset OffsetRunning;

	/**Lag values used while standing around.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Viewmodel | Lag", meta = (DisplayName = "Lag Standing"))
	FLPSPLagValues LagStanding;

	/**Lag values used while aiming.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Viewmodel | Lag", meta = (DisplayName = "Lag Aiming"))
	FLPSPLagValues LagAiming;

	/**
	* Interpolation speed applied on the character's movement input used to calculate movement lag.
	* Bigger values will make the lag snap more.
	*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Viewmodel | Lag", meta = (DisplayName = "Lag Movement Interp Speed"))
	float LagMovementInterpSpeed = 10.0f;
	
	/**Movement value from zero to one.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	float Movement;

	/**Speed of turning.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	float TurnRate;

	/**Pitch acceleration.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	float PitchAcceleration;
	
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector AimingLocationLag;

	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector AimingRotationLag;

	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector AimingMovementLocationLag;

	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector AimingMovementRotationLag;
	
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector StandingLocationLag;
	
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector StandingRotationLag;

	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector StandingMovementLocationLag;

	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector StandingMovementRotationLag;

	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector FinalStandingOffset;

	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FVector FinalRotationStandingOffset;
	
	/**Offset values taken from the attachments currently equipped on the character's weapon.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FLPSPOffset AttachmentsOffset;

	/**Offset values coming from the currently equipped scope.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	FLPSPOffset ScopeAimOffset;

	/**Value to multiply the default field of view while aiming (comes from the currently equipped scope).*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	float AimFieldOfViewMultiplier = 1.0f;

	/**Is the character crouched?*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")	
	bool bCrouching;

	/**Current holster state the character's equipped weapon is in.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")	
	TEnumAsByte<ELPSPWeaponHolsterState> WeaponHolsterState;

	/**Returns aiming speed from the equipped scope.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Viewmodel")
	float GetAimPlayRate() const
	{
		//Validity check.
		if(!Character.IsValid())
			return 1.0f;
		
		//Return.
		return Character->GetAimSpeedMultiplier();
	}

	/**Returns the value used as the aiming speed.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Viewmodel")
	TArray<float> GetAimBlendTime() const
	{
		//Check Character validity.
		if(!Character.IsValid())
			return {};
		
		//Value.
		const float Return = Character->GetAimDuration();
		
		//Return.
		return { Return, Return };
	}

private:
	/***/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Viewmodel | Look Offsets", meta = (DisplayName = "Look Offset Multiplier Location"))
	FVector LookOffsetMultiplierLocation = FVector(0.0f, 0.0f, 0.15f);

	/***/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Viewmodel | Look Offsets", meta = (DisplayName = "Look Offset Multiplier Rotation"))
	FVector LookOffsetMultiplierRotation = FVector(0.0f, 0.5f, 0.0f);
	
	FVector2D CharacterMovementValue;
	FVectorSpringState AimingLocationSpringState;
	FVectorSpringState AimingRotationSpringState;
	FVectorSpringState AimingMovementLocationSpringState;
	FVectorSpringState AimingMovementRotationSpringState;
	FVectorSpringState StandingLocationSpringState;
	FVectorSpringState StandingRotationSpringState;
	FVectorSpringState StandingMovementLocationSpringState;
	FVectorSpringState StandingMovementRotationSpringState;
	FVectorSpringState StandingOffsetLocationSpringState;
	FVectorSpringState StandingOffsetRotationSpringState;
};