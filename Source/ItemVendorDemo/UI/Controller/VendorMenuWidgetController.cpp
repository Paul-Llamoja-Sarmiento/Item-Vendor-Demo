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
		IVendorWidgetInterface::Execute_IUnbindFromOnExitButtonClicked(Menu, ExitButtonClickedDelegate);
	}

	if (PurchaseButtonClickedDelegate.IsBound())
	{
		IVendorWidgetInterface::Execute_IUnbindFromOnPurchaseButtonClicked(Menu, PurchaseButtonClickedDelegate);
	}
}

void UVendorMenuWidgetController::BindToMenuInterface()
{
	ExitButtonClickedDelegate.BindDynamic(this, &UVendorMenuWidgetController::OnExitButtonClicked);
	IVendorWidgetInterface::Execute_IBindToOnExitButtonClicked(Menu, ExitButtonClickedDelegate);

	PurchaseButtonClickedDelegate.BindDynamic(this, &UVendorMenuWidgetController::OnPurchaseButtonClicked);
	IVendorWidgetInterface::Execute_IBindToOnPurchaseButtonClicked(Menu, PurchaseButtonClickedDelegate);
}

void UVendorMenuWidgetController::CloseMenu()
{
	// Remove the menu from viewport
	Super::CloseMenu();
}

void UVendorMenuWidgetController::OnPurchaseButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, "Purchase Button Clicked");
	// Handle purchase 
}

void UVendorMenuWidgetController::OnExitButtonClicked()
{
	GEngine->AddOnScreenDebugMessage(-1, 10.f, FColor::Cyan, "Exit Button Clicked");
	CloseMenu();
}
