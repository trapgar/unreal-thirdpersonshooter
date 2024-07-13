// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldCollectable_WeaponPickup.h"
#include "Components/CapsuleComponent.h"

AWorldCollectable_WeaponPickup::AWorldCollectable_WeaponPickup()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
	CollisionVolume->InitCapsuleSize(80.f, 80.f);

	bReplicates = true;
}
