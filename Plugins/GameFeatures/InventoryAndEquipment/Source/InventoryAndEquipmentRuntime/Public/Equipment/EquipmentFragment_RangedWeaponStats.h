#pragma once

#include "Curves/CurveFloat.h"
#include "EquipmentItemDefinition.h"
#include "Templates/SubclassOf.h"
#include "Weapons/RangedWeaponProjectile.h"

#include "EquipmentFragment_RangedWeaponStats.generated.h"

class UEquipmentItemDefinition;
class UPhysicalMaterial;
class UObject;


UCLASS(BlueprintType)
class UEquipmentFragment_RangedWeaponStats : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:

	// Initial velocity of the bullet on projectile spawn
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits="cm/s", ClampMin="0.0", UIMin="0.0"))
	float MuzzleVelocity = 25000.0f;

	// A curve that maps the distance (in cm) to a multiplier on the base damage from the associated gameplay effect
	// If there is no data in this curve, then the weapon is assumed to have no falloff with distance
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ballistics", meta=(ForceUnits=cm))
	FRuntimeFloatCurve DistanceDamageFalloff;

	// The radius for bullet traces sweep spheres (0.0 will result in a line trace)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits=cm, ClampMin="0.0", UIMin="0.0"))
	float BulletTraceSweepRadius = 1.0f;

	// The maximum distance at which this weapon can deal damage
	// TODO: May not need this prop as it is already in DistanceDamageFalloff?
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits=cm))
	float MaxDamageRange = 25000.0f;

	// Bullet class definition to spawn on weapon fire
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ballistics")
	TSubclassOf<ARangedWeaponProjectile> ProjectileToSpawn;

	// Applied damage type
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects")
	TSubclassOf<UGameplayEffect> DamageType;

	// Base amount of damage to apply to a on projectile hit
	// This is pre-multipliers (e.g. damage falloff, headshot, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects", meta=(ClampMin="0.0", UIMin="0.0"))
	float SingleBulletDamage;

	// List of special tags that affect how damage is dealt
	// These tags will be compared to tags in the physical material of the thing being hit
	// If more than one tag is present, the multipliers will be combined multiplicatively
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Effects", meta=(Categories="Gameplay.Zone", ForceUnits=x))
	TMap<FGameplayTag, float> DamageZoneMultipliers;

	// Max rate of fire in rounds per minute
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handling", meta=(ForceUnits="rpm", ClampMin=1.0f, UIMin=1.0f))
	float RoundsPerMinute = 1.0f;

	// List of available fire modes the player can cycle through (e.g. single, semi-auto, full-auto, etc.)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handling", meta=(Categories="Equipment.Weapon.Mode"))
	FGameplayTagContainer FireModes;

	// 2d linear curve of the bullet spread
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handling")
	FInterpCurveVector2D BulletSpread;

	// 2d linear curve of the muzzle recoil
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Handling")
	FInterpCurveVector2D Recoil;

	// Max number of bullets the magazine can hold
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammunition", meta=(ClampMin="1", UIMin="1"))
	int32 MagazineSize = 30;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammunition", meta=(ClampMin="1", UIMin="1"))
	int32 BulletsPerCartridge = 1;

	// Flag indicating if the gun can hold an additional round in the chamber at when max capacity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammunition", meta=(DisplayName="Can Hold 1 in the Chamber?"))
	bool bCanHold1InTheChamber = true;

};
