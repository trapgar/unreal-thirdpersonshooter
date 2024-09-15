#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility_FromRangedWeapon.h"
#include "Equipment/Weapons/RangedWeaponItemInstance.h"

#include "GameplayAbility_RangedWeapon_Fire.generated.h"

/**
 * UAbility_RangedWeapon_Fire
 * 
 * Gameplay ability that handles ranged weapon firing with spread, aiming, etc.
 */
UCLASS()
class THIRDPERSONSHOOTER_API UGameplayAbility_RangedWeapon_Fire : public UGameplayAbility_FromRangedWeapon
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

private:
	// Returns the spread angle multiplier that should be applied to the base spread angle
	float GetSpreadAngleMultiplier() const;

	// Tells the associated weapon to what the current spread angle & multiplier is
	void OnHandleUpdateWeaponStatsChanged();

	// Decays the spread accumulated by automatic fire
	void OnHandleSpreadDecay();

	FTimerHandle TimerHandleSpread;
	FTimerHandle TimerHandleSpreadDecay;

	// World time that this weapon was last fired
	// This is used by in the spread calculation
	float TimeLastFired = 0.0f;

	// World time that this weapon was last equipped
	// This is used by the auto reload GameplayAbility
	float TimeLastEquipped = 0.0f;

	// World seconds that the decay tick was last called
	float TimeSinceLastDecayed = 0.0f;

	// The current spread angle accumulated by automatic fire
	float AccumulatedSpreadAngle = 0.0f;

	// Do we currently have first shot accuracy?
	// TODO: Not used - pull from RangedWeaponStats if/when actually implemented
	bool bHasFirstShotAccuracy = false;

	// Whether or not the weapon has a bullet in the chamber
	bool bHas1InTheChamber = false;
	
	// Weapon root scene component, used to get the muzzle transform on fire
	USceneComponent* WeaponComponent;
};
