//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPCrosshair.h"
#include "Kismet/GameplayStatics.h"

ESlateVisibility ULPSPCrosshair::GetSlateVisibility()
{
	//We'll get errors if we check Character code while it's not assigned.
	if(Character.IsValid())
	{
		//The Crosshair is visible only if the Character allows it to be!
		if(Character->IsCrosshairVisible())
			return ESlateVisibility::Visible;
	}
	else
		TryGetPlayerCharacter();

	//Return Hidden.
	return ESlateVisibility::Hidden;
}

void ULPSPCrosshair::TryGetPlayerCharacter()
{
	//Ignore already valid references to not waste time.
	if(Character.IsValid())
		return;
	
	//Get player character.
	ACharacter* Reference = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);
	if(IsValid(Reference))
		Character = Cast<ALPSPCharacter>(Reference);
}