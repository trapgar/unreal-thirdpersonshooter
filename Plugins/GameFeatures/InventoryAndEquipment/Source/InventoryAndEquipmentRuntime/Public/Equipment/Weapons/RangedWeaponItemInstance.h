#pragma once

#include "NativeGameplayTags.h"
#include "Curves/CurveFloat.h"
#include "Templates/SubclassOf.h"
#include "Physics/PhysicalMaterialWithTags.h"
#include "Equipment/EquipmentItemInstance.h"
#include "RangedWeaponProjectile.h"
#include "GameplayAbilities/ModularAbilityAttenuatorInterface.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "RangedWeaponItemInstance.generated.h"


INVENTORYANDEQUIPMENTRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Message_StatsChanged);
INVENTORYANDEQUIPMENTRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Equipment_Weapon_MagazineSize);


// --------------------------------------------------------


USTRUCT(BlueprintType)
struct INVENTORYANDEQUIPMENTRUNTIME_API FWeaponStatsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	TObjectPtr<UEquipmentItemInstance> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float TimeLastFired = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float TimeLastEquipped = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float SpreadAngle = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float SpreadAngleMultiplier = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	bool bHas1InTheChamber = false;
};


// --------------------------------------------------------

UCLASS(Abstract)
class INVENTORYANDEQUIPMENTRUNTIME_API URangedWeaponItemInstance : public UEquipmentItemInstance, public IModularAbilityAttenuatorInterface
{
	GENERATED_BODY()

public:

	// Initial velocity of the projectile on spawn
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

	// Angle in degrees to increase the spread by on every shot
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float SpreadAngleAccumulationPerShot = 0.098f;

	// Angle in degrees to decrease the spread by per second
	// @note this does not get applied until the weapon has not been fired for 60s / `RoundsPerMinute` * 1.2s
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Spread", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float SpreadAngleAccumulationDecayPerSecond = 11.1f;

	// Max recoil pitch angle in degrees
	// @note this will be applied as `RecoilPitch * FMath::RandRange(0.9f, 1.0f)`
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float RecoilPitch = 0.32f;

	// Min recoil yaw angle in degrees
	// @note horizontal recoil is applied as `FMath::RandRange(RecoilYawMin, RecoilYawMax)`
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMax=0.0f, UIMax=0.0f, ForceUnits="deg"))
	float RecoilYawMin = -0.12f;

	// Max recoil yaw angle in degrees
	// @note horizontal recoil is applied as `FMath::RandRange(RecoilYawMin, RecoilYawMax)`
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Handling|Recoil", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="deg"))
	float RecoilYawMax = 0.28f;

	// Additional recoil multiplier applied to the first shot of the heat
	// @experimental
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

public:

	/** Returns World time that this weapon was last fired (returns 0 if never fired) */
	UFUNCTION(BlueprintCallable, Category="Weapon|Timing")
	float GetTimeLastFired() const { return TimeLastFired; }

	/** Returns World time tath this weapon was last equipped (returns 0 if never equipped) */
	UFUNCTION(BlueprintCallable, Category="Weapon|Timing")
	float GetTimeLastEquipped() const { return TimeLastEquipped; }

	/** Returns whether or not the weapon has a bullet in the chamber */
	UFUNCTION(BlueprintCallable, Category="Weapon|Ammunition", meta=(DisplayName="Has 1 in the Chamber?"))
	bool GetHas1InTheChamber() const { return bHas1InTheChamber; }

	/** Returns whether or not the weapon has a bullet in the chamber */
	UFUNCTION(BlueprintCallable, Category="Weapon|Spread")
	float GetSpreadAngle() const { return SpreadAngle; }

	/** Returns whether or not the weapon has a bullet in the chamber */
	UFUNCTION(BlueprintCallable, Category="Weapon|Spread")
	float GetSpreadAngleMultiplier() const { return SpreadAngleMultiplier; }

	virtual float GetDistanceAttenuation(float Distance, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr ) const override
	{
		const FRichCurve* Curve = DistanceDamageFalloff.GetRichCurveConst();
		return Curve->HasAnyData() ? Curve->Eval(Distance) : 1.0f;
	};
	virtual float GetPhysicalMaterialAttenuation(const UPhysicalMaterial* PhysicalMaterial, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr ) const override
	{
		float CombinedMultiplier = 1.0f;
		if (const UPhysicalMaterialWithTags* PhysMatWithTags = Cast<const UPhysicalMaterialWithTags>(PhysicalMaterial))
		{
			for (const FGameplayTag MaterialTag : PhysMatWithTags->Tags)
			{
				if (const float* pTagMultiplier = DamageZoneMultipliers.Find(MaterialTag))
				{
					CombinedMultiplier *= *pTagMultiplier;
				}
			}
		}

		return CombinedMultiplier;
	};

public:

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

	void SetAssociatedItem(UInventoryItemInstance* InSource)
	{
		Super::SetAssociatedItem(InSource);

		if (InSource)
		{
			InSource->AddStatTagStack(TAG_Equipment_Weapon_MagazineSize, MagazineSize);
		}
	};

private:
	float TimeLastFired = 0.0f;
	float TimeLastEquipped = 0.0f;
	float SpreadAngle = 0.0f;
	float SpreadAngleMultiplier = 1.0f;
	bool bHas1InTheChamber = false;

private:
	FGameplayMessageListenerHandle Handle_WeaponStatsChanged;

	void OnWeaponStatsChanged(const FGameplayTag Channel, const FWeaponStatsChangedMessage& Payload);
};
