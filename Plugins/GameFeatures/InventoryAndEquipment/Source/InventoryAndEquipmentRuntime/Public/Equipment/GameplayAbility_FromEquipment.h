// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Ability/Abilities/ModularGameplayAbility.h"

#include "GameplayAbility_FromEquipment.generated.h"

class AEquipmentItemInstance;
class UInventoryItemInstance;

/**
 * UGameplayAbility_FromEquipment
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class INVENTORYANDEQUIPMENTRUNTIME_API UGameplayAbility_FromEquipment : public UModularGameplayAbility
{
	GENERATED_BODY()

public:

	UGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, Category="Ability|Equipment")
	AEquipmentItemInstance* GetAssociatedEquipment() const;

	UFUNCTION(BlueprintCallable, Category = "Ability|Inventory")
	UInventoryItemInstance* GetAssociatedItem() const;

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

};
