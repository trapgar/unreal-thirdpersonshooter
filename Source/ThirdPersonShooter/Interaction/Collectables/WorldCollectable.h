// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Interaction/WorldInteractable.h"
#include "Inventory/IPickupable.h"
#include "Inventory/InventoryItemDefinition.h"

#include "WorldCollectable.generated.h"

class UObject;
struct FInteractionQuery;

/**
 * AWorldCollectable
 * 
 * Allows an actor to be picked up by a user and added to an inventory / grant abilities.
 */
UCLASS(Abstract, Blueprintable)
class THIRDPERSONSHOOTER_API AWorldCollectable : public AWorldInteractable, public IPickupable
{
	GENERATED_BODY()

public:

	AWorldCollectable()
	{
		// Set default subtext if there is only 1 item in the box
		if (StaticInventory.Instances.Num() <= 0 && StaticInventory.Templates.Num() == 1)
		{
			FPickupTemplate Template = StaticInventory.Templates[0];
			UInventoryItemDefinition* ItemDef = Template.ItemDef->GetDefaultObject<UInventoryItemDefinition>();

			Option.SubText = ItemDef->DisplayName;
		}
	};

	virtual FInventoryPickup GetPickupInventory() const override
	{
		return StaticInventory;
	};

protected:

	UPROPERTY(EditAnywhere, Category = Default)
	FInventoryPickup StaticInventory;
};
