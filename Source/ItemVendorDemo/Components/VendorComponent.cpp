// Fill out your copyright notice in the Description page of Project Settings.


#include "VendorComponent.h"

#include "UIManagerComponent.h"
#include "ItemVendorDemo/Definitions/VendorDefinition.h"
#include "ItemVendorDemo/UI/Controller/VendorMenuWidgetController.h"
#include "Blueprint/UserWidget.h"
#include "Engine/AssetManager.h"


UVendorComponent::UVendorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

bool UVendorComponent::MakeQuote(const FPrimaryAssetId& ItemId, int32 Quantity, int32& UnitPrice,
                                 int32& TotalPrice) const
{
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
