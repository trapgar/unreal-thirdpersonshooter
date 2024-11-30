#pragma once

#include "Equipment/EquipmentItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "EquipmentFragment_CosmeticInfo.generated.h"

class UEquipmentItemDefinition;
class UObject;


/**
 * UEquipmentFragment_CosmeticInfo
 * 
 * Spawns a cosmetic actor while the equipment is in use.
 */
UCLASS(BlueprintType, Const)
class UEquipmentFragment_CosmeticInfo : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:
	// Actor class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Cosmetics)
	TSubclassOf<AActor> ActorToSpawn;

	// Socket tag to attach the spawned actor to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Cosmetics)
	FName AttachSocketName;

	// Transform to apply to the spawned actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Cosmetics)
	FTransform AttachTransform;

	void OnInstanceCreated(UEquipmentItemInstance* Instance) const override;
	void OnInstanceDestroyed(UEquipmentItemInstance* Instance) const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
