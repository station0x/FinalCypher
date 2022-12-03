//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPMagazine.h"
#include "LPSPProjectile.h"
#include "LPSPCasing.h"
#include "Kismet/KismetMathLibrary.h"

void ALPSPMagazine::BeginPlay()
{
	//Base.
	Super::BeginPlay();
	
	//Top up the ammo.
	Fill();
}

void ALPSPMagazine::AddAmmo(const int Amount)
{
	//Add ammunition.
	CurrentAmmo += Amount;
	CurrentAmmo = UKismetMathLibrary::Clamp(CurrentAmmo, 0, TotalAmmo);

	//Blueprint event.
	OnChangeAmmoCount();
}

void ALPSPMagazine::Fill()
{
	//Update ammo.
	CurrentAmmo = TotalAmmo;

	//Blueprint event.
	OnFill();
}

bool ALPSPMagazine::TryGetCasingType_Implementation(TSubclassOf<ALPSPCasing>& Type) const
{
	//Return Casing Type.
	Type = CasingType;
	return IsValid(Type);
}

bool ALPSPMagazine::TryGetProjectileType_Implementation(TSubclassOf<ALPSPProjectile>& Type) const
{
	//Return Projectile Type.
	Type = ProjectileType;
	return IsValid(Type);
}