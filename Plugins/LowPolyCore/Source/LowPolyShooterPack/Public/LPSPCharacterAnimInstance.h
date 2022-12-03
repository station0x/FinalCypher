//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPCharacter.h"
#include "Animation/AnimInstance.h"
#include "LPSPCharacterAnimInstance.generated.h"

/**Base Animation Instance Class used for all Characters.*/
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ULPSPCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()

protected:
	/**Character Pointer.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	TWeakObjectPtr<ALPSPCharacter> Character;

	/**Is the character aiming?*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	bool bAiming;

	/**Returns true if the character is in the air.*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	bool bFalling;

	/**
	*Returns true if the character is running.
	*Can be false even while the character is running, if the animation shouldn't be playing.
	*/
	UPROPERTY(BlueprintReadOnly, Category = "Low Poly Shooter Pack")
	bool bRunning;

	/**Returns true if the Character is currently running.*/
	UFUNCTION(BlueprintPure, BlueprintCallable, meta = (BlueprintThreadSafe), Category = "Low Poly Shooter Pack | Character")
	bool IsRunning() const
	{
		//Return if character is invalid (Editor).
		if(!Character.IsValid())
			return false;
		
		//No running animation while falling or aiming.
		if(bFalling || bAiming)
			return false;

		//Return.
		return Character->IsRunning();
	}

public:	
	/**Animation value update.*/
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
};