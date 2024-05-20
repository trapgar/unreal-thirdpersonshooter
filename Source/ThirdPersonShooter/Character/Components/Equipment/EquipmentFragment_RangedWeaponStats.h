#pragma once

#include "Curves/CurveFloat.h"
#include "EquipmentItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "EquipmentFragment_RangedWeaponStats.generated.h"

class UEquipmentItemDefinition;
class UPhysicalMaterial;
class UObject;


UCLASS(BlueprintType)
class UEquipmentFragment_RangedWeaponStats : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits="cm/s"))
	float MuzzleVelocity = 250000.0f;

	// A curve that maps the distance (in cm) to a multiplier on the base damage from the associated gameplay effect
	// If there is no data in this curve, then the weapon is assumed to have no falloff with distance
	UPROPERTY(EditAnywhere, Category = "Ballistics")
	FRuntimeFloatCurve DistanceDamageFalloff;

	// The radius for bullet traces sweep spheres (0.0 will result in a line trace)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits=cm))
	float BulletTraceSweepRadius = 0.0f;

	// The maximum distance at which this weapon can deal damage
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits=cm))
	float MaxDamageRange = 25000.0f;

	// Bullet class definition to spawn on weapon fire
	UPROPERTY(EditAnywhere, Category = "Ballistics")
	TSubclassOf<AActor> ProjectileToSpawn;

	// Points of damage to apply to a on projectile hit
	UPROPERTY(EditAnywhere, Category = "Effects")
	float SingleBulletDamage;

	// Points of damage to apply to a on projectile hit
	UPROPERTY(EditAnywhere, Category = "Effects", meta=(Categories="Gameplay.Zone"))
	TMap<FGameplayTag, float> DamageZoneMultipliers;

	// Max rate of fire in rounds per minute
	UPROPERTY(EditAnywhere, Category = "Handling", meta=(ForceUnits="rounds/min"))
	float RoundsPerMinute = 0.0f;

	// List of available fire modes the player can cycle through (e.g. semi-auto, full-auto, etc.)
	UPROPERTY(EditAnywhere, Category = "Handling", meta=(Categories="Equipment.Weapon.Mode"))
	FGameplayTagContainer FireModes;

	// 2d linear curve of the bullet spread
	UPROPERTY(EditAnywhere, Category = "Handling")
	FInterpCurveVector2D BulletSpread;

	// 2d linear curve of the muzzle recoil
	UPROPERTY(EditAnywhere, Category = "Handling")
	FInterpCurveVector2D Recoil;

	// Max number of bullets the magazine can hold
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammunition")
	int32 MagazineSize = 30;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammunition")
	int32 BulletsPerCartridge = 1;

	// Flag indicating if the gun can hold an additional round in the chamber at when max capacity
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Ammunition", meta=(DisplayName="Can Hold 1 in the Chamber?"))
	bool bCanHold1InTheChamber = true;

};
