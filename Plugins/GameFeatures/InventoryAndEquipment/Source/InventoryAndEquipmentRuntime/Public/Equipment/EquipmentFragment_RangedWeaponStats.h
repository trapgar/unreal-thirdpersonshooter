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

UCLASS(BlueprintType)
class UEquipmentFragment_RangedWeaponStats : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UEquipmentItemInstance* Instance) const override;

public:

	// Initial velocity of the bullet on projectile spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="cm/s"))
	float MuzzleVelocity = 25000.0f;

	// A curve that maps the distance (in cm) to a multiplier on the base damage from the associated gameplay effect
	// If there is no data in this curve, then the weapon is assumed to have no falloff with distance
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits="cm"))
	FRuntimeFloatCurve DistanceDamageFalloff;

	// The radius for bullet traces sweep spheres (0.0 will result in a line trace)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="cm"))
	float BulletTraceSweepRadius = 1.0f;

	// The maximum distance at which this weapon can deal damage
	// TODO: May not need this prop as it is already in DistanceDamageFalloff?
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(ForceUnits="cm"))
	float MaxDamageRange = 25000.0f;

	// Bullet class definition to spawn on weapon fire
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics")
	TSubclassOf<ARangedWeaponProjectile> ProjectileToSpawn;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ballistics", meta=(DisplayName="Has Perfect First Shot Accuracy?"))
	bool bHasFirstShotAccuracy = false;

	// Applied damage type
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
	float RoundsPerMinute = 600.0f;

	// List of available fire modes the player can cycle through (e.g. single, semi-auto, full-auto, etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling", meta=(Categories="Equipment.Weapon.Mode"))
	FGameplayTagContainer FireModes;

	// Spread angle in degrees while aiming down sight (base spread angle)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float SpreadAngleAimDownSight = 0.2f;

	// Spread angle penalty multiplier while moving
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="x"))
	float SpreadAngleMultiplierWhileMoving = 5.0f;

	// Spread angle penalty multiplier while hip firing
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="x"))
	float SpreadAngleMultiplierHipFire = 15.0f;

	// Max angle of the spread in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float SpreadAngleMax = 3.5f;

	// Max recoil pitch angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float RecoilPitch = 0.32f;

	// Min recoil yaw angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMax=0.0f, UIMax=0.0f, ForceUnits="deg"))
	float RecoilYawMin = -0.12f;

	// Max recoil yaw angle in degrees
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float RecoilYawMax = 0.28f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="x"))
	float FirstShotRecoilMultiplier = 2.3f;

	// Max number of bullets the magazine can hold
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammunition", meta=(ClampMin=1, UIMin=1))
	int32 MagazineSize = 30;

	// Number of bullets to fire in a single cartridge (typically 1, but may be more for shotguns)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammunition", meta=(ClampMin=1, UIMin=1))
	int32 BulletsPerCartridge = 1;

	// Flag indicating if the gun can hold an additional round in the chamber at when max capacity
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ammunition", meta=(DisplayName="Can Hold 1 in the Chamber?"))
	bool bCanHold1InTheChamber = true;

#if WITH_EDITOR
public:
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
};
