// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayTagContainer.h"
#include "InventoryItemInstance.h"
#include "InventoryItemDefinition.h"

#include "InventoryFragment_SetStats.generated.h"

class AInventoryItemInstance;
class UObject;
struct FGameplayTag;

UCLASS()
class UInventoryFragment_SetStats : public UInventoryItemFragment
{
	GENERATED_BODY()

protected:

	// List of initial stats for item (starting magazine ammo, etc.)
	UPROPERTY(EditDefaultsOnly, Category=Equipment)
	TMap<FGameplayTag, int32> InitialItemStats;

public:
	virtual void OnInstanceCreated(AInventoryItemInstance* Instance) const override;

	int32 GetItemStatByTag(FGameplayTag Tag) const;
};
