// Fill out your copyright notice in the Description page of Project Settings.


#include "VendorComponent.h"

#include "UIManagerComponent.h"
#include "ItemVendorDemo/Definitions/VendorDefinition.h"
#include "ItemVendorDemo/UI/Controller/VendorMenuWidgetController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"
#include "ItemVendorDemo/Definitions/ItemDefinition/ItemBaseDefinition.h"


UVendorComponent::UVendorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

bool UVendorComponent::MakeQuote(const FPrimaryAssetId& ItemId, int32 Quantity, int32& OutUnitPrice,
                                 int32& OutTotalPrice) const
{
	if (!IsServer() || LoadedVendorDefinition == nullptr || Quantity <= 0 || !ItemId.IsValid())
	{
		return false;
	}
	const auto* Found = LoadedVendorDefinition->Inventory.FindByPredicate(
		[&ItemId](const FVendorItemRef& Ref) { return Ref.ItemId == ItemId; });

	if (Found == nullptr)
	{
		return false;
	}

	int UnitPriceFound = 0;
	if (!ResolveUnitPrice(ItemId, Found->PriceOverride, UnitPriceFound))
	{
		return false;
	}

	OutUnitPrice = UnitPriceFound;
	OutTotalPrice = UnitPriceFound * Quantity;	
	return true;
}

void UVendorComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	ClearBindings();

	Super::EndPlay(EndPlayReason);
}


void UVendorComponent::OpenForPlayer(APlayerController* InteractorController)
{
	if (!IsServer() || !IsValid(InteractorController) || CurrentInteractors.Contains(InteractorController))
	{
		return;
	}

	if (!IsValid(LoadedVendorDefinition))
	{
		UObject* VendorDefinitionObj = VendorDefinition.ToSoftObjectPath().TryLoad();
		LoadedVendorDefinition = Cast<UVendorDefinition>(VendorDefinitionObj);
		if (!IsValid(LoadedVendorDefinition))
		{
			return;
		}
	}

	// TODO:
	// Check other interaction validations here, like distance, etc.
	BuildAndSendOpenScreen(InteractorController);
}

void UVendorComponent::BeginPlay()
{
	Super::BeginPlay();

	if (IsServer())
	{
		InitializeFromDefinitionAsync();
	}
}

bool UVendorComponent::IsServer() const
{
	return GetOwner() && GetOwner()->HasAuthority();
}

void UVendorComponent::BuildAndSendOpenScreen(APlayerController* InteractorController)
{
	if (!IsValid(InteractorController))
	{
		return;
	}

	UUIManagerComponent* UIManagerComponent = InteractorController->FindComponentByClass<UUIManagerComponent>();
	if (!IsValid(UIManagerComponent))
	{
		return;
	}

	TSoftClassPtr<UUserWidget> MenuWidgetClass = LoadedVendorDefinition->VendorWidgetClass;

	FUIScreenRecipe Recipe;
	Recipe.UIMenuTag = InteractiveMenuTag;
	Recipe.WidgetClass = MenuWidgetClass;

	FVendorScreenPayload Payload;
	Payload.VendorId = LoadedVendorDefinition->GetPrimaryAssetId();
	Payload.VendorActor = GetOwner();

	UIManagerComponent->Client_OpenScreen(Recipe, FInstancedStruct::Make<FVendorScreenPayload>(Payload));
	UIManagerComponent->OnMenuClosed.AddDynamic(this, &UVendorComponent::OnMenuClosedForPlayer);
	CurrentInteractors.Add(InteractorController);
}

void UVendorComponent::InitializeFromDefinitionAsync()
{
	if (!VendorDefinition.IsValid())
	{
		if (VendorDefinition.ToSoftObjectPath().IsNull())
		{
			return;
		}

		const UAssetManager& AssetManager = UAssetManager::Get();
		TSharedPtr<FStreamableHandle> Handle = AssetManager.GetStreamableManager().RequestAsyncLoad(
			VendorDefinition.ToSoftObjectPath(), FStreamableDelegate::CreateWeakLambda(this, [this]()
			{
				LoadedVendorDefinition = VendorDefinition.Get();
				if (!IsValid(LoadedVendorDefinition))
				{
					UE_LOG(LogTemp, Error, TEXT("Failed to load vendor definition: %s"),
					       *VendorDefinition.ToSoftObjectPath().ToString());
				}
			}));

		if (Handle)
		{
			PendingAssetLoads.Add(MoveTemp(Handle));
		}
	}
	else
	{
		LoadedVendorDefinition = VendorDefinition.Get();
	}
}

void UVendorComponent::OnMenuClosedForPlayer(APlayerController* InteractorController)
{
	if (IsValid(InteractorController))
	{
		auto Manager = InteractorController->FindComponentByClass<UUIManagerComponent>();
		if (IsValid(Manager))
		{
			Manager->OnMenuClosed.RemoveDynamic(this, &UVendorComponent::OnMenuClosedForPlayer);
		}
	}

	CurrentInteractors.Remove(InteractorController);
}

void UVendorComponent::ClearBindings()
{
	for (const APlayerController* PC : CurrentInteractors)
	{
		if (IsValid(PC))
		{
			auto Manager = PC->FindComponentByClass<UUIManagerComponent>();
			if (IsValid(Manager))
			{
				Manager->OnMenuClosed.RemoveDynamic(this, &UVendorComponent::OnMenuClosedForPlayer);
			}
		}
	}

	CurrentInteractors.Empty();
}

bool UVendorComponent::ResolveUnitPrice(const FPrimaryAssetId& ItemId, int32 PriceOverride, int32& OutUnitPrice) const
{
	if (!IsServer())
	{
		return false;
	}

	if (PriceOverride >= 0)
	{
		OutUnitPrice = PriceOverride;
		return true;
	}

	const UAssetManager& AssetManager = UAssetManager::Get();
	UObject* Obj = AssetManager.GetPrimaryAssetObject(ItemId);
	if (Obj == nullptr)
	{
		const FSoftObjectPath Path = AssetManager.GetPrimaryAssetPath(ItemId);
		Obj = Path.TryLoad();
	}

	const UItemBaseDefinition* ItemDefinition = Cast<UItemBaseDefinition>(Obj);
	if (ItemDefinition == nullptr)
	{
		return false;
	}

	OutUnitPrice = ItemDefinition->BasePrice;
	return true;
}
