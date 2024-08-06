#pragma once

#include "Curves/CurveFloat.h"
#include "EquipmentItemDefinition.h"
#include "EquipmentItemInstance.h"
#include "Templates/SubclassOf.h"
#include "Weapons/RangedWeaponProjectile.h"

#include "EquipmentFragment_RangedWeaponStats.generated.h"

class UEquipmentItemInstance;
class UEquipmentItemDefinition;
class UPhysicalMaterial;
class UObject;

/**
 * UEquipmentFragment_RangedWeaponStats
 * 
 * Equipment Fragment containing stats for ranged weapons.
 * 
 * @note Spread is the random angle applied to the projectile's initial spawn transform.
 * @note Recoil is the random jitter applier to the players' camera
 * @note Vertical recoil receives a random multiplier between 0.9 and 1.0 when applied
 */
UCLASS(BlueprintType)
class UEquipmentFragment_RangedWeaponStats : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UEquipmentItemInstance* Instance) const override;

public:

	// Initial velocity of the bullet on projectile spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="cm/s"))
	float MuzzleVelocity = 60000.0f;

	// A curve that maps the distance (in cm) to a multiplier on the base damage from the associated gameplay effect
	// If there is no data in this curve, then the weapon is assumed to have no damage falloff with distance
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits="cm"))
	FRuntimeFloatCurve DistanceDamageFalloff;

	// The radius for bullet traces sweep spheres (0.0 will result in a line trace)
	// TODO: not entirely sure what the benefit of this is... while bullets have a measurable diameter, idk if this actually results in a gameplay difference?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="cm"))
	float BulletTraceSweepRadius = 1.0f;

	// The maximum distance at which this weapon can deal damage - used for despawning
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits="cm"))
	float MaxDamageRange = 50000.0f;

	// Bullet actor class definition to spawn on weapon fire
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics")
	TSubclassOf<ARangedWeaponProjectile> ProjectileToSpawn;

	// Flag indicating if the 1st shot of a heat should be perfectly accurate or not (0 spread applied)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(DisplayName="Has Perfect First Shot Accuracy?"))
	bool bHasFirstShotAccuracy = false;

	// Applied damage type on hit
	// TODO: Should this be an attribute of the projectile instead?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> DamageType;

	// Base amount of damage to apply to a on projectile hit
	// This is pre-multipliers (e.g. damage falloff, headshot, etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects", meta=(ClampMin=0.0f, UIMin=0.0f))
	float SingleBulletDamage;

	// List of special tags that affect how damage is dealt
	// These tags will be compared to tags in the physical material of the thing being hit
	// If more than one tag is present, the multipliers will be combined multiplicatively
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects", meta=(Categories="Gameplay.Zone", ForceForceUnits="x"))
	TMap<FGameplayTag, float> DamageZoneMultipliers;

	// Max rate of fire in rounds per minute
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling", meta=(ClampMin=1.0f, UIMin=1.0f, ForceUnits="rpm"))
	float RoundsPerMinute = 750.0f;

	// List of available fire modes the player can cycle through (e.g. single, semi-auto, full-auto, etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling", meta=(Categories="Equipment.Weapon.Mode"))
	FGameplayTagContainer FireModes;

	// Base spread angle in degrees
	// This is the initial spread angle all the multipliers are applied to
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float SpreadAngleBase = 1.0f;

	// Spread angle multiplier in degrees while aiming down sight
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="x"))
	float SpreadAngleMultiplier_AimDownSight = 0.2f;

	// Spread angle penalty multiplier while hip firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="x"))
	float SpreadAngleMultiplier_HipFire = 3.5f;

	// Spread angle penalty multiplier while moving
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="x"))
	float SpreadAngleMultiplier_Moving = 1.1667f;

	// Degrees to increase the spread angle by per tick (degrees per shot)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float SpreadAngleAccumulationAmount = 0.098f;

	// Degrees to decrease the spread angle by per tick (degrees per second)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float SpreadAngleDecayAmount = 11.1f;

	// Max recoil pitch angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float RecoilPitch = 0.32f;

	// Min recoil yaw angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMax=0.0f, UIMax=0.0f, ForceUnits="deg"))
	float RecoilYawMin = -0.12f;

	// Max recoil yaw angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float RecoilYawMax = 0.28f;

	// Additional recoil multiplier applied to the first shot of the heat
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="x"))
	float FirstShotRecoilMultiplier = 2.3f;

	// Max number of bullets the magazine can hold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammunition", meta=(ClampMin=1, UIMin=1))
	int32 MagazineSize = 30;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammunition", meta=(ClampMin=1, UIMin=1))
	int32 BulletsPerCartridge = 1;

	// Flag indicating if the gun can hold an additional round in the chamber when at max capacity
	// (e.g.: an M16 can hold 30+1 bullets, while a .44 Magnum can hold at most 6)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammunition", meta=(DisplayName="Can Hold 1 in the Chamber?"))
	bool bCanHold1InTheChamber = true;

#if WITH_EDITOR
public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
