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
class UNiagaraComponent;

/**
 * ARangedWeaponProjectile
 * 
 * Projectile fire from a ranged weapon.
 */
UCLASS(Abstract, Blueprintable, HideCategories=("Actor Tick", "Component Tick", "Input", "HLOD"))
class THIRDPERSONSHOOTER_API ARangedWeaponProjectile : public AActor
{
	GENERATED_BODY()

public:

	ARangedWeaponProjectile();

public:

	void OnConstruction(const FTransform& Transform) override;

public:

	// The projectile's collision volume.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UCapsuleComponent> CollisionVolume;

	// The projectile's movement component.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta=(DisplayName="Projectile Movement Component"))
	UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(BlueprintReadOnly, meta=(ExposeOnSpawn = true))
	URangedWeaponItemInstance* Weapon;

public:
	// Creates a new effect context for the projectile, where the ranged weapon is the ability source for attenuation.
	UFUNCTION(BlueprintCallable)
	FGameplayEffectContextHandle MakeEffectContext() const;

	/** Function called when projectile hits something */
	UFUNCTION()
	void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Default")
	TObjectPtr<UNiagaraComponent> ImpactDecal;

	/** Please add a variable description */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category="Components")
	TObjectPtr<UNiagaraComponent> ImpactSystem;

private:
	FVector StartingLocation;

};
