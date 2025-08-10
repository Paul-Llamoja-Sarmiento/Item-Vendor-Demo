// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerTransactionComponent.generated.h"


class UNiagaraComponent;
class UNiagaraSystem;
class UPlayerInventoryComponent;
class UPlayerWalletComponent;

// TODO: Add transaction id to avoid processing the same request multiple times
USTRUCT(BlueprintType)
struct FPurchaseResult
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly)
	bool bSuccess = false;

	UPROPERTY(BlueprintReadOnly)
	FPrimaryAssetId ItemId;

	UPROPERTY(BlueprintReadOnly)
	int32 UnitPrice = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 TotalPrice = 0;

	UPROPERTY(BlueprintReadOnly)
	int32 Quantity = 0;

	UPROPERTY(BlueprintReadOnly)
	FText ErrorMessage = FText();
};

/*
 * Player Controller component for handling vendor requests
 */

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMVENDORDEMO_API UPlayerTransactionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UPlayerTransactionComponent();

	UFUNCTION(Server, Reliable)
	void Server_RequestPurchase(AActor* VendorActor, const FPrimaryAssetId& ItemId, int32 Quantity);

	UFUNCTION(Client, Reliable)
	void Client_PurchaseResult(const FPurchaseResult& Result);

protected:
	// Coupled here for simplicity
	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UNiagaraSystem> PurchaseEffect;

	FTimerHandle PurchaseTimerHandle;
	
	virtual void BeginPlay() override;

private:
	UPlayerWalletComponent* GetPlayerWalletComponent() const;

	UPlayerInventoryComponent* GetInventoryComponent() const;
};
