// Fill out your copyright notice in the Description page of Project Settings.


#include "DynamicMenuControllerBase.h"

#include "Blueprint/UserWidget.h"

void UDynamicMenuControllerBase::InitializeMenu(UUserWidget* InMenu, const FInstancedStruct& Payload)
{
	MenuReference = InMenu;
	BindToMenuInterface();
}

void UDynamicMenuControllerBase::SetOwnerPlayerController(APlayerController* InPlayerController)
{
	OwnerPlayerController = InPlayerController;
}

void UDynamicMenuControllerBase::CloseMenu()
{
	ClearControllerBindings();
	if (IsValid(MenuReference))
	{
		MenuReference->RemoveFromParent();	
	}

	MenuReference = nullptr;
	OnDynamicMenuClosed.Broadcast();
}
