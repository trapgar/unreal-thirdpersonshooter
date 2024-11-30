// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Equipment/EquipmentItemDefinition.h"
#include "Styling/SlateBrush.h"

#include "EquipmentFragment_QuickbarIcon.generated.h"

class UObject;

/**
 * UEquipmentFragment_QuickbarIcon
 * 
 * UI Brush to display the equipment in the quickbar.
 */
UCLASS()
class UEquipmentFragment_QuickbarIcon : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=Appearance)
	FSlateBrush WeaponCard;
};
