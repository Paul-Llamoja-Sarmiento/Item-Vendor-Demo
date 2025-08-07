// Fill out your copyright notice in the Description page of Project Settings.


#include "FeedbackWidgetComponent.h"

#include "Components/BoxComponent.h"
#include "Components/WidgetComponent.h"


UFeedbackWidgetComponent::UFeedbackWidgetComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UFeedbackWidgetComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UnbindFromBoxComponentEvents();
	
	Super::EndPlay(EndPlayReason);
}


void UFeedbackWidgetComponent::BeginPlay()
{
	Super::BeginPlay();

	SetFeedbackWidget();
	BindToBoxComponentEvents();
}

void UFeedbackWidgetComponent::OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && IsValid(FeedbackWidget) && OtherActor->GetLocalRole() == ROLE_AutonomousProxy)
	{
		FeedbackWidget->SetVisibility(ESlateVisibility::Visible);
	}
}

void UFeedbackWidgetComponent::OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (IsValid(OtherActor) && IsValid(FeedbackWidget) && OtherActor->GetLocalRole() == ROLE_AutonomousProxy)
	{
		FeedbackWidget->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UFeedbackWidgetComponent::SetFeedbackWidget()
{
	UWidgetComponent* FeedbackWidgetComponent = GetOwner()->FindComponentByTag<UWidgetComponent>(FeedbackWidgetTag);
	if (!IsValid(FeedbackWidgetComponent))
	{
		return;
	}
	
	FeedbackWidget = FeedbackWidgetComponent->GetUserWidgetObject();
	if (!IsValid(FeedbackWidget))
	{
		return;
	}

	FeedbackWidget->SetVisibility(ESlateVisibility::Hidden);
}


void UFeedbackWidgetComponent::BindToBoxComponentEvents()
{
	BoxCollisionComponent = GetOwner()->FindComponentByClass<UBoxComponent>();
	if (IsValid(BoxCollisionComponent))
	{
		BoxCollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &UFeedbackWidgetComponent::OnBoxBeginOverlap);
		BoxCollisionComponent->OnComponentEndOverlap.AddDynamic(this, &UFeedbackWidgetComponent::OnBoxEndOverlap);
	}	
}

void UFeedbackWidgetComponent::UnbindFromBoxComponentEvents()
{
	if (IsValid(BoxCollisionComponent))
	{
		BoxCollisionComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UFeedbackWidgetComponent::OnBoxBeginOverlap);
		BoxCollisionComponent->OnComponentEndOverlap.RemoveDynamic(this, &UFeedbackWidgetComponent::OnBoxEndOverlap);
		BoxCollisionComponent = nullptr;
	}
}

