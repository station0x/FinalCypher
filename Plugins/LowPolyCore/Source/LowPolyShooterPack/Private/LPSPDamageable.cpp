//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPDamageable.h"

ALPSPDamageable::ALPSPDamageable()
{
	//No Tick.
	PrimaryActorTick.bStartWithTickEnabled = false;
	PrimaryActorTick.bCanEverTick = false;
}

void ALPSPDamageable::BeginPlay()
{
	//Base.
	Super::BeginPlay();
}
