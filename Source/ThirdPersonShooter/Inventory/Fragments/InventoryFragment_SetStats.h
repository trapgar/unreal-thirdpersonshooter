// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryItemDefinition.h"

#include "InventoryFragment_SetStats.generated.h"

class UInventoryItemInstance;
class UObject;
struct FGameplayTag;

/**
 * UInventoryFragment_SetStats
 * 
 * Set initial stats for inventory item - starting ammo, etc.
 */
UCLASS()
class UInventoryFragment_SetStats : public UInventoryItemFragment
{
	GENERATED_BODY()

protected:

	// List of initial stats for item (starting magazine ammo, etc.)
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;
};
