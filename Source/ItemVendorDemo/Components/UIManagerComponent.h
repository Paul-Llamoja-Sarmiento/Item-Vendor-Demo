// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "InstancedStruct.h"
#include "Components/ActorComponent.h"
#include "UIManagerComponent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMenuClosed,APlayerController*, InteractorController);

class UDynamicMenuControllerBase;


USTRUCT(BlueprintType)
struct FUIScreenRecipe
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	FGameplayTag UIMenuTag = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	TSoftClassPtr<UUserWidget> WidgetClass = nullptr;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract, Blueprintable)
class ITEMVENDORDEMO_API UUIManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUIManagerComponent();

	UPROPERTY(BlueprintAssignable)
	FOnMenuClosed OnMenuClosed;

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_OpenScreen(const FUIScreenRecipe& Recipe, const FInstancedStruct& Payload);

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TMap<FGameplayTag, TSubclassOf<UDynamicMenuControllerBase>> MenuControllersRegistry;

	UFUNCTION(Server, Reliable)
	void Server_NotifyMenuClosed(APlayerController* InteractorController);

private:
	UPROPERTY()
	TObjectPtr<UUserWidget> CurrentOpenMenu = nullptr;

	UPROPERTY()
	TObjectPtr<UDynamicMenuControllerBase> CurrentMenuController = nullptr;

	void LoadAndCreateMenu(TSubclassOf<UDynamicMenuControllerBase> ControllerClass,
	                       const TSoftClassPtr<UUserWidget>& WidgetClass,
	                       const FInstancedStruct& Payload);

	UFUNCTION()
	void OnCurrentMenuClosed();
};
