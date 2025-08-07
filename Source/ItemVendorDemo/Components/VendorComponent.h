// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Components/ActorComponent.h"
#include "VendorComponent.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMVENDORDEMO_API UVendorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UVendorComponent();

protected:
	virtual void BeginPlay() override;
};
