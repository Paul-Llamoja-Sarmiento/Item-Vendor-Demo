// Fill out your copyright notice in the Description page of Project Settings.


#include "VendorMenuWidgetController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "ItemVendorDemo/Components/PlayerTransactionComponent.h"
#include "ItemVendorDemo/Definitions/ItemDefinition/ItemBaseDefinition.h"


void UVendorMenuWidgetController::InitializeMenu(UUserWidget* InMenu, const FInstancedStruct& Payload)
{
	check(InMenu->GetClass()->ImplementsInterface(UVendorWidgetInterface::StaticClass()));
	
	Super::InitializeMenu(InMenu, Payload);

	ResolveMenuPayload(Payload);
}

void UVendorMenuWidgetController::BeginDestroy()
{
	CleanPendingAssetLoads();
	
	Super::BeginDestroy();
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
	CurrentVendorActor = nullptr;
	CleanPendingAssetLoads();
	Super::CloseMenu();
}

void UVendorMenuWidgetController::ResolveMenuPayload(const FInstancedStruct& Payload)
{
	if (!Payload.IsValid() || !Payload.GetScriptStruct()->IsChildOf(FVendorScreenPayload::StaticStruct()))
	{
		return;
	}

	const FVendorScreenPayload& VendorPayload = Payload.Get<FVendorScreenPayload>();
	CurrentVendorActor = VendorPayload.VendorActor;
	OpenVendorMenu(VendorPayload.VendorId);
}

void UVendorMenuWidgetController::OpenVendorMenu(const FPrimaryAssetId& VendorId)
{
	ShowLoadingScreen(true);

	const UAssetManager& AssetManager = UAssetManager::Get();
	const FSoftObjectPath VendorPath = AssetManager.GetPrimaryAssetPath(VendorId);
	TSharedPtr<FStreamableHandle> Handle = AssetManager.GetStreamableManager().RequestAsyncLoad(
		VendorPath, FStreamableDelegate::CreateWeakLambda(this, [this, VendorId]()
		{
			LoadItemsForVendor(VendorId);
		}
		));

	if (Handle.IsValid())
	{
		PendingAssetLoads.Add(MoveTemp(Handle));		
	}	
}

void UVendorMenuWidgetController::LoadItemsForVendor(const FPrimaryAssetId& VendorId)
{
	const UAssetManager& AssetManager = UAssetManager::Get();
	UObject* VendorObj = AssetManager.GetPrimaryAssetObject(VendorId);
	if (VendorObj == nullptr)
	{
		ShowLoadingScreen(false);
		return;
	}
	
	UVendorDefinition* VendorDefinition = Cast<UVendorDefinition>(VendorObj);
	if (VendorDefinition == nullptr)
	{
		ShowLoadingScreen(false);
		return;
	}

	TArray<FSoftObjectPath> ItemPaths;
	ItemPaths.Reserve(VendorDefinition->Inventory.Num());

	for (const auto& ItemRef : VendorDefinition->Inventory)
	{
		ItemPaths.Add(AssetManager.GetPrimaryAssetPath(ItemRef.ItemId));
	}
	
	TSharedPtr<FStreamableHandle> Handle = AssetManager.GetStreamableManager().RequestAsyncLoad(
		ItemPaths, FStreamableDelegate::CreateWeakLambda(this, [this, VendorDefinition]()
		{
			PushViewDataToMenu(*VendorDefinition);
		}
		));

	if (Handle.IsValid())
	{
		PendingAssetLoads.Add(MoveTemp(Handle));
	}
}

void UVendorMenuWidgetController::PushViewDataToMenu(const UVendorDefinition& VendorDefinition)
{
	const UAssetManager& AssetManager = UAssetManager::Get();
	TArray<FVendorViewData> VendorViewData;
	VendorViewData.Reserve(VendorDefinition.Inventory.Num());
	if (MenuReference == nullptr)
	{
		ShowLoadingScreen(false);
		return;
	}

	for (const auto& ItemRef : VendorDefinition.Inventory)
	{
		UObject* VendorObj = AssetManager.GetPrimaryAssetObject(ItemRef.ItemId);
		const UItemBaseDefinition* ItemDefinition = Cast<UItemBaseDefinition>(VendorObj);
		if (ItemDefinition == nullptr)
		{
			continue;
		}

		FVendorViewData ViewData;
		ViewData.ItemId = ItemRef.ItemId;
		ViewData.DisplayName = ItemDefinition->DisplayName;
		ViewData.Description = ItemDefinition->Description;
		ViewData.IconTexture = ItemDefinition->IconTexture;
		ViewData.BasePrice = ItemRef.PriceOverride == -1 ? ItemDefinition->BasePrice : ItemRef.PriceOverride;
		VendorViewData.Add(MoveTemp(ViewData));
	}

	IVendorWidgetInterface::Execute_ISetVendorData(MenuReference, VendorViewData);
	ShowLoadingScreen(false);
}

void UVendorMenuWidgetController::ShowLoadingScreen(bool bShow) const
{
	if (MenuReference != nullptr)
	{
		IVendorWidgetInterface::Execute_IShowLoadingScreen(MenuReference, bShow);
	}
}

void UVendorMenuWidgetController::CleanPendingAssetLoads()
{
	for (const auto& Handle : PendingAssetLoads)
	{
		if (Handle.IsValid())
		{
			Handle->CancelHandle();
		}
	}
	
	PendingAssetLoads.Empty();
}


void UVendorMenuWidgetController::OnPurchaseButtonClicked(FPrimaryAssetId ItemId, int32 Quantity)
{
	if (OwnerPlayerController != nullptr && ItemId.IsValid())
	{
		UPlayerTransactionComponent* TransactionComponent = OwnerPlayerController->FindComponentByClass<UPlayerTransactionComponent>();
		TransactionComponent->Server_RequestPurchase(CurrentVendorActor.Get(), ItemId, Quantity);
	}
}

void UVendorMenuWidgetController::OnExitButtonClicked()
{
	CloseMenu();
}
