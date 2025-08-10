// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerWalletComponent.h"

#include "Net/UnrealNetwork.h"

UPlayerWalletComponent::UPlayerWalletComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	SetIsReplicatedByDefault(true);
}

bool UPlayerWalletComponent::TrySpendMoney(int32 Amount)
{
	return TrySpendMoneyInternal(Amount);
}

bool UPlayerWalletComponent::TryAddMoney(int32 Amount)
{
	return TryAddMoneyInternal(Amount);
}


void UPlayerWalletComponent::BeginPlay()
{
	Super::BeginPlay();
	
	TryAddMoney(InitialMoney);
}

void UPlayerWalletComponent::OnRep_CurrentMoney()
{
	// UE_LOG(LogTemp, Warning, TEXT("PWC Client: CurrentMoney changed: %d"), CurrentMoney);
	// GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Cyan, FString::Printf(TEXT("PWC Client: Current Money: %d for PlayerState: %s"), CurrentMoney, *GetOwner()->GetName()));
}

bool UPlayerWalletComponent::TrySpendMoneyInternal(int32 Amount)
{
	if (!DoesOwnerHaveAuthority() || CurrentMoney < Amount || Amount <= 0)
	{
		return false;
	}

	CurrentMoney -= Amount;
	return true;
}

bool UPlayerWalletComponent::TryAddMoneyInternal(int32 Amount)
{
	if (!DoesOwnerHaveAuthority() || Amount <= 0)
	{
		return false;
	}

	CurrentMoney += Amount;
	return true;
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


