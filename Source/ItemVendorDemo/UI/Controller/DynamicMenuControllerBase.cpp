// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicMenuControllerBase.h"

#include "Blueprint/UserWidget.h"

void UDynamicMenuControllerBase::InitializeMenu(UUserWidget* InMenu)
{
	Menu = InMenu;
	BindToMenuInterface();
}

void UDynamicMenuControllerBase::CloseMenu()
{
	ClearControllerBindings();
	if (IsValid(Menu))
	{
		Menu->RemoveFromParent();	
	}

	Menu = nullptr;
}
