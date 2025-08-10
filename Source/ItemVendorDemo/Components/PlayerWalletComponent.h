// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerWalletComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ITEMVENDORDEMO_API UPlayerWalletComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UPlayerWalletComponent();
	
	bool TrySpendMoney(int32 Amount);
	
	bool TryAddMoney(int32 Amount);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	int32 InitialMoney = 100;
	
	virtual void BeginPlay() override;


private:
	UPROPERTY(ReplicatedUsing=OnRep_CurrentMoney)
	int32 CurrentMoney = 0;

	UFUNCTION()
	void OnRep_CurrentMoney();

	bool TrySpendMoneyInternal(int32 Amount);

	bool TryAddMoneyInternal(int32 Amount);

	bool DoesOwnerHaveAuthority() const;
};
