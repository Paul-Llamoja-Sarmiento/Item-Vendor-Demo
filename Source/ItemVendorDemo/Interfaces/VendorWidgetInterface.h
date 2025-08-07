// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VendorWidgetInterface.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnPurchaseButtonClickedInternal);
DECLARE_DYNAMIC_DELEGATE(FOnPurchaseButtonClicked);

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExitButtonClickedInternal);
DECLARE_DYNAMIC_DELEGATE(FOnExitButtonClicked);

// This class does not need to be modified.
UINTERFACE()
class UVendorWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class ITEMVENDORDEMO_API IVendorWidgetInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:

	/* Purchase Button binding*/
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IBindToOnPurchaseButtonClicked(const FOnPurchaseButtonClicked& Event);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IUnbindFromOnPurchaseButtonClicked(const FOnPurchaseButtonClicked& Event);

	/* Exit Button binding */
	
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IBindToOnExitButtonClicked(const FOnExitButtonClicked& Event);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void IUnbindFromOnExitButtonClicked(const FOnExitButtonClicked& Event);
};
