// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerComponent.h"

#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
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

void UUIManagerComponent::Server_NotifyMenuClosed_Implementation(APlayerController* InteractorController)
{
	OnMenuClosed.Broadcast(InteractorController);
}

void UUIManagerComponent::LoadAndCreateMenu(TSubclassOf<UDynamicMenuControllerBase> ControllerClass,
                                            const TSoftClassPtr<UUserWidget>& WidgetClass,
                                            const FInstancedStruct& Payload)
{
	if (ControllerClass == nullptr || WidgetClass == nullptr)
	{
		return;
	}

	const FSoftObjectPath WidgetPath = WidgetClass.ToSoftObjectPath();
	const UAssetManager& AssetManager = UAssetManager::Get();
	AssetManager.GetStreamableManager().RequestAsyncLoad(WidgetPath, FStreamableDelegate::CreateWeakLambda(this, 
		[this, ControllerClass, WidgetClass, Payload]()
		{
			UClass* ResolvedWidgetClass = WidgetClass.Get();
			if (ResolvedWidgetClass == nullptr)
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

			UUserWidget* NewMenu = CreateWidget<UUserWidget>(PlayerController, ResolvedWidgetClass);
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
			FInputModeUIOnly InputMode;
			InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
			InputMode.SetWidgetToFocus(CurrentOpenMenu->TakeWidget());
			PlayerController->SetInputMode(InputMode);
			PlayerController->bShowMouseCursor = true;
		}
	));
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
	OnMenuClosed.Broadcast(PlayerController);
	Server_NotifyMenuClosed(PlayerController);
}
