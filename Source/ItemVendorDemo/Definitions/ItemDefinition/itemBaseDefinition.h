// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "itemBaseDefinition.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType)
class ITEMVENDORDEMO_API UitemBaseDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Item|UI")
	FText DisplayName = FText();

	UPROPERTY(EditDefaultsOnly, Category="Item|UI")
	FText Description = FText();
	
	UPROPERTY(EditDefaultsOnly, Category="Item|UI")
	UTexture2D* IconTexture = nullptr;
	
	UPROPERTY(EditDefaultsOnly, Category="Item|Shop")
	int32 BasePrice = 10;
	
	// virtual FPrimaryAssetId GetPrimaryAssetId() const override
	// {
	// 	const FPrimaryAssetType Type =
	// 		UAssetManager::Get().Class GetPrimaryAsset GetPrimaryAssetTypeFromClass(GetClass());
	// 	return FPrimaryAssetId(Type, GetFName());
	// }
};
