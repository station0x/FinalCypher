//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPEjectCasingNotify.h"
#include "LPSPMagazine.h"
#include "LPSPWeapon.h"
#include "LPSPCasing.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

void ULPSPEjectCasingNotify::Notify(USkeletalMeshComponent* Weapon, UAnimSequenceBase* Animation)
{
	//Cache representation reference.
	ALPSPWeapon* Representation = Cast<ALPSPWeapon>(Weapon->GetOwner());
	if(!IsValid(Representation))
		return;

	//Save a reference to the currently equipped magazine.
	ALPSPMagazine* Magazine = nullptr;
	if(!Representation->TryGetMag(Magazine))
		return;
		
	//Check validity.
	TSubclassOf<ALPSPCasing> Casing = nullptr;
	if(Magazine->TryGetCasingType(Casing))
	{
		//Get socket rotation.
		const FRotator SocketRotation = Weapon->GetSocketRotation(EjectSocketName);

		//Get socket location.
		const FVector SocketForward = UKismetMathLibrary::GetForwardVector(SocketRotation);
		const FVector SocketLocation = Weapon->GetSocketLocation(EjectSocketName) + (SocketForward * (Offset + Magazine->GetCasingOffset()));
			
		//Spawn casing.
		const FRotator SpawnRotation = bRandomizeInitialRotation ? UKismetMathLibrary::RandomRotator(true) : SocketRotation;
		ALPSPCasing* SpawnedCasing = Cast<ALPSPCasing>(Weapon->GetWorld()->SpawnActor(Casing, &SocketLocation, &SpawnRotation));
		SpawnedCasing->OnSpawn();
		
		//Add impulse.
		const FVector2D ImpulseRange = Magazine->GetCasingImpulseRange();
		SpawnedCasing->ApplyImpulse(SocketForward,
		                            UKismetMathLibrary::RandomFloatInRange(ImpulseRange.X, ImpulseRange.Y) * Impulse *
		                            FVector::OneVector);
	}
}