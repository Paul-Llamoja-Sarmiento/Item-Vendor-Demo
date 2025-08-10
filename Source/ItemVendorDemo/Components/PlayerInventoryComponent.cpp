// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerInventoryComponent.h"

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

void UPlayerInventoryComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(UPlayerInventoryComponent, Inventory, COND_OwnerOnly);
}


void UPlayerInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
}

