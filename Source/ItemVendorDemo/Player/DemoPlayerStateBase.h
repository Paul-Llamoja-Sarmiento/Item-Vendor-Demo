// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "DemoPlayerStateBase.generated.h"

class UPlayerWalletComponent;
/**
 * 
 */
UCLASS(Abstract)
class ITEMVENDORDEMO_API ADemoPlayerStateBase : public APlayerState
{
	GENERATED_BODY()

public:
	ADemoPlayerStateBase();

protected:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UPlayerWalletComponent> PlayerWalletComponent;
};
