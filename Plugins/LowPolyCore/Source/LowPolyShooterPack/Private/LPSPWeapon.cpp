//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPWeapon.h"
#include "LPSPMuzzle.h"
#include "LPSPGrip.h"
#include "LPSPLaser.h"
#include "LPSPMagazine.h"
#include "LPSPScope.h"
#include "Components/ArrowComponent.h"

ALPSPWeapon::ALPSPWeapon()
{
	//Disable tick.
	PrimaryActorTick.bCanEverTick = false;

	//Create weapon mesh component.
	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh"));
	Mesh->CanCharacterStepUpOn = ECB_No;
	RootComponent = Mesh;

	//Muzzle holder.
	Muzzles = CreateDefaultSubobject<USceneComponent>(TEXT("Muzzles"));
	Muzzles->SetupAttachment(Mesh, "SOCKET_Muzzle");
	
	//Scope holder.
	Scopes = CreateDefaultSubobject<USceneComponent>(TEXT("Scopes"));
	Scopes->SetupAttachment(Mesh, "SOCKET_Scope");

	//Laser holder.
	Lasers = CreateDefaultSubobject<USceneComponent>(TEXT("Lasers"));
	Lasers->SetupAttachment(Mesh, "SOCKET_Laser");
	
	//Mag holder.
	Magazines = CreateDefaultSubobject<USceneComponent>(TEXT("Magazines"));
	Magazines->SetupAttachment(Mesh, "SOCKET_Magazine");
	
	//Grip holder.
	Grips = CreateDefaultSubobject<USceneComponent>(TEXT("Grips"));
	Grips->SetupAttachment(Mesh, "SOCKET_Grip");

	//Defaults holder.
	Defaults = CreateDefaultSubobject<USceneComponent>(TEXT("Defaults"));
	Defaults->SetupAttachment(Mesh, "SOCKET_Default");

	//Ejection visual representation.
	Eject = CreateDefaultSubobject<UArrowComponent>(TEXT("Eject"));
	Eject->SetupAttachment(Mesh, "SOCKET_Eject");

	//Update arrow properties to make more readable.
	Eject->ArrowSize = 0.1f;
	Eject->ArrowColor = FColor::Blue;
}

void ALPSPWeapon::OnConstruction(const FTransform& Transform)
{
	//Base.
	Super::OnConstruction(Transform);
	
	//Get all child actor components.
	TArray<AActor*> Actors;
	GetAllChildActors(Actors, false);
	for (AActor* const Actor : Actors)
	{
		//Disable all attachments.
		ALPSPAttachment* const Attachment = Cast<ALPSPAttachment>(Actor);
		if(IsValid(Attachment))
			Attachment->TrySetEnabled(Attachment->IsAlwaysVisible());
	}

	//Select the correct attachments.
	Attachments = TArray<ALPSPAttachment*>{};
	CacheAttachment<ALPSPMuzzle>(EquippedMuzzle);
	CacheAttachment<ALPSPScope>(EquippedScope);
	CacheAttachment<ALPSPLaser>(EquippedLaser);
	CacheAttachment<ALPSPMagazine>(EquippedMagazine);
	CacheAttachment<ALPSPGrip>(EquippedGrip);
}

bool ALPSPWeapon::TryGetAttachment(const TSubclassOf<ALPSPAttachment> Class, ALPSPAttachment*& OutAttachment)
{
	//Try finding a muzzle attachment.
	for(ALPSPAttachment* Attachment : Attachments)
	{
		//Validity check.
		if(!IsValid(Attachment))
			continue;
		
		//Cast.
		if(Attachment->IsA(Class))
		{
			OutAttachment = Attachment;
			return true;
		}
	}

	return false;
}

bool ALPSPWeapon::TryGetMuzzle(ALPSPMuzzle*& Muzzle)
{
	//Try finding a muzzle attachment.
	for(ALPSPAttachment* Attachment : Attachments)
	{
		//Validity check.
		if(!IsValid(Attachment))
			continue;
		
		//Cast.
		Muzzle = Cast<ALPSPMuzzle>(Attachment);
		if(IsValid(Muzzle))
			return true;
	}

	return false;
}

bool ALPSPWeapon::TryGetScope(ALPSPScope*& Scope)
{
	//Try finding a scope attachment.
	for(ALPSPAttachment* Attachment : Attachments)
	{
		//Validity check.
		if(!IsValid(Attachment))
			continue;
		
		//Cast.
		Scope = Cast<ALPSPScope>(Attachment);
		if(IsValid(Scope))
			return true;
	}

	return false;
}

bool ALPSPWeapon::TryGetLaser(ALPSPLaser*& Laser)
{
	//Try finding a laser attachment.
	for(ALPSPAttachment* Attachment : Attachments)
	{
		//Validity check.
		if(!IsValid(Attachment))
			continue;
		
		//Cast.
		Laser = Cast<ALPSPLaser>(Attachment);
		if(IsValid(Laser))
			return true;
	}

	return false;
}

bool ALPSPWeapon::TryGetMag(ALPSPMagazine*& Mag)
{
	//Try finding a mag attachment.
	for(ALPSPAttachment* Attachment : Attachments)
	{
		//Validity check.
		if(!IsValid(Attachment))
			continue;
		
		//Cast.
		Mag = Cast<ALPSPMagazine>(Attachment);
		if(IsValid(Mag))
			return true;
	}

	return false;
}

bool ALPSPWeapon::TryGetGrip(ALPSPGrip*& Grip)
{
	//Try finding a mag attachment.
	for(ALPSPAttachment* Attachment : Attachments)
	{
		//Validity check.
		if(!IsValid(Attachment))
			continue;
		
		//Cast.
		Grip = Cast<ALPSPGrip>(Attachment);
		if(IsValid(Grip))
			return true;
	}

	return false;
}