//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPCharacter.h"
#include "LPSPGameInstance.h"
#include "LPSPScope.h"
#include "LPSPWeapon.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/InputComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

ALPSPCharacter::ALPSPCharacter()
{
	//Disable Tick since we have our own custom delay.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//Create Spring Arm.
	Spring = CreateDefaultSubobject<USpringArmComponent>(TEXT("Spring"));
	Spring->SetupAttachment(GetMesh());
	Spring->TargetArmLength = 0.0f;
	
	//Create camera.
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(Spring);

	//Create first person character component.
	Arms = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Arms"));
	Arms->SetupAttachment(Camera);
	Arms->bSelfShadowOnly = true;
	
	//Required fix.
	Arms->SetRelativeRotation(FRotator(0, -90, 0));
}

void ALPSPCharacter::OnConstruction(const FTransform& Transform)
{
	//Update pivot location for better previewing.
	Spring->SetRelativeLocation(GetViewLocation());

	//Fix pivot issue with the character. If we don't do this, the weapon mesh will be a lot higher up than the camera.
	const FVector CameraLocation = Arms->GetSocketTransform("SOCKET_Camera", RTS_Component).GetLocation();
	Arms->SetRelativeLocation(FVector(0, 0, -CameraLocation.Z));
}

void ALPSPCharacter::BeginPlay()
{
	//Base.
	Super::BeginPlay();

	//Grab the game instance so we can get settings from it at runtime and other goodies.
	GInstance = Cast<ULPSPGameInstance>(GetGameInstance());
	
	//Scale first-person view down. This fixes intersections with walls.
	Spring->SetRelativeScale3D(Spring->GetRelativeScale3D() * ViewScaleFactor);

	//Spawn weapon.
	if(IsValid(WeaponClass))
	{
		//Spawn.
		Weapon = Cast<ALPSPWeapon>(GetWorld()->SpawnActor(WeaponClass));
		if(Weapon.IsValid())
		{
			//Attach to character.
			Weapon->AttachToComponent(Arms, FAttachmentTransformRules::SnapToTargetIncludingScale, WeaponSocket);
			Weapon->SetOwner(this);

			//Cache attachment references.
			Weapon->TryGetScope(Scope);
			Weapon->TryGetMag(Magazine);
			Weapon->TryGetLaser(Laser);
			Weapon->TryGetMuzzle(Muzzle);

			//Update the AnimationInstance to use the Weapon's one. This makes sure that we're using the correct one for each Weapon.
			Arms->SetAnimInstanceClass(Weapon->GetInstance());
		}
	}
	
	//Update timer. We use this so we don't have to use the default Tick function.
	GetWorld()->GetTimerManager().SetTimer(UpdateTimerHandle, this, &ALPSPCharacter::Update, UpdateInterval, true);
}

void ALPSPCharacter::StopGameAbility(const FLPSPGameAbility Ability)
{
	//Get Animation Instance.
	const UAnimInstance* Instance = Arms->GetAnimInstance();

	//Check if we should add removed tags back. (Only add if no other montage is playing)
	const bool AddRemoved = IsValid(Instance) && (IsValid(Instance->GetCurrentActiveMontage()) == false);
	
	//Stop the ability.
	Ability.Stop(AbilityTags, AddRemoved);
}

void ALPSPCharacter::Update()
{
	//Blueprint Update. Important so we can run any Blueprint-specific code.
	BlueprintUpdate();
	
	//Calculate mapped speed by using the velocity and dividing it by the maximum speed. This gives us a zero to one speed value.
	const FVector Velocity = GetCharacterMovement()->Velocity;
	MappedSpeed = FVector2D(Velocity.X, Velocity.Y).Size() / GetCharacterMovement()->GetMaxSpeed();

	//Player specific code.
	if(IsPlayerControlled())
	{
		//First-Person specific code.
		if(!IsThirdPerson())
		{
			//Constantly update the pivot's location so the camera (parented to it) can react to changes like crouching.
			Spring->SetRelativeLocation(UKismetMathLibrary::VInterpTo(Spring->GetRelativeLocation(), GetViewLocation(),
			                                                          UGameplayStatics::GetWorldDeltaSeconds(
				                                                          GetWorld()), ViewInterpSpeed));

			//Checking in case we don't want to play the camera animation data.
			if(bUseWeaponCameraAnimation)
			{
				//Update rotation with the camera animation data from animations.
				const FRotator AnimatedRotation = Arms->GetSocketTransform("SOCKET_Camera", RTS_Component).GetRotation().Rotator();
				Camera->SetRelativeRotation(FRotator(AnimatedRotation.Roll * -1, AnimatedRotation.Yaw, AnimatedRotation.Pitch));
			}
		}

		//Update camera field of view.
		const UAnimInstance* Instance = Arms->GetAnimInstance();
		if(IsValid(Instance))
			Camera->SetFieldOfView(Instance->GetCurveValue("Field Of View"));
	}
}

void ALPSPCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	//Base.
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

float ALPSPCharacter::GetPitchAcceleration()
{
	//Current Pitch.
	const float Pitch = Spring->GetRelativeRotation().Pitch;
	
	//Calculate Acceleration. (Using the spring's relative rotation because control rotation has weird issues in the center (Pitch 0.0f)).
	const float Acceleration = UKismetMathLibrary::FClamp(PrevControlPitch - Pitch, -1.0f, 1.0f);
	
	//Update previous value.
	PrevControlPitch = Pitch;

	//Return.
	return Acceleration;
}

bool ALPSPCharacter::TryGetWeapon(ALPSPWeapon* &Value) const
{
	//Try returning Weapon.
	Value = Weapon.Get();
	return IsValid(Value);
}

void ALPSPCharacter::PlayWeaponMontage(UAnimMontage* Montage) const
{
	//Check montage validity.
	if(!IsValid(Montage))
		return;
	
	//Play.
	if(Weapon.IsValid() && IsValid(Weapon->GetMesh()) && IsValid(Weapon->GetMesh()->GetAnimInstance()))
		Weapon->GetMesh()->GetAnimInstance()->Montage_Play(Montage);
}

float ALPSPCharacter::GetAimSpeedMultiplier() const
{
	//Return scope multiplier.
	if(IsValid(Scope))
		return Scope->GetAimSpeedMultiplier();

	//Return.
	return 1.0f;
}

void ALPSPCharacter::Destroyed()
{
	//Base.
	Super::Destroyed();

	//We explicitly destroy the Weapon, otherwise it stays floating around.
	if(Weapon.IsValid())
		Weapon->Destroy();
}

// float ALPSPCharacter::GetFieldOfView() const
// {
	//We grab the field of view from the settings file, so we can update it from menus.
	// if(GInstance.IsValid())
	// {
		// ULPSPSGSettings* Settings = GInstance->LoadSettings();
		// if(IsValid(Settings))
			// return Settings->GetFieldOfView();
	// }

	//Return a weird value to remind us that we have an error.
	// return 97.0f;
// }

// float ALPSPCharacter::GetSensitivity() const
// {
// 	//We grab the sensitivity from the settings file, so we can update it from menus.
// 	if(GInstance.IsValid())
// 	{
// 		ULPSPSGSettings* Settings = GInstance->LoadSettings();
// 		if(IsValid(Settings))
// 			return Settings->GetSensitivity();
// 	}
//
// 	//Return a weird value to remind us that we have an error.
// 	return 0.75f;
// }