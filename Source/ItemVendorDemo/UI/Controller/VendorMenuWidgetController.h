// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DynamicMenuControllerBase.h"
#include "Engine/StreamableManager.h"
#include "ItemVendorDemo/Definitions/VendorDefinition.h"
#include "ItemVendorDemo/Interfaces/VendorWidgetInterface.h"
#include "VendorMenuWidgetController.generated.h"


USTRUCT(BlueprintType)
struct FVendorScreenPayload
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FPrimaryAssetId VendorId = FPrimaryAssetId();

	UPROPERTY(BlueprintReadWrite)
	TWeakObjectPtr<AActor> VendorActor = nullptr;
};


/**
 * 
 */
UCLASS()
class ITEMVENDORDEMO_API UVendorMenuWidgetController : public UDynamicMenuControllerBase
{
	GENERATED_BODY()

public:
	/* DynamicMenuControllerBase */
	virtual void InitializeMenu(UUserWidget* InMenu, const FInstancedStruct& Payload) override;

	virtual void BeginDestroy() override;

protected:
	FOnPurchaseButtonClicked PurchaseButtonClickedDelegate;
	
	FOnExitButtonClicked ExitButtonClickedDelegate;
	
	virtual void BindToMenuInterface() override;

	virtual void ClearControllerBindings() override;

	virtual void CloseMenu() override;

private:
	UPROPERTY()
	FPrimaryAssetId CurrentVendorId;

	UPROPERTY()
	TWeakObjectPtr<AActor> CurrentVendorActor = nullptr;

	TArray<TSharedPtr<FStreamableHandle>> PendingAssetLoads;
	
	void ResolveMenuPayload(const FInstancedStruct& Payload);

	void OpenVendorMenu(const FPrimaryAssetId& VendorId);

	void LoadItemsForVendor(const FPrimaryAssetId& VendorId);

	void PushViewDataToMenu(const UVendorDefinition& VendorDefinition);

	void ShowLoadingScreen(bool bShow) const;

	void CleanPendingAssetLoads();
	
	UFUNCTION()
	void OnPurchaseButtonClicked();

	UFUNCTION()
	void OnExitButtonClicked();
};
