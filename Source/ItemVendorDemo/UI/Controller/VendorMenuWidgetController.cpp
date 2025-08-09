// Fill out your copyright notice in the Description page of Project Settings.


#include "VendorMenuWidgetController.h"

#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "ItemVendorDemo/Definitions/ItemDefinition/ItemBaseDefinition.h"


void UVendorMenuWidgetController::InitializeMenu(UUserWidget* InMenu, const FInstancedStruct& Payload)
{
	check(InMenu->GetClass()->ImplementsInterface(UVendorWidgetInterface::StaticClass()));
	
	Super::InitializeMenu(InMenu, Payload);

	ApplyMenuPayload(Payload);
}

void UVendorMenuWidgetController::BeginDestroy()
{
	for (const auto& Handle : PendingAssetLoads)
	{
		if (Handle.IsValid())
		{
			Handle->CancelHandle();
		}
	}
	
	PendingAssetLoads.Empty();
	
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
	// Remove the menu from viewport
	Super::CloseMenu();
}

void UVendorMenuWidgetController::ApplyMenuPayload(const FInstancedStruct& Payload)
{
	if (!Payload.IsValid() || !Payload.GetScriptStruct()->IsChildOf(FVendorScreenPayload::StaticStruct()))
	{
		return;
	}

	const FVendorScreenPayload& VendorPayload = Payload.Get<FVendorScreenPayload>();
	OpenVendorMenu(VendorPayload.VendorId);
}

void UVendorMenuWidgetController::OpenVendorMenu(const FPrimaryAssetId& VendorId)
{
	CurrentVendorId = VendorId;
	ShowLoadingScreen(true);

	const UAssetManager& AssetManager = UAssetManager::Get();
	const FSoftObjectPath VendorPath = AssetManager.GetPrimaryAssetPath(CurrentVendorId);
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


void UVendorMenuWidgetController::OnPurchaseButtonClicked()
{
	// Handle purchase 
}

void UVendorMenuWidgetController::OnExitButtonClicked()
{
	CloseMenu();
}
