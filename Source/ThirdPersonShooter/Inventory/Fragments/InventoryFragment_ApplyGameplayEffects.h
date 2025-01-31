// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryItemDefinition.h"

#include "InventoryFragment_ApplyGameplayEffects.generated.h"

class UInventoryItemInstance;
class UObject;
struct FGameplayTag;

/**
 * InventoryFragment_ApplyGameplayEffects
 * 
 * Set initial stats for inventory item - starting ammo, etc.
 */
UCLASS()
class UInventoryFragment_ApplyGameplayEffects : public UInventoryItemFragment
{
	GENERATED_BODY()

protected:

	// List of initial stats for item (starting magazine ammo, etc.)
	UPROPERTY(EditDefaultsOnly, Category=Effects, meta=(DisplayName="Gameplay Effect List"))
	TArray<TSubclassOf<class UGameplayEffect>> GameplayEffectsToApply;

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;
};
