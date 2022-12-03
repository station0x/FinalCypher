//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPRepresentation.h"

ALPSPRepresentation::ALPSPRepresentation()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = bTicks;
}

void ALPSPRepresentation::OnConstruction(const FTransform& Transform)
{
	//Base.
	Super::OnConstruction(Transform);

	//Update Enabled.
	SetEnabled(bEnabled);
}

void ALPSPRepresentation::TrySetEnabled(const bool bValue)
{
	//Update value.
	if(bEnabled != bValue)
		SetEnabled(bValue);
}

void ALPSPRepresentation::SetEnabled(const bool bValue)
{
	//Update the value.
	bEnabled = bValue;
	bEnabled ? OnEnable() : OnDisable();
}

void ALPSPRepresentation::OnEnable_Implementation()
{
	//Update settings.
	SetActorHiddenInGame(bHiddenInGame);
	
	//Reset ticking.
	SetActorTickEnabled(bTicks);
	PrimaryActorTick.bStartWithTickEnabled = bTicks;
}

void ALPSPRepresentation::OnDisable_Implementation()
{
	//Update settings.
	SetActorHiddenInGame(true);
	
	//Disable ticking.
	SetActorTickEnabled(false);
	PrimaryActorTick.bStartWithTickEnabled = false;
}