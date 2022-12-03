//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPCasing.h"
#include "Runtime/Engine/Classes/Kismet/GameplayStatics.h"
#include "Runtime/Engine/Classes/Kismet/KismetMathLibrary.h"

ALPSPCasing::ALPSPCasing()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//Simple root creation.
	Pivot = CreateDefaultSubobject<USceneComponent>(TEXT("Pivot"));
	RootComponent = Pivot;

	//Create casing mesh component.
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Pivot);
}

void ALPSPCasing::OnSpawn_Implementation()
{
	//We calculate the delay on spawn so that it works properly, as Begin Play is not getting called.
	DestroyDelay = UKismetMathLibrary::RandomFloatInRange(DestroyDelayRange.X, DestroyDelayRange.Y);
}

void ALPSPCasing::ApplyImpulse_Implementation(FVector Direction, const FVector Strength) const
{
	//Add impulse.
	Mesh->AddImpulse(Direction.Normalize() * Strength);
}

void ALPSPCasing::Tick(const float DeltaTime)
{
	//Base.
	Super::Tick(DeltaTime);

	//Rotate constantly.
	const float Rotation = DeltaTime * RotationSpeed;
	Mesh->AddLocalRotation(FRotator(Rotation, Rotation, 0));

	//Destroy.
	if(GetGameTimeSinceCreation() >= DestroyDelay)
	{
		//Play sound.
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), Sound, GetActorLocation());

		//Destroy this actor.
		Destroy();
	}
}