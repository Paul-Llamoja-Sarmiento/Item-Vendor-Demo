
#pragma once

#include "CoreMinimal.h"
#include "itemBaseDefinition.h"
#include "PotionDefinition.generated.h"

/**
 * 
 */
UCLASS()
class ITEMVENDORDEMO_API UPotionDefinition : public UitemBaseDefinition
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
