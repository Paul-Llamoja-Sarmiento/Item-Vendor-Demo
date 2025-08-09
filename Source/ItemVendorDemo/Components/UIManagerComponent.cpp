// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerComponent.h"

#include "Blueprint/UserWidget.h"
#include "ItemVendorDemo/UI/Controller/DynamicMenuControllerBase.h"


UUIManagerComponent::UUIManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UUIManagerComponent::Client_OpenScreen_Implementation(const FUIScreenRecipe& Recipe, const FInstancedStruct& Payload)
{
	if (GetOwner()->GetLocalRole() != ROLE_AutonomousProxy)
	{
		return;
	}
	
	if (const auto ControllerClass = MenuControllersRegistry.Find(Recipe.UIMenuTag))
	{
		LoadAndCreateMenu(*ControllerClass, Recipe.WidgetClass, Payload);
	}

}

void UUIManagerComponent::LoadAndCreateMenu(TSubclassOf<UDynamicMenuControllerBase> ControllerClass,
                                            TSubclassOf<UUserWidget> WidgetClass,
                                            const FInstancedStruct& Payload)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Loading Menu...");

	if (ControllerClass == nullptr || WidgetClass == nullptr)
	{
		return;
	}

	if (IsValid(CurrentOpenMenu) || IsValid(CurrentMenuController))
	{
		// TODO:
		// Close the current menu if it exists
	}

	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	UUserWidget* NewMenu = CreateWidget<UUserWidget>(PlayerController, WidgetClass);
	UDynamicMenuControllerBase* MenuController = NewObject<UDynamicMenuControllerBase>(this, ControllerClass);
	if (!IsValid(NewMenu) || !IsValid(MenuController))
	{
		return;
	}

	MenuController->SetOwnerPlayerController(PlayerController);
	MenuController->InitializeMenu(NewMenu, Payload);

	CurrentOpenMenu = NewMenu;
	CurrentMenuController = MenuController;

	NewMenu->AddToViewport();

	// Set input mode to UI only
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(NewMenu->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}
