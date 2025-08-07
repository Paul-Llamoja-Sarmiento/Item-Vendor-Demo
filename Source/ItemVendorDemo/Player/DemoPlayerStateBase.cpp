// Fill out your copyright notice in the Description page of Project Settings.


#include "DemoPlayerStateBase.h"

#include "ItemVendorDemo/Components/PlayerWalletComponent.h"


ADemoPlayerStateBase::ADemoPlayerStateBase()
{
	bReplicates = true;
	PlayerWalletComponent = CreateDefaultSubobject<UPlayerWalletComponent>(TEXT("PlayerWalletComponent"));
}
