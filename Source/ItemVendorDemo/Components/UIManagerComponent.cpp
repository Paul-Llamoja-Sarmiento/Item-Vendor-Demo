// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerComponent.h"

#include "Blueprint/UserWidget.h"
#include "ItemVendorDemo/UI/Controller/DynamicMenuControllerBase.h"


UUIManagerComponent::UUIManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


// TODO: Block if there is already an open menu
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
	if (ControllerClass == nullptr || WidgetClass == nullptr)
	{
		return;
	}

	if (IsValid(CurrentOpenMenu) || IsValid(CurrentMenuController))
	{
		UE_LOG(LogTemp, Display, TEXT("Clean your previos objects!!!!"));
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

	CurrentOpenMenu = NewMenu;
	CurrentMenuController = MenuController;

	CurrentMenuController->SetOwnerPlayerController(PlayerController);
	CurrentMenuController->InitializeMenu(NewMenu, Payload);
	CurrentMenuController->OnDynamicMenuClosed.AddDynamic(this, &UUIManagerComponent::OnCurrentMenuClosed);
	

	CurrentOpenMenu->AddToViewport();

	// Set input mode to UI only
	FInputModeGameAndUI InputMode;
	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputMode.SetWidgetToFocus(CurrentOpenMenu->TakeWidget());
	PlayerController->SetInputMode(InputMode);
	PlayerController->bShowMouseCursor = true;
}

void UUIManagerComponent::OnCurrentMenuClosed()
{
	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (!IsValid(PlayerController))
	{
		return;
	}

	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
	CurrentMenuController = nullptr;
	CurrentOpenMenu = nullptr;
}
