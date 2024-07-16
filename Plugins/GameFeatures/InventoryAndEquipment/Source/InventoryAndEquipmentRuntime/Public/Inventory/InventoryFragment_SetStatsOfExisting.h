// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "InventoryItemInstance.h"
#include "InventoryItemDefinition.h"

#include "InventoryFragment_SetStatsOfExisting.generated.h"

class UInventoryItemInstance;
class UObject;
struct FGameplayTag;

UCLASS()
class UInventoryFragment_SetStatsOfExisting : public UInventoryItemFragment
{
	GENERATED_BODY()

protected:

	// List of additional stats for item (starting magazine ammo, etc.)
	UPROPERTY(EditDefaultsOnly, Category=Inventory)
	TMap<FGameplayTag, int32> AdditionalItemStats;

	// Definition of the equipment item to find & set stats for
	UPROPERTY(EditDefaultsOnly, Category=Inventory)
	TSubclassOf<UInventoryItemDefinition> ItemDefinition;

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const override;
};
