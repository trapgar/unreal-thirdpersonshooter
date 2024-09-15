#pragma once

#include "InventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_QuickbarCosmeticInfo.generated.h"

class UInventoryItemDefinition;
class UObject;

/**
 * UInventoryFragment_QuickbarCosmeticInfo
 * 
 * Fragment containing cosmetic information for an inventory item that should be spawned when it is put in the Quickbar.
 */
UCLASS(BlueprintType, Const)
class THIRDPERSONSHOOTER_API UInventoryFragment_QuickbarCosmeticInfo : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	// Actor class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Cosmetic)
	TSubclassOf<AActor> ActorToSpawn;

	// Socket tag to attach the spawned actor to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Cosmetic)
	FName AttachSocketName;

	// Transform to apply to the spawned actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Cosmetic)
	FTransform AttachTransform;

	void OnInstanceAdded(UInventoryItemInstance* Instance);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
