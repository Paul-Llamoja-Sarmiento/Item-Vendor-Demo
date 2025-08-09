
#pragma once

#include "CoreMinimal.h"
#include "ItemBaseDefinition.h"
#include "PotionDefinition.generated.h"

/**
 * 
 */
UCLASS()
class ITEMVENDORDEMO_API UPotionDefinition : public UItemBaseDefinition
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Potion")
	float Duration = 10.f;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Potion"), GetFName());
	}
};
