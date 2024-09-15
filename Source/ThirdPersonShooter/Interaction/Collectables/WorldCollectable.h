// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "Interaction/WorldInteractable.h"
#include "Inventory/IPickupable.h"

#include "WorldCollectable.generated.h"

class UObject;
struct FInteractionQuery;

/**
 * Allows an actor to be picked up by a user and added to an inventory / grant abilities.
 * 
 * TODO: Should this inherit from AWorldInteractable? Guess it depends on what we're going for ('E to Pickup' vs auto pickup)
 */
UCLASS(Abstract, Blueprintable)
class THIRDPERSONSHOOTER_API AWorldCollectable : public AWorldInteractable, public IPickupable
{
	GENERATED_BODY()

public:

	AWorldCollectable();

	virtual FInventoryPickup GetPickupInventory() const override;

protected:

	UPROPERTY(EditAnywhere)
	FInventoryPickup StaticInventory;
};
