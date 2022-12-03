//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPAttachment.h"

ALPSPAttachment::ALPSPAttachment()
{
	//Disable ticking.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	//We create the basic mesh here for convenience.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = Mesh;

	//We disable decals so that they don't show up on the Attachments in first person, otherwise it looks weird.
	Mesh->bReceivesDecals = false;
}

void ALPSPAttachment::OnConstruction(const FTransform& Transform)
{
	//Base.
	Super::OnConstruction(Transform);
	
	//Check Mesh validity.
	if(!IsValid(Mesh))
		return;

	//Loop through the material pairs.
	for(const TPair<FName, UMaterialInterface*> Pair : Materials)
	{
		//Get and check Slot name..
		const FName Slot = Pair.Key;
		if(!Mesh->IsMaterialSlotNameValid(Slot))
			continue;

		//Get and check Material.
		UMaterialInterface* Material = Pair.Value;
		if(!IsValid(Material))
			return;

		//Set material.
		Mesh->SetMaterialByName(Slot, Material);
	}
}

void ALPSPAttachment::OnEnable_Implementation()
{
	//Base.
	Super::OnEnable_Implementation();
	
	//Update mesh values.
	Mesh->SetVisibility(true);
}

void ALPSPAttachment::OnDisable_Implementation()
{
	//Base.
	Super::OnDisable_Implementation();
	
	//Update mesh values.
	Mesh->SetVisibility(false);
}