// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "Engine/StreamableManager.h"
#include "VendorComponent.generated.h"


class UUIManagerComponent;
class UVendorDefinition;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMVENDORDEMO_API UVendorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVendorComponent();

	bool MakeQuote(const FPrimaryAssetId& ItemId, int32 Quantity, int32& OutUnitPrice, int32& OutTotalPrice) const;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Vendor|Configuration")
	TSoftObjectPtr<UVendorDefinition> VendorDefinition;

	UPROPERTY(EditDefaultsOnly, Category = "Vendor|Configuration")
	FGameplayTag InteractiveMenuTag;

	UFUNCTION(BlueprintCallable)
	void OpenForPlayer(APlayerController* InteractorController);
	
	virtual void BeginPlay() override;

private:
	UPROPERTY(Transient)
	TObjectPtr<UVendorDefinition> LoadedVendorDefinition = nullptr;

	TArray<TSharedPtr<FStreamableHandle>> PendingAssetLoads;

	UPROPERTY()
	TArray<APlayerController*> CurrentInteractors;
	
	bool IsServer() const;

	void BuildAndSendOpenScreen(APlayerController* InteractorController);

	void InitializeFromDefinitionAsync();

	UFUNCTION()
	void OnMenuClosedForPlayer(APlayerController* InteractorController);

	void ClearBindings();

	bool ResolveUnitPrice(const FPrimaryAssetId& ItemId, int32 PriceOverride, int32& OutUnitPrice) const;
};
