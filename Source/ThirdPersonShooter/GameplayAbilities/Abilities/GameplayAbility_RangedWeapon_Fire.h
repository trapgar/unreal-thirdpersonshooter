#pragma once

#include "CoreMinimal.h"
#include "GameplayAbility_FromRangedWeapon.h"
#include "Equipment/Weapons/RangedWeaponItemInstance.h"

#include "GameplayAbility_RangedWeapon_Fire.generated.h"

THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Message_Weapon_Fired);

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
	
	// Weapon root scene component, used to get the muzzle transform on fire
	USceneComponent* WeaponComponent;
};
