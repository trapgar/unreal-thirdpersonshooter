// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InventoryItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "InventoryFragment_EquippableItem.generated.h"

class UEquipmentItemDefinition;
class UObject;

UCLASS()
class UInventoryFragment_EquippableItem : public UInventoryItemFragment
{
	GENERATED_BODY()

public:

	// Item definition for the equipment to be added
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Equipment)
	TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
