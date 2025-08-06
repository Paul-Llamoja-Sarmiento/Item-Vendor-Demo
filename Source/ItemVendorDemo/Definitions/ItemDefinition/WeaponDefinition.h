
#pragma once

#include "CoreMinimal.h"
#include "itemBaseDefinition.h"
#include "WeaponDefinition.generated.h"

/**
 * 
 */
UCLASS()
class ITEMVENDORDEMO_API UWeaponDefinition : public UitemBaseDefinition
{
	GENERATED_BODY()
	
public:
	UPROPERTY(EditDefaultsOnly, Category="Weapon", meta=(AssetBundles="Gameplay"))
	USkeletalMesh* Mesh = nullptr;

	virtual FPrimaryAssetId GetPrimaryAssetId() const override
	{
		return FPrimaryAssetId(TEXT("Weapon"), GetFName());
	}
};
