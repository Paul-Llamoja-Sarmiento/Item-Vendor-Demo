// Fill out your copyright notice in the Description page of Project Settings.


#include "UIManagerComponent.h"


UUIManagerComponent::UUIManagerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UUIManagerComponent::Client_OpenScreen_Implementation(const FUIScreenRecipe& Recipe)
{
	if (GetOwner()->GetLocalRole() != ROLE_AutonomousProxy)
	{
		return;
	}
	
	if (const auto ControllerClass = MenuControllersRegistry.Find(Recipe.UIMenuTag))
	{
		LoadAndCreateMenu(*ControllerClass, Recipe.WidgetClass);
	}

}

void UUIManagerComponent::LoadAndCreateMenu(TSubclassOf<UDynamicMenuControllerBase> ControllerClass,
	TSubclassOf<UUserWidget> WidgetClass)
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, "Loading Menu...");
}
