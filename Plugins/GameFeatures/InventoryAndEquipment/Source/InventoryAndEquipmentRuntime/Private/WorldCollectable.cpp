// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldCollectable.h"

#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WorldCollectable)

struct FInteractionQuery;

AWorldCollectable::AWorldCollectable()
{
}

FInventoryPickup AWorldCollectable::GetPickupInventory() const
{
	return StaticInventory;
}
