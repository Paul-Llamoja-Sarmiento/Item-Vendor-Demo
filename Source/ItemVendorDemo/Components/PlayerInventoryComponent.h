// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "PlayerInventoryComponent.generated.h"

USTRUCT(BlueprintType)
struct FInventoryItem: public FFastArraySerializerItem
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	FPrimaryAssetId ItemId;

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity;
};


USTRUCT(BlueprintType)
struct FInventoryList: public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<FInventoryItem> Items;

	
	/* Helper functions. Server Only */
	
	int32 FindIndex(const FPrimaryAssetId& ItemId) const
	{
		for (int32 i = 0; i < Items.Num(); i++)
		{
			if (Items[i].ItemId == ItemId)
			{
				return i;
			}
		}
		return INDEX_NONE;
	}
	
	void AddItem(const FPrimaryAssetId& ItemId, int32 Quantity)
	{
		if (Quantity <= 0 || !ItemId.IsValid())
		{
			return;
		}

		const int32 Index = FindIndex(ItemId);
		if (Index == INDEX_NONE)
		{
			FInventoryItem& NewItem = Items.AddDefaulted_GetRef();
			NewItem.ItemId = ItemId;
			NewItem.Quantity = Quantity;
			MarkItemDirty(NewItem);
		}
		else
		{
			FInventoryItem& Item = Items[Index];
			Item.Quantity += Quantity;
			MarkItemDirty(Item);
		}
	}

	bool RemoveItem(const FPrimaryAssetId& ItemId, int32 Quantity)
	{
		if (Quantity <= 0 || !ItemId.IsValid())
		{
			return false;
		}

		const int32 Index = FindIndex(ItemId);
		if (Index == INDEX_NONE)
		{
			return false;
		}

		FInventoryItem& Item = Items[Index];
		if (Item.Quantity < Quantity)
		{
			return false;
		}

		Item.Quantity -= Quantity;
		if (Item.Quantity <= 0)
		{
			Items.RemoveAt(Index);
			return true;
		}
		
		MarkItemDirty(Item);
		return true;
	}

	int32 GetQuantity(const FPrimaryAssetId& ItemId) const
	{
		const int32 Index = FindIndex(ItemId);
		if (Index == INDEX_NONE)
		{
			return 0;
		}
		
		return Items[Index].Quantity;
	}
};


/*
 * Player State component for managing the player's inventory
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMVENDORDEMO_API UPlayerInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerInventoryComponent();

	/*
	 * Adds an item to the player's inventory. Should be called on the server.
	 * @param ItemId The ID of the item to add.
	 * @param Quantity The number of items to add.
	 */
	void AddItem(const FPrimaryAssetId& ItemId, int32 Quantity);

	/*
	 * Removes an item from the player's inventory. Should be called on the server.
	 * @param ItemId The ID of the item to remove.
	 * @param Quantity The number of items to remove.
	 * @return True if the item was successfully removed, false if not enough items were available.
	 */
	bool RemoveItem(const FPrimaryAssetId& ItemId, int32 Quantity);

	int32 GetQuantity(const FPrimaryAssetId& ItemId) const;
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(Replicated)
	FInventoryList Inventory;

	virtual void BeginPlay() override;
};
