
#pragma once

#include "CoreMinimal.h"
#include "itemBaseDefinition.h"
#include "ResourceDefinition.generated.h"

/**
 * 
 */
UCLASS()
class ITEMVENDORDEMO_API UResourceDefinition : public UitemBaseDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Resource")
	int32 MaxStack = 99;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Resource"), GetFName());
	}
};
