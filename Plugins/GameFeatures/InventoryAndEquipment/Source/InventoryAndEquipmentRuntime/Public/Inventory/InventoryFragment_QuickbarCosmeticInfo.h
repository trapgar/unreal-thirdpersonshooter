#pragma once

#include "InventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_QuickbarCosmeticInfo.generated.h"

class UInventoryItemDefinition;
class UObject;

/**
 * EQuickbarCosmeticInfoLifeTime
 *
 * Defines when a cosmetic item should be spawned.
 */
UENUM(BlueprintType)
enum class EQuickbarCosmeticInfoLifeTime : uint8
{
	// Spawned while the item is in the quickbar.
	Persistent,

	// Spawned only while the item is *not* actively being used.
	WhileNotInUse,

	MAX	UMETA(Hidden)
};


// --------------------------------------------------------


UCLASS(BlueprintType)
class INVENTORYANDEQUIPMENTRUNTIME_API UInventoryFragment_QuickbarCosmeticInfo : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	// Actor class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	TSubclassOf<AActor> ActorToSpawn;

	// When to spawn the actor
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	EQuickbarCosmeticInfoLifeTime LifeTime = EQuickbarCosmeticInfoLifeTime::Persistent;

	// Socket tag to attach the spawned actor to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	FName AttachSocketName;

	// Transform to apply to the spawned actor
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Default)
	FTransform AttachTransform;

	void OnInstanceAdded(UInventoryItemInstance* Instance);

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
