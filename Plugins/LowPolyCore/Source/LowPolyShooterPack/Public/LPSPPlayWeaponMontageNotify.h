//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "LPSPPlayWeaponMontageNotify.generated.h"

/**Plays a Montage on the Character's Weapon.*/
UCLASS(meta = (DisplayName = "Play Weapon Montage"))
class LOWPOLYSHOOTERPACK_API ULPSPPlayWeaponMontageNotify final : public UAnimNotify
{
	GENERATED_BODY()

public:
	//Begin UAnimNotify interface.
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override { return "Play Weapon Montage"; };
	//End UAnimNotify interface.

private:
	/**Montage to play on the weapon skeletal mesh.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	UAnimMontage* Montage;
};