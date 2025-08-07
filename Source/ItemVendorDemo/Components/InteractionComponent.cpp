
#include "InteractionComponent.h"

#include "Components/CapsuleComponent.h"
#include "ItemVendorDemo/Interfaces/InteractableInterface.h"


UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UInteractionComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		UnbindFromOwnerCapsuleEvents();
	}
	
	Super::EndPlay(EndPlayReason);
}

void UInteractionComponent::TryInteract()
{
	if (!GetOwner() || GetOwner()->HasAuthority() || !IsValid(CurrentInteractableObject))
	{
		return;
	}
	
	Server_RequestInteract(CurrentInteractableObject);
}


void UInteractionComponent::BeginPlay()
{
	Super::BeginPlay();

	// Bind to overlap event only on clients
	if (GetOwner() && !GetOwner()->HasAuthority())
	{
		BindToOwnerCapsuleEvents();
	}
}

void UInteractionComponent::BindToOwnerCapsuleEvents()
{
	OwnerCapsuleComponent = GetOwner()->FindComponentByClass<UCapsuleComponent>();
	if (IsValid(OwnerCapsuleComponent))
	{
		OwnerCapsuleComponent->OnComponentBeginOverlap.AddDynamic(this, &UInteractionComponent::OnCapsuleBeginOverlap);
		OwnerCapsuleComponent->OnComponentEndOverlap.AddDynamic(this, &UInteractionComponent::OnCapsuleEndOverlap);
	}	
}

void UInteractionComponent::UnbindFromOwnerCapsuleEvents()
{
	if (IsValid(OwnerCapsuleComponent))
	{
		OwnerCapsuleComponent->OnComponentBeginOverlap.RemoveDynamic(this, &UInteractionComponent::OnCapsuleBeginOverlap);
		OwnerCapsuleComponent->OnComponentEndOverlap.RemoveDynamic(this, &UInteractionComponent::OnCapsuleEndOverlap);
		OwnerCapsuleComponent = nullptr;
	}
}

void UInteractionComponent::OnCapsuleBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                                  UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (IsValid(OtherActor) && OtherActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		// We are considering that we only can interact with one interactable at a time.
		// If we want to support multiple interactables, we can use an array or a set to store them.
		CurrentInteractableObject = OtherActor;
	}
}

void UInteractionComponent::OnCapsuleEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (CurrentInteractableObject == OtherActor)
	{
		CurrentInteractableObject = nullptr;
	}
}

void UInteractionComponent::Server_RequestInteract_Implementation(UObject* InteractableActor)
{
	if (!GetOwner()->HasAuthority())
	{
		return;
	}
	
	if (IsValid(InteractableActor) && InteractableActor->GetClass()->ImplementsInterface(UInteractableInterface::StaticClass()))
	{
		// Do some verifications depending on the game logic, like if the player is close enough to the interactable, etc.
		IInteractableInterface::Execute_IInteract(InteractableActor, GetOwner());
	}
}


