// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "VendorDefinition.generated.h"


USTRUCT(BlueprintType)
struct FVendorItemRef
{
	GENERATED_BODY()
	UPROPERTY(EditDefaultsOnly)
	FPrimaryAssetId ItemId;

	// Override the price of the item, if -1 then use the base price from the item definition
	UPROPERTY(EditDefaultsOnly)
	int32 PriceOverride = -1;

	// If set to -1, then the item can be purchased infinitely; otherwise it will be limited to this stock amount
	UPROPERTY(EditDefaultsOnly)
	int32 Stock = -1;
};


/**
 * 
 */
UCLASS()
class ITEMVENDORDEMO_API UVendorDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly)
	FText VendorName;
	
	UPROPERTY(EditDefaultsOnly)
	TArray<FVendorItemRef> Inventory;
	
	UPROPERTY(EditDefaultsOnly)
	TSoftClassPtr<UUserWidget> VendorWidgetClass;

	// TODO: Add optional rules, if needed.

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Vendor"), GetFName());
	}
};
