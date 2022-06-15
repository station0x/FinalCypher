// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SecureCharacter.generated.h"

UCLASS()
class TPSFPS_API ASecureCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASecureCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Replication Security")
	TSet<APlayerController*> IrrelevantControllers;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	virtual bool IsNetRelevantFor
	(
		const AActor* RealViewer,
		const AActor* ViewTarget,
		const FVector& SrcLocation
	) const override;

};
