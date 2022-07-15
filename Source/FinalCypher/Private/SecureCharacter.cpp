// Fill out your copyright notice in the Description page of Project Settings.


#include "SecureCharacter.h"

// Sets default values
ASecureCharacter::ASecureCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void ASecureCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ASecureCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void ASecureCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool ASecureCharacter::IsNetRelevantFor
(
	const AActor* RealViewer,
	const AActor* ViewTarget,
	const FVector& SrcLocation
) const {
	//APlayerController* playerController = Cast<APlayerController>(RealViewer);
	const bool irrelevant = IrrelevantControllers.Contains(Cast<APlayerController>(RealViewer));
	return !irrelevant && Super::IsNetRelevantFor(RealViewer, ViewTarget, SrcLocation);
}


