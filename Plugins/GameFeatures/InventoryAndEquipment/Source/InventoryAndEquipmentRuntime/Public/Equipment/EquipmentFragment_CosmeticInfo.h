#pragma once

#include "EquipmentItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "EquipmentFragment_CosmeticInfo.generated.h"

class UEquipmentItemDefinition;
class UObject;

/**
 * EEquipmentCosmeticInfoLifeTime
 *
 * Defines when a cosmetic item should be spawned.
 */
UENUM(BlueprintType)
enum class EEquipmentCosmeticInfoLifeTime : uint8
{
	// Spawned only while the item is actively being used.
	WhileInUse,

	MAX	UMETA(Hidden)
};


// --------------------------------------------------------


UCLASS(BlueprintType, Const)
class UEquipmentFragment_CosmeticInfo : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:
	// Actor class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	TSubclassOf<AActor> ActorToSpawn;

	// When to spawn the actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	EEquipmentCosmeticInfoLifeTime LifeTime = EEquipmentCosmeticInfoLifeTime::WhileInUse;

	// Socket tag to attach the spawned actor to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	FName AttachSocketName;

	// Transform to apply to the spawned actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Default)
	FTransform AttachTransform;

	void OnInstanceCreated(UEquipmentItemInstance* Instance) const override;
	void OnInstanceDestroyed(UEquipmentItemInstance* Instance) const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
