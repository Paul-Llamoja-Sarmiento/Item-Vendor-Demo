// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FeedbackWidgetComponent.generated.h"


class UBoxComponent;
class UWidgetComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMVENDORDEMO_API UFeedbackWidgetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFeedbackWidgetComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly)
	FName FeedbackWidgetTag = "feedback-widget";
	
	UUserWidget* FeedbackWidget = nullptr;

	UBoxComponent* BoxCollisionComponent = nullptr;

private:
	UFUNCTION()
	void OnBoxBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							   UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
							   const FHitResult& SweepResult);

	UFUNCTION()
	void OnBoxEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
							 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	void SetFeedbackWidget();

	void BindToBoxComponentEvents();

	void UnbindFromBoxComponentEvents();
};
