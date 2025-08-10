// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"

#include "Engine/AssetManager.h"
#include "ItemVendorDemo/Definitions/ItemDefinition/ItemBaseDefinition.h"
#include "Net/UnrealNetwork.h"


UPlayerInventoryComponent::UPlayerInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
	SetIsReplicatedByDefault(true);
}

void UPlayerInventoryComponent::AddItem(const FPrimaryAssetId& ItemId, int32 Quantity)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return;
	}

	Inventory.AddItem(ItemId, Quantity);
}

bool UPlayerInventoryComponent::RemoveItem(const FPrimaryAssetId& ItemId, int32 Quantity)
{
	if (GetOwnerRole() != ROLE_Authority)
	{
		return false;
	}
	
	return Inventory.RemoveItem(ItemId, Quantity);
}

int32 UPlayerInventoryComponent::GetQuantity(const FPrimaryAssetId& ItemId) const
{
	return Inventory.GetQuantity(ItemId);
}

void UPlayerInventoryComponent::DebugPrintInventory(bool bToScreen) const
{
	const FString InventoryString = TEXT("[Inventory] ===========");
	if (bToScreen && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, InventoryString);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *InventoryString);

	if (Inventory.Items.Num() == 0)
	{
		const FString EmptyString = TEXT("[Inventory] (empty)");
		if (bToScreen && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, EmptyString);
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *EmptyString);
		return;
	}

	const UAssetManager& AssetManager = UAssetManager::Get();
	for (const FInventoryItem& Item : Inventory.Items)
	{
		FString ItemName = Item.ItemId.ToString();
		const int32 Quantity = Item.Quantity;
		FString Line = FString::Printf(TEXT(" - %s x %d"), *ItemName, Quantity);
		
		if (UObject* Obj = AssetManager.GetPrimaryAssetObject(Item.ItemId))
		{
			if (const UItemBaseDefinition* ItemDef = Cast<UItemBaseDefinition>(Obj))
			{
				Line += FString::Printf(TEXT(" [%s]"), *ItemDef->DisplayName.ToString());
			}
		}

		if (bToScreen && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, Line);
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *Line);
	}
}

void UPlayerInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPlayerInventoryComponent, Inventory, COND_OwnerOnly);
}


void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

