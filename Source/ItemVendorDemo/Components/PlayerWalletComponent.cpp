// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWalletComponent.h"

#include "Net/UnrealNetwork.h"

UPlayerWalletComponent::UPlayerWalletComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

void UPlayerWalletComponent::Server_SpendMoney_Implementation(int32 Amount)
{
	if (Amount > 0)
	{
		TrySpendMoneyInternal(Amount);
	}
}

void UPlayerWalletComponent::Server_AddMoney_Implementation(int32 Amount)
{
	if (Amount > 0)
	{
		AddMoneyInternal(Amount);
	}
}


void UPlayerWalletComponent::BeginPlay()
{
	Super::BeginPlay();

	if (!DoesOwnerHaveAuthority())
	{
		Server_AddMoney(InitialMoney);
	}
}

void UPlayerWalletComponent::OnRep_CurrentMoney()
{
	// UE_LOG(LogTemp, Warning, TEXT("PWC Client: CurrentMoney changed: %d"), CurrentMoney);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("PWC Client: Current Money: %d for PlayerState: %s"), CurrentMoney, *GetOwner()->GetName()));
}

void UPlayerWalletComponent::TrySpendMoneyInternal(int32 Amount)
{
	if (!DoesOwnerHaveAuthority() || CurrentMoney < Amount || Amount <= 0)
	{
		return;
	}

	CurrentMoney -= Amount;
}

void UPlayerWalletComponent::AddMoneyInternal(int32 Amount)
{
	if (!DoesOwnerHaveAuthority())
	{
		return;
	}

	CurrentMoney += Amount;
}

bool UPlayerWalletComponent::DoesOwnerHaveAuthority() const
{
	return IsValid(GetOwner()) && GetOwner()->HasAuthority();
}

void UPlayerWalletComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	DOREPLIFETIME_CONDITION(UPlayerWalletComponent, CurrentMoney, COND_OwnerOnly);
}


