// Fill out your copyright notice in the Description page of Project Settings.


#include "VendorComponent.h"


UVendorComponent::UVendorComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}


void UVendorComponent::BeginPlay()
{
	Super::BeginPlay();
}

