//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "LPSPCharacter.h"
#include "Blueprint/UserWidget.h"
#include "LPSPCrosshair.generated.h"

/**Base Crosshair class. Any class inheriting from this one will only need to implement the visual part of the Crosshair!*/
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ULPSPCrosshair final : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/**Should this crosshair be visible?*/
	UFUNCTION(BlueprintPure, Category = "Low Poly Shooter Pack | Crosshair", meta = (DisplayName = "Get Slate Visibility"))
	ESlateVisibility GetSlateVisibility();

private:
	/**Player.*/
	UPROPERTY()
	TWeakObjectPtr<ALPSPCharacter> Character;

	/**Updates the reference to the player character.*/
	void TryGetPlayerCharacter();
};