//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "LPSPGameAbility.h"
#include "LPSPGameInstance.h"
#include "LPSPWeaponHolsterState.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "LPSPCharacter.generated.h"

/**Base Character class used in the asset.*/
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPCharacter final : public ACharacter
{
	GENERATED_BODY()

public:
	//Sets default values for this character's properties.
	ALPSPCharacter();

	//Called to bind functionality to input.
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//Called while in-blueprint.
	virtual void OnConstruction(const FTransform& Transform) override;

	/**Called when the object is destroyed.*/
	virtual void Destroyed() override;

	/**Returns the value of Spring.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	class USpringArmComponent* GetSpring() const { return Spring; }

	/**Returns the value of Camera.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	class UCameraComponent* GetCamera() const { return Camera; }

	/**Returns the value of Arms.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	USkeletalMeshComponent* GetArms() const { return Arms; }

	/**Returns the value of FieldOfView.*/
	// UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	// float GetFieldOfView() const;

	/**Returns the value of RunningFieldOfView.*/
	// UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	// float GetRunningFieldOfView() const { return GetFieldOfView() * RunningFieldOfViewMultiplier; }

	/**Returns mapped speed float.**/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	float GetMappedSpeed() const { return MappedSpeed; }

	/**Returns the current pitch acceleration.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	float GetPitchAcceleration();

	/**Returns look around values.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	FVector2D GetLook() const { return Look; }

	/**Returns look around values.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	FVector2D GetMovement() const { return Movement; }

	/**Returns true if this character is currently aiming.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	bool IsAiming() const { return bAiming; }

	/**Returns true if this character is currently running.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	bool IsRunning() const { return bRunning; }

	/**Returns the holstering state that the equipped weapon is currently in.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	TEnumAsByte<ELPSPWeaponHolsterState> GetWeaponHolsterState() const { return WeaponHolsterState; }

	/**Returns the reference to the weapon's representation.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	bool TryGetWeapon(class ALPSPWeapon* &Value) const;

	/**Returns the value of Weapon.*/
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Low Poly Shooter Pack | Character | Weapon")
	ALPSPWeapon* GetWeapon() const { return Weapon.Get(); }
	
	/**Sets the weapon holster state value for this character.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Character")
    void SetWeaponHolsterState(const TEnumAsByte<ELPSPWeaponHolsterState> Value) { WeaponHolsterState = Value; }

	/**Plays a montage on the weapon skeletal mesh.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Character")
	void PlayWeaponMontage(UAnimMontage* Montage) const;

	/**Returns the value of bThirdPerson.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	bool IsThirdPerson() const { return bThirdPerson; }

	/**Returns a pointer to the equipped weapon's scope.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	class ALPSPScope* GetScope() const { return Scope; }

	/**Returns a pointer to the equipped weapon's magazine.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	class ALPSPMagazine* GetMagazine() const { return Magazine; }

	/**Returns a pointer to the equipped weapon's laser.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	class ALPSPLaser* GetLaser() const { return Laser; }

	/**Returns a pointer to the equipped weapon's muzzle.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	class ALPSPMuzzle* GetMuzzle() const { return Muzzle; }

	/**Returns aiming speed from the equipped scope.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	float GetAimSpeedMultiplier() const;

	/**Returns the value used as the aiming speed.*/
	UFUNCTION(BlueprintGetter, Category = "Low Poly Shooter Pack | Character")
	float GetAimDuration() const
	{
		//Return.
		return AimBlendTime / GetAimSpeedMultiplier();
	}

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Low Poly Shooter Pack | Character")
	bool IsCrosshairVisible() const;

protected:
	/**Is this character in third person mode?*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Shooter Pack | Character | View")
	bool bThirdPerson;

	/**Offset from the top of the character's capsule at which the camera will be located.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | View")
	FVector FirstPersonOffset = FVector(0, 0, -35.0f);

	/**Speed of interpolation for any transformations applied to the character's camera. The main transformation being applied is the crouching.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | View")
	float ViewInterpSpeed = 15;
	
	/**Scale factor applied to the first-person view. Used in order to avoid weapons intersecting walls. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | View")
	float ViewScaleFactor = 0.2f;

	/**Montage to play when throwing a grenade.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | Animation", meta = (DisplayName = "Grenade Throw Montage"))
	UAnimMontage* GrenadeThrowMontage;

	/**Right-Hand Additive Montage to play when throwing a grenade.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | Animation", meta = (DisplayName = "Grenade Throw Montage Additive"))
	UAnimMontage* GrenadeThrowAdditiveMontage;

	/**Montage to play when attacking melee.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | Animation", meta = (DisplayName = "Melee Montage"))
	UAnimMontage* MeleeMontage;
	
	/**Montage to play when attacking melee.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | Animation", meta = (DisplayName = "Melee Montage Additive"))
	UAnimMontage* MeleeAdditiveMontage;

	/**Value to multiply the movement speed when moving sideways.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | Movement Settings")
	float HorizontalSpeedMultiplier = 0.8f;
	
	/**Value to multiply the movement speed when moving backwards.*/
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Low Poly Shooter Pack | Character | Movement Settings")
	float BackwardsSpeedMultiplier = 0.7f;
	
	/**Velocity value mapped from zero to one.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	float MappedSpeed;
	
	/**Is this character currently aiming?*/
	UPROPERTY(BlueprintReadWrite, Category = "Low Poly Shooter Pack")
	bool bAiming;

	/**Is this character currently running?*/
	UPROPERTY(BlueprintReadWrite, Category = "Low Poly Shooter Pack")
	bool bRunning;

	/**Is this character currently crouching?*/
	UPROPERTY(BlueprintReadWrite, Category = "Low Poly Shooter Pack")
	bool bCrouching;

	/**Returns the character weapon's scope.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	class ALPSPScope* Scope;

	/**Magazine attached to the equipped weapon.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	class ALPSPMagazine* Magazine;

	/**Laser attached to the equipped weapon.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	class ALPSPLaser* Laser;

	/**Muzzle attached to the equipped weapon.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	class ALPSPMuzzle* Muzzle;

	/**Current ability tags.*/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Low Poly Shooter Pack | Data")
	FGameplayTagContainer AbilityTags;

	/**Forward and right movement values.*/
	UPROPERTY(BlueprintReadWrite, Category = "Low Poly Shooter Pack")
	FVector2D Movement;

	/**Yaw and Pitch values for the look axes.*/
	UPROPERTY(BlueprintReadWrite, Category = "Low Poly Shooter Pack")
	FVector2D Look;
	
	//Called when the game starts or when spawned.
	virtual void BeginPlay() override;
	
	/**Update. Basically a Tick function using a special delay setup.*/
	UFUNCTION()
	virtual void Update();

	/**Blueprint version of our Update function. Allows us to do more custom work in Blueprints.*/
	UFUNCTION(BlueprintCallable, BlueprintImplementableEvent, Category = "Low Poly Shooter Pack | Character")
	void BlueprintUpdate();

	/**Returns the location at which should be at any given time.*/
	UFUNCTION(BlueprintPure, Category = "Low Poly Shooter Pack | Character")
	FVector GetViewLocation() const
	{
		//Offset from the top of the capsule.
		return FirstPersonOffset + FVector(0, 0, GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
	}

	/**Utility function to stop a game ability.*/
	UFUNCTION(BlueprintCallable, Category = "Low Poly Shooter Pack | Character", meta = (DisplayName = "Game Ability Stop"))
	void StopGameAbility(FLPSPGameAbility Ability);

	/**Returns the default sensitivity value.*/
	// UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Character")
	// float GetSensitivity() const;

private:
	/**Spring Arm Component used to allow for more customization of the view location.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetSpring, Category = "Low Poly Shooter Pack")
	class USpringArmComponent* Spring;

	/**Camera used in-game.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetCamera, Category = "Low Poly Shooter Pack")
	class UCameraComponent* Camera;

	/**Skeletal Mesh used to represent the Character's first-person arms. Weapons are parented to this Mesh.*/
	UPROPERTY(EditInstanceOnly, BlueprintGetter = GetArms, Category = "Low Poly Shooter Pack")
	class USkeletalMeshComponent* Arms;

	/**Interval between each Update call.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Character | Update")
	float UpdateInterval = 0.01f;
	
	/**Field Of View value used when running.*/
	// UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Character | Camera", meta = (DisplayName = "Field Of View Running Multiplier"))
	// float RunningFieldOfViewMultiplier = 1.06f;
	
	/**Type Of Weapon To Use.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Character | Weapon", meta = (DisplayName = "Weapon Class"))
	TSubclassOf<ALPSPWeapon> WeaponClass;

	/**Socket on the character to parent the weapon to.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Character | Weapon", meta = (DisplayName = "Weapon Socket"))
	FName WeaponSocket = "SOCKET_Weapon";

	/**Should the weapon's camera animation be used?*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Character | Weapon", meta = (DisplayName = "Use Weapon Camera Animation"))
	bool bUseWeaponCameraAnimation = true;
	
	/**Pitch value the pivot had last frame.*/
	float PrevPitch;

	/**Clamped acceleration of the camera's pitch.*/
	float PrevControlPitch;

	/**Reference to the equipped Weapon.*/
	TWeakObjectPtr<ALPSPWeapon> Weapon;

	/**Settings object.*/
	TWeakObjectPtr<ULPSPGameInstance> GInstance;
	
	/**Handle for the Update Timer. Helps us pause or stop the Timer if we ever need to.*/
	FTimerHandle UpdateTimerHandle;

	/**Speed Of Aiming. Don't change this.*/
	const float AimBlendTime = 0.25f;

	/**Current holster state the character's equipped weapon is in.*/
	TEnumAsByte<ELPSPWeaponHolsterState> WeaponHolsterState;

	/**Default Implementation.*/
	bool IsCrosshairVisible_Implementation() const { return !bAiming && WeaponHolsterState == Available; }
};