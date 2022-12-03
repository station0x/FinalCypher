//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPCharacterAnimInstance.h"
#include "LPSPCharacter.h"
#include "Kismet/GameplayStatics.h"

void ULPSPCharacterAnimInstance::NativeUpdateAnimation(const float DeltaSeconds)
{
	//Get character reference.
	Character = Cast<ALPSPCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));

	//Not having a Character will cause errors, so we return.
	if(!Character.IsValid())
		return;

	//Update the boolean values.
	bAiming = Character->IsAiming();
	bRunning = IsRunning();
}