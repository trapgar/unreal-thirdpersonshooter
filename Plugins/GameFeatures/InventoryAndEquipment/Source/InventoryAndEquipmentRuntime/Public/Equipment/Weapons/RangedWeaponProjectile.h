// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"

#include "RangedWeaponProjectile.generated.h"

class UProjectileMovementComponent;
class UCapsuleComponent;

/**
 * ARangedWeaponProjectile
 * 
 * Projectile fire from a ranged weapon.
 */
UCLASS(Abstract, Blueprintable, HideCategories=("Actor Tick", "Component Tick", "Replication", "Input", "HLOD"))
class INVENTORYANDEQUIPMENTRUNTIME_API ARangedWeaponProjectile : public AActor
{
	GENERATED_BODY()

public:

	ARangedWeaponProjectile();

public:

	void OnConstruction(const FTransform& Transform) override;

	void BeginPlay() override;

	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	// The projectile's collision volume.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CollisionVolume;

	// The projectile's movement component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Projectile Movement Component"))
	UProjectileMovementComponent* ProjectileMovementComponent;

	// Maximum lifetime of the projectile.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (ForceUnits="s"))
	float MaxLifeTime = 5.0f;

	// Initial speed of the projectile.
	UPROPERTY(BlueprintReadWrite, Meta = (ExposeOnSpawn = true))
	float InitialSpeed = 0.0;

private:

	void OnExpiration();

	FTimerHandle ExpirationTimerHandle;

};
