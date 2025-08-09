// Fill out your copyright notice in the Description page of Project Settings.


#include "VendorMenuWidgetController.h"

#include "Blueprint/UserWidget.h"


void UVendorMenuWidgetController::InitializeMenu(UUserWidget* InMenu, const FInstancedStruct& Payload)
{
	check(InMenu->GetClass()->ImplementsInterface(UVendorWidgetInterface::StaticClass()));
	
	Super::InitializeMenu(InMenu, Payload);
}

void UVendorMenuWidgetController::ClearControllerBindings()
{
	if (ExitButtonClickedDelegate.IsBound())
	{
		IVendorWidgetInterface::Execute_IUnbindFromOnExitButtonClicked(MenuReference, ExitButtonClickedDelegate);
	}

	if (PurchaseButtonClickedDelegate.IsBound())
	{
		IVendorWidgetInterface::Execute_IUnbindFromOnPurchaseButtonClicked(MenuReference, PurchaseButtonClickedDelegate);
	}
}

void UVendorMenuWidgetController::BindToMenuInterface()
{
	ExitButtonClickedDelegate.BindDynamic(this, &UVendorMenuWidgetController::OnExitButtonClicked);
	IVendorWidgetInterface::Execute_IBindToOnExitButtonClicked(MenuReference, ExitButtonClickedDelegate);

	PurchaseButtonClickedDelegate.BindDynamic(this, &UVendorMenuWidgetController::OnPurchaseButtonClicked);
	IVendorWidgetInterface::Execute_IBindToOnPurchaseButtonClicked(MenuReference, PurchaseButtonClickedDelegate);
}

void UVendorMenuWidgetController::CloseMenu()
{
	// Remove the menu from viewport
	Super::CloseMenu();
}

void UVendorMenuWidgetController::OnPurchaseButtonClicked()
{
	// Handle purchase 
}

void UVendorMenuWidgetController::OnExitButtonClicked()
{
	CloseMenu();
}
