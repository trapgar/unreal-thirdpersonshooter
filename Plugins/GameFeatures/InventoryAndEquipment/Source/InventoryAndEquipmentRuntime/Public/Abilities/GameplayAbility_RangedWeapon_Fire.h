#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility_FromEquipment.h"
#include "Equipment/Weapons/RangedWeaponItemInstance.h"
#include "Equipment/EquipmentFragment_RangedWeaponStats.h"

#include "GameplayAbility_RangedWeapon_Fire.generated.h"

/**
 * UAbility_RangedWeapon_Fire
 * 
 * Gameplay ability that handles ranged weapon firing with spread, aiming, etc.
 */
UCLASS()
class INVENTORYANDEQUIPMENTRUNTIME_API UGameplayAbility_RangedWeapon_Fire : public UGameplayAbility_FromEquipment
{
	GENERATED_BODY()

public:
	// Override OnGiveAbility to set some variables we'll use on fire
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	// Clean up timers etc on remove
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData
	) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled
	) override;

	// Returns a transform that is based on the current spread angle (pre-spread)
	UFUNCTION(BlueprintCallable, Category="Weapon|Accuracy")
	virtual FTransform GetBaseProjectileSpawnTransform(float Radius, float MaxTraceDistanceInCm = 10000.0f) const;

	// Returns a spread rotator that is based on the current spread angle
	UFUNCTION(BlueprintCallable, Category="Weapon|Accuracy")
	virtual FRotator GetProjectileSpreadRotator() const;

	URangedWeaponItemInstance* GetAssociatedWeapon() const { return Cast<URangedWeaponItemInstance>(GetAssociatedEquipment()); }

private:
	// Returns the spread angle multiplier that should be applied to the base spread angle
	float CalculateSpreadAngleMultiplier() const;

	// Tells the associated weapon to what the current spread angle & multiplier is
	void OnTickSpreadCheck();

	// Decays the spread accumulated by automatic fire
	void OnTickSpreadDecay();

	FTimerHandle TimerHandleSpread;
	FTimerHandle TimerHandleSpreadDecay;

	const UEquipmentFragment_RangedWeaponStats* RangedWeaponStats;

	// World time that this weapon was last fired
	// This is used by in the spread calculation
	float TimeLastFired = 0.0f;

	// World time tath this weapon was last equipped
	// This is used by the auto reload GameplayAbility
	float TimeLastEquipped = 0.0f;

	// The current spread angle multiplier accumulated by automatic fire
	float AccumulatedSpreadAngleMultiplier = 1.0f;

	// Do we currently have first shot accuracy?
	// TODO: Not used - pull from RangedWeaponStats if/when actually implemented
	bool bHasFirstShotAccuracy = false;

	// Whether or not the weapon has a bullet in the chamber
	bool bHas1InTheChamber = false;
	
	// Weapon root scene component, used to get the muzzle transform on fire
	USceneComponent* WeaponComponent;
};
