// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerControllerBase.h"

#include "ItemVendorDemo/Components/PlayerTransactionComponent.h"

ADemoPlayerControllerBase::ADemoPlayerControllerBase()
{
	VendorInteractionComponent = CreateDefaultSubobject<UPlayerTransactionComponent>(TEXT("VendorInteractionComponent"));
}
