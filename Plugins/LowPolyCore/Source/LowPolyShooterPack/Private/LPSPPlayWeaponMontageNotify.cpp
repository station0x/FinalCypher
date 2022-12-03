//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPPlayWeaponMontageNotify.h"
#include "LPSPCharacter.h"
#include "Animation/AnimMontage.h"
#include "Kismet/GameplayStatics.h"

void ULPSPPlayWeaponMontageNotify::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation)
{
	//Check montage validity.
	if(!IsValid(Montage))
		return;
	
	//Play Montage.
	ALPSPCharacter* Character = Cast<ALPSPCharacter>(UGameplayStatics::GetPlayerCharacter(MeshComp->GetWorld(), 0));
	if(IsValid(Character))
		Character->PlayWeaponMontage(Montage);
}