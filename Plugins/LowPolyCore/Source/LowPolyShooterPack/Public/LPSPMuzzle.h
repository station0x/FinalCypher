//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPAttachment.h"
#include "LPSPMuzzle.generated.h"

UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ALPSPMuzzle final : public ALPSPAttachment
{
	GENERATED_BODY()

	/**Returns the value of SocketName.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Muzzle")
	FName GetSocketName() const { return SocketName; }

	/**Returns the value of FiringCue.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Muzzle")
	USoundCue* GetFiringCue() const { return FiringCue; }

	/**Returns the value of FlashParticles.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Muzzle")
	UParticleSystem* GetFlashParticles() const { return FlashParticles; }

	/**Returns the value of bOverheats.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Muzzle")
	bool ShouldOverheat() const { return bOverheats; }

	/**Returns the value of ShotsToOverheat.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Muzzle")
	int GetShotsToOverheat() const { return ShotsToOverheat; }

	/**Returns the value of OverheatParticles.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, Category = "Low Poly Shooter Pack | Muzzle")
	UParticleSystem* GetOverheatParticles() const { return OverheatParticles; }

private:
	/**Socket from the muzzle mesh used to spawn the firing effects.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Muzzle | General Settings")
	FName SocketName = "SOCKET_Emitter";

	/**Sound played when firing.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Muzzle | General Settings")	
	USoundCue* FiringCue;

	/**Muzzle flash particles played when firing a weapon with this muzzle selected.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Muzzle | General Settings")		
	UParticleSystem* FlashParticles;

	/**Should this muzzle overheat and play some nice particles?*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Muzzle | Overheating")			
	bool bOverheats = true;

	/**Amount of shots required to make this muzzle overheat (plays smoke particles once it does).*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Muzzle | Overheating",
		meta = (DisplayName = "Overheat Shots", EditCondition = "bOverheats"))
	int ShotsToOverheat = 12;

	/**Particles played when this muzzle overheats.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Muzzle | Overheating", meta = (EditCondition = "bOverheats"))		
	UParticleSystem* OverheatParticles;
};