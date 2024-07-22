#pragma once

#include "EquipmentItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "EquipmentFragment_TransformQuickbarCosmeticInfo.generated.h"

class UEquipmentItemDefinition;
class UObject;


/**
 * UEquipmentFragment_TransformQuickbarCosmeticInfo
 * 
 * Finds an actor of the specified type from the InventoryItemInstance and updates its transform.
 * This is used by equipment like guns that are visible in the quickbar (i.e. holstered on the players hip), and then swap to the hand on equip.
 */
UCLASS(BlueprintType, Const)
class UEquipmentFragment_TransformQuickbarCosmeticInfo : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:
	// Actor class to find
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Cosmetic)
	TSubclassOf<AActor> ActorToFind;

	// Socket tag to attach the spawned actor to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Cosmetic)
	FName AttachSocketName;

	// Transform to apply to the spawned actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Cosmetic)
	FTransform AttachTransform;

	void OnInstanceCreated(UEquipmentItemInstance* Instance) const override;
	// void OnInstanceDestroyed(UEquipmentItemInstance* Instance) const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
