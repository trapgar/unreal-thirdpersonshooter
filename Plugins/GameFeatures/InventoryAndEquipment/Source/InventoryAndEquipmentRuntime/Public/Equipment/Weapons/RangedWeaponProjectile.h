// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilities/ModularAbilityAttenuatorInterface.h"
#include "GameplayAbilities/ModularGameplayEffectContext.h"

#include "RangedWeaponProjectile.generated.h"

class UProjectileMovementComponent;
class UCapsuleComponent;
class URangedWeaponItemInstance;

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

	// Maximum lifetime of the projectile in seconds - actor will be destroyed after this if it hasn't hit anything yet.
	// 0.0f will never expire.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ForceUnits="s"))
	float MaxLifeTime = 5.0f;

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn = true))
	URangedWeaponItemInstance* Weapon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(Categories="GameplayCue"))
	FGameplayTag GameplayCueToExecuteOnImpact;

public:
	// Creates a new effect context for the projectile, where the ranged weapon is the ability source for attenuation.
	UFUNCTION(BlueprintCallable)
	FGameplayEffectContextHandle MakeEffectContext() const;

private:

	void OnExpiration();

	FTimerHandle ExpirationTimerHandle;

};
