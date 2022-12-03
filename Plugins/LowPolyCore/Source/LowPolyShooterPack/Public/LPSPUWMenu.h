//Copyright 2021, Infima Games. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "LPSPUWMenu.generated.h"

/**Base class used for menus. Helps with some of the more repetitive elements of making menus.*/
UCLASS(Abstract)
class LOWPOLYSHOOTERPACK_API ULPSPUWMenu final : public UUserWidget
{
	GENERATED_BODY()
	
protected:
	/**Event called to close this menu.*/
	UFUNCTION(BlueprintImplementableEvent, Category = "Low Poly Shooter Pack")
	void OnCloseMenu();
	
	/**On Key Down.*/
	virtual FReply NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent) override;

private:
	/**Array of keys that can close this menu.*/
	UPROPERTY(EditAnywhere, Category = "Low Poly Shooter Pack | Widgets | Menus")
	TArray<FKey> CloseMenuKeys;
};
