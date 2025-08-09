// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMenuControllerBase.h"
#include "ItemVendorDemo/Interfaces/VendorWidgetInterface.h"
#include "VendorMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class ITEMVENDORDEMO_API UVendorMenuWidgetController : public UDynamicMenuControllerBase
{
	GENERATED_BODY()

public:
	/* DynamicMenuControllerBase */
	virtual void InitializeMenu(UUserWidget* InMenu) override;

protected:
	FOnPurchaseButtonClicked PurchaseButtonClickedDelegate;
	
	FOnExitButtonClicked ExitButtonClickedDelegate;
	
	virtual void BindToMenuInterface() override;

	virtual void ClearControllerBindings() override;

	virtual void CloseMenu() override;

private:
	
	UFUNCTION()
	void OnPurchaseButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();
};
