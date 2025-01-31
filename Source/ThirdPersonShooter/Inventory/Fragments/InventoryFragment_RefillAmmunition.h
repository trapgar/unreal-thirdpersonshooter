// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryItemDefinition.h"
#include "AbilitySystemGlobals.h"

#include "InventoryFragment_RefillAmmunition.generated.h"

class UInventoryItemInstance;
class UObject;
struct FGameplayTag;

/**
 * InventoryFragment_RefillAmmunition
 * 
 * Set initial stats for inventory item - starting ammo, etc.
 * 
 * @TODO: idk if I want this to be a fragment... feels kinda weird...
 */
UCLASS()
class UInventoryFragment_RefillAmmunition : public UInventoryItemFragment
{
	GENERATED_BODY()

private:

	// 
	FGameplayEffectSpecHandle MakeOutgoingAmmunitionSpec(UInventoryItemInstance* ItemInstance) const;

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;
};
