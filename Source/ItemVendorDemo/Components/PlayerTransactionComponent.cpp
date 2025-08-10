// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerTransactionComponent.h"

#include "PlayerInventoryComponent.h"
#include "PlayerWalletComponent.h"
#include "VendorComponent.h"
#include "GameFramework/PlayerState.h"


UPlayerTransactionComponent::UPlayerTransactionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UPlayerTransactionComponent::Server_RequestPurchase_Implementation(AActor* VendorActor, const FPrimaryAssetId& ItemId, int32 Quantity)
{
	if (!ItemId.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("RequestPurchase_Implementation: Invalid ItemId"));
		return;
	}
	
	FPurchaseResult Result;
	Result.ItemId = ItemId;
	Result.Quantity = Quantity;

	APlayerController* PlayerController = Cast<APlayerController>(GetOwner());
	if (PlayerController == nullptr || Quantity <= 0 || !IsValid(VendorActor))
	{
		Result.ErrorMessage = FText::FromString("Invalid purchase request.");
		Result.bSuccess = false;
		Client_PurchaseResult(Result);
		return;
	}

	// Other distance/interaction validations

	UVendorComponent* VendorComponent = VendorActor->FindComponentByClass<UVendorComponent>();
	int32 UnitPrice = 0;
	int32 TotalPrice = 0;
	if (!IsValid(VendorComponent) || !VendorComponent->MakeQuote(ItemId, Quantity, UnitPrice, TotalPrice))
	{
		Result.ErrorMessage = FText::FromString("Vendor could not process the request.");
		Result.bSuccess = false;
		Client_PurchaseResult(Result);
		return;
	}

	Result.UnitPrice = UnitPrice;
	Result.TotalPrice = TotalPrice;

	UPlayerWalletComponent* PlayerWalletComponent = GetPlayerWalletComponent();
	if (!IsValid(PlayerWalletComponent) || !PlayerWalletComponent->TrySpendMoney(TotalPrice))
	{
		Result.ErrorMessage = FText::FromString("Insufficient funds.");
		Client_PurchaseResult(Result);
		return;
	}

	UPlayerInventoryComponent* InventoryComponent = GetInventoryComponent();
	if (IsValid(InventoryComponent))
	{
		InventoryComponent->AddItem(ItemId, Quantity);
	}
	
	Result.bSuccess = true;
	Client_PurchaseResult(Result);	
}


void UPlayerTransactionComponent::Client_PurchaseResult_Implementation(const FPurchaseResult& Result)
{
	UE_LOG(LogTemp, Warning, TEXT("TODO: Client_PurchaseResult_Implementation"));
}

void UPlayerTransactionComponent::BeginPlay()
{
	Super::BeginPlay();
}

UPlayerWalletComponent* UPlayerTransactionComponent::GetPlayerWalletComponent() const
{
	if (GetOwner() == nullptr || !GetOwner()->HasAuthority())
	{
		return nullptr;
	}

	APlayerState* PlayerState = Cast<APlayerController>(GetOwner())->PlayerState;
	return PlayerState ? PlayerState->FindComponentByClass<UPlayerWalletComponent>() : nullptr;
}

UPlayerInventoryComponent* UPlayerTransactionComponent::GetInventoryComponent() const
{
	if (GetOwner() == nullptr || !GetOwner()->HasAuthority())
	{
		return nullptr;
	}

	APlayerState* PlayerState = Cast<APlayerController>(GetOwner())->PlayerState;
	return PlayerState ? PlayerState->FindComponentByClass<UPlayerInventoryComponent>() : nullptr;
}

