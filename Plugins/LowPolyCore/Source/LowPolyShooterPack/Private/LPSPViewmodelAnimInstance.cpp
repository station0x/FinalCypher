//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPViewmodelAnimInstance.h"
#include "LPSPCharacter.h"
#include "LPSPScope.h"
#include "LPSPWeapon.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

void ULPSPViewmodelAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	//Invoke base.
	Super::NativeUpdateAnimation(DeltaSeconds);

	//Character validity check.
	if(!Character.IsValid())
		return;

	//Update booleans.
	bCrouching = Character->bIsCrouched;
	bFalling = Character->GetCharacterMovement()->IsFalling();

	//Update states.
	WeaponHolsterState = Character->GetWeaponHolsterState();

	//Lag multiplier.
	FLPSPLagValues LagMultiplier = FLPSPLagValues();

	//Reset attachments offset.
	AttachmentsOffset = FLPSPOffset();

	//Get a reference to the character's weapon.
	ALPSPWeapon* Weapon = Character->GetWeapon();
	if(IsValid(Weapon))
	{
		//Include attachment offsets in our offset calculation.
		TArray<ALPSPAttachment*> Attachments = Weapon->GetAttachments();
		for(const ALPSPAttachment* Attachment : Attachments)
			AttachmentsOffset = AttachmentsOffset + Attachment->GetOffset();
		
		//Get a reference to the equipped scope on the character's weapon.
		ALPSPScope* Scope = nullptr;
		if(Weapon->TryGetScope(Scope))
		{
			//Get scope aiming offset.
			ScopeAimOffset = Scope->GetAimOffset();

			//Get aiming field of view multiplier.
			AimFieldOfViewMultiplier = Scope->GetFieldOfViewMultiplier();

			//Get lag multiplier.
			LagMultiplier = Scope->GetLagMultiplier();
		}
	}
	
	//Update the movement value.
	Movement = UKismetMathLibrary::FClamp(Character->GetMappedSpeed(), 0.0f, 1.0f);

	//Update our turning rate. (Used to play nice turning animations)
	const float Yaw = UKismetMathLibrary::FClamp(Character->GetLook().X, -1.0f, 1.0f);
	TurnRate = UKismetMathLibrary::Abs(Yaw) - Movement;

	//Calculate the pitch. This line is useful to get proper pitch values [-90, 90].
	const float Pitch = UKismetMathLibrary::NormalizedDeltaRotator(Character->GetControlRotation(),
	                                                               Character->GetActorRotation()).Pitch * -0.1f;

	//Get the pitch acceleration so we can have nice clamped lag!
	PitchAcceleration = Character->GetPitchAcceleration();

	//We interpolate the movement to get a smoother result, otherwise direction changes make lag snap.
	CharacterMovementValue = UKismetMathLibrary::Vector2DInterpTo(CharacterMovementValue, Character->GetMovement(), DeltaSeconds, LagMovementInterpSpeed);

	FVector AimingLocationTarget = FVector();
	AimingLocationTarget = LagAiming.GetLook().Location.Horizontal * LagMultiplier.GetLook().Location.Horizontal *
		UKismetMathLibrary::FClamp(Character->GetLook().X, -1.0f, 1.0f) + LagAiming.GetLook().Location.Vertical *
		LagMultiplier.GetLook().Location.Vertical * PitchAcceleration;
	AimingLocationLag = UKismetMathLibrary::VectorSpringInterp(AimingLocationLag, AimingLocationTarget,
	                                                           AimingLocationSpringState, LagAiming.GetStiffness(),
	                                                           LagAiming.GetDamping(), DeltaSeconds, 0.006f);

	FVector AimingRotationTarget = FVector();
	AimingRotationTarget = LagAiming.GetLook().Rotation.Horizontal * LagMultiplier.GetLook().Rotation.Horizontal *
		UKismetMathLibrary::FClamp(Character->GetLook().X, -1.0f, 1.0f) + LagAiming.GetLook().Rotation.Vertical *
		LagMultiplier.GetLook().Rotation.Vertical * PitchAcceleration;
	AimingRotationLag = UKismetMathLibrary::VectorSpringInterp(AimingRotationLag, AimingRotationTarget, AimingRotationSpringState, LagAiming.GetStiffness(), LagAiming.GetDamping(), DeltaSeconds, 0.006f);

	const FVector AimingMovementLocationTarget = LagAiming.GetMovement().Location.Horizontal * LagMultiplier.GetMovement().Location.Horizontal * UKismetMathLibrary::FClamp(CharacterMovementValue.X, -1.0f, 1.0f) + LagAiming.GetMovement().Location.Vertical * LagMultiplier.GetMovement().Location.Vertical * UKismetMathLibrary::FClamp(CharacterMovementValue.Y, -1.0f, 1.0f);
	AimingMovementLocationLag = UKismetMathLibrary::VectorSpringInterp(AimingMovementLocationLag, AimingMovementLocationTarget, AimingMovementLocationSpringState, LagAiming.GetStiffness(), LagAiming.GetDamping(), DeltaSeconds, 0.006f);
	
	const FVector AimingMovementRotationTarget = LagAiming.GetMovement().Rotation.Horizontal * LagMultiplier.GetMovement().Rotation.Horizontal * UKismetMathLibrary::FClamp(CharacterMovementValue.X, -1.0f, 1.0f) + LagAiming.GetMovement().Rotation.Vertical * LagMultiplier.GetMovement().Rotation.Vertical * UKismetMathLibrary::FClamp(CharacterMovementValue.Y, -1.0f, 1.0f);
	AimingMovementRotationLag = UKismetMathLibrary::VectorSpringInterp(AimingMovementRotationLag, AimingMovementRotationTarget, AimingMovementRotationSpringState, LagAiming.GetStiffness(), LagAiming.GetDamping(), DeltaSeconds, 0.006f);
	
	FVector StandingLocationTarget = UKismetMathLibrary::FClamp(Character->GetLook().X, -1.0f, 1.0f) * LagStanding
		.GetLook().Location.Horizontal + LagStanding.GetLook().Location.Vertical * PitchAcceleration;
	StandingLocationTarget += Pitch * LookOffsetMultiplierLocation;
	StandingLocationLag = UKismetMathLibrary::VectorSpringInterp(StandingLocationLag, StandingLocationTarget,
	                                                             StandingLocationSpringState,
	                                                             LagStanding.GetStiffness(), LagStanding.GetDamping(),
	                                                             DeltaSeconds, 0.006f);

	FVector StandingRotationTarget;
	StandingRotationTarget = LagStanding.GetLook().Rotation.Horizontal *
		UKismetMathLibrary::FClamp(Character->GetLook().X, -1.0f, 1.0f) + LagStanding.GetLook().Rotation.Vertical *
		PitchAcceleration;
	StandingRotationTarget += UKismetMathLibrary::Clamp(Pitch, -10.0f, 0.0f) * LookOffsetMultiplierRotation;
	StandingRotationLag = UKismetMathLibrary::VectorSpringInterp(StandingRotationLag, StandingRotationTarget,
	                                                             StandingRotationSpringState,
	                                                             LagStanding.GetStiffness(), LagStanding.GetDamping(),
	                                                             DeltaSeconds, 0.006f);
	
	const FVector StandingMovementLocationTarget = LagStanding.GetMovement().Location.Horizontal * UKismetMathLibrary::FClamp(CharacterMovementValue.X, -1.0f, 1.0f) + LagStanding.GetMovement().Location.Vertical * UKismetMathLibrary::FClamp(CharacterMovementValue.Y, -1.0f, 1.0f);
	StandingMovementLocationLag = UKismetMathLibrary::VectorSpringInterp(StandingMovementLocationLag, StandingMovementLocationTarget, StandingMovementLocationSpringState, LagStanding.GetStiffness(), LagStanding.GetDamping(), DeltaSeconds, 0.006f);

	const FVector StandingMovementRotationTarget = LagStanding.GetMovement().Rotation.Horizontal * UKismetMathLibrary::FClamp(CharacterMovementValue.X, -1.0f, 1.0f) + LagStanding.GetMovement().Rotation.Vertical * UKismetMathLibrary::FClamp(CharacterMovementValue.Y, -1.0f, 1.0f);
	StandingMovementRotationLag = UKismetMathLibrary::VectorSpringInterp(StandingMovementRotationLag, StandingMovementRotationTarget, StandingMovementRotationSpringState, LagStanding.GetStiffness(), LagStanding.GetDamping(), DeltaSeconds, 0.006f);
}