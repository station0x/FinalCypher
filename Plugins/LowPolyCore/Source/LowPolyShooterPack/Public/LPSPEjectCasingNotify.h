//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "LPSPEjectCasingNotify.generated.h"

/**Animation Notify used to spawn casings when a Weapon is fired.*/
UCLASS(meta = (DisplayName = "Eject Casing"))
class LOWPOLYSHOOTERPACK_API ULPSPEjectCasingNotify final : public UAnimNotify
{
	GENERATED_BODY()

public:
	//Begin UAnimNotify interface.
	virtual void Notify(USkeletalMeshComponent* Weapon, UAnimSequenceBase* Animation) override;
	virtual FString GetNotifyName_Implementation() const override { return "Eject Casing"; }
	//End UAnimNotify interface.

private:
	/**Forward offset from the ejection port. Used to allow Casings to not spawn directly inside it.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	float Offset = 1.0f;

	/**Value used to multiply the casing impulse (found in a weapon's magazine).*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack");
	float Impulse = 1.0f;

	/**Determines whether we randomize the initial casing rotation, or use the socket rotation.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")
	bool bRandomizeInitialRotation = true;

	/**Name of the socket whose location and rotation we use to spawn this casing.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack")	
	FName EjectSocketName = "SOCKET_Eject";
};