// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "DynamicMenuControllerBase.generated.h"

/**
 * 
 */
UCLASS(Abstract)
class ITEMVENDORDEMO_API UDynamicMenuControllerBase : public UObject
{
	GENERATED_BODY()

public:
	virtual void InitializeMenu(UUserWidget* InMenu);

protected:
	UPROPERTY()
	TObjectPtr<UUserWidget> Menu;

	virtual void BindToMenuInterface() { }

	virtual void ClearControllerBindings() { }

	virtual void CloseMenu();
};
