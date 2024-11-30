// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryItemDefinition.h"

#include "InventoryFragment_SetStatsOfExisting.generated.h"

class UInventoryItemInstance;
class UObject;
struct FGameplayTag;

/**
 * UInventoryFragment_SetStatsOfExisting
 *
 * Updates the stats of an existing inventory item by item definition (e.g.: ammo box).
 */
UCLASS()
class UInventoryFragment_SetStatsOfExisting : public UInventoryItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

protected:

	// List of additional stats to add to the item (ammo, etc.)
	UPROPERTY(EditDefaultsOnly, Category=Inventory)
	TMap<FGameplayTag, int32> AdditionalItemStats;

	// Definition of the inventory item to find & set stats for
	UPROPERTY(EditDefaultsOnly, Category=Inventory)
	TSubclassOf<UInventoryItemDefinition> ItemDefinition;
};
