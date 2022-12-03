//Copyright 2021, Infima Games. All Rights Reserved.

#include "LPSPUWMenu.h"

FReply ULPSPUWMenu::NativeOnKeyDown(const FGeometry& InGeometry, const FKeyEvent& InKeyEvent)
{
	//Make sure we want to close the menu with this key.
	const FKey Key = InKeyEvent.GetKey();
	if(CloseMenuKeys.Contains(Key))
		OnCloseMenu();

	//Return.
	return FReply::Handled();
}