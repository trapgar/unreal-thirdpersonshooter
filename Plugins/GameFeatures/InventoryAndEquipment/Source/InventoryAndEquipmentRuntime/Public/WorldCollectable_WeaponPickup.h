// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "WorldCollectable.h"

#include "WorldCollectable_WeaponPickup.generated.h"

class UCapsuleComponent;

/**
 * AWorldCollectable_WeaponPickup
 * 
 * Represents a world collectable containing a weapon that can be picked up by the player.
 */
UCLASS(Blueprintable, BlueprintType)
class INVENTORYANDEQUIPMENTRUNTIME_API AWorldCollectable_WeaponPickup : public AWorldCollectable
{
	GENERATED_BODY()

public:

	AWorldCollectable_WeaponPickup();

public:

	// Collision volume
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CollisionVolume;
};
