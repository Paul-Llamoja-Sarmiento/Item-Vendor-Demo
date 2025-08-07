// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "UIManagerComponent.generated.h"

class UDynamicMenuControllerBase;

USTRUCT(BlueprintType)
struct FUIScreenRecipe
{
	GENERATED_BODY()
	
	UPROPERTY(BlueprintReadWrite)
	FGameplayTag UIMenuTag = FGameplayTag();

	UPROPERTY(BlueprintReadWrite)
	TSubclassOf<UUserWidget> WidgetClass = nullptr;
};


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent), Abstract, Blueprintable)
class ITEMVENDORDEMO_API UUIManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UUIManagerComponent();

	UFUNCTION(Client, Reliable, BlueprintCallable)
	void Client_OpenScreen(const FUIScreenRecipe& Recipe);

protected:
	UPROPERTY(EditDefaultsOnly, Category="UI")
	TMap<FGameplayTag, TSubclassOf<UDynamicMenuControllerBase>> MenuControllersRegistry;

private:
	void LoadAndCreateMenu(TSubclassOf<UDynamicMenuControllerBase> ControllerClass, TSubclassOf<UUserWidget> WidgetClass);

};
