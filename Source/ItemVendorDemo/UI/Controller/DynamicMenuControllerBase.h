// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InstancedStruct.h"
#include "UObject/Object.h"
#include "DynamicMenuControllerBase.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDynamicMenuClosed);

/**
 * 
 */
UCLASS(Abstract)
class ITEMVENDORDEMO_API UDynamicMenuControllerBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitializeMenu(UUserWidget* InMenu, const FInstancedStruct& Payload);

	void SetOwnerPlayerController(APlayerController* InPlayerController);

	UPROPERTY()
	FOnDynamicMenuClosed OnDynamicMenuClosed;

protected:
	UPROPERTY()
	TObjectPtr<UUserWidget> MenuReference = nullptr;

	APlayerController* OwnerPlayerController = nullptr;

	virtual void BindToMenuInterface() { }

	virtual void ClearControllerBindings() { }

	virtual void CloseMenu();
};
