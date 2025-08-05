
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"


class UCapsuleComponent;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class ITEMVENDORDEMO_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/*
	 * Called from the client side by the interact-input
	 */
	UFUNCTION(BlueprintCallable)
	void TryInteract();
	
protected:	
	virtual void BeginPlay() override;

private:
	UObject* CurrentInteractableObject = nullptr;

	UCapsuleComponent* OwnerCapsuleComponent = nullptr;

	void BindToOwnerCapsuleEvents();

	void UnbindFromOwnerCapsuleEvents();

	UFUNCTION()
	void OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                           UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep,
	                           const FHitResult& SweepResult);

	UFUNCTION()
	void OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	                         UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	UFUNCTION(Server, Reliable)
	void Server_RequestInteract(UObject* InteractableActor);
};
