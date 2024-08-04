#pragma once

#include "Equipment/EquipmentItemInstance.h"
#include "Equipment/EquipmentFragment_RangedWeaponStats.h"

#include "RangedWeaponItemInstance.generated.h"


UCLASS(Abstract)
class INVENTORYANDEQUIPMENTRUNTIME_API URangedWeaponItemInstance : public UEquipmentItemInstance
{
	GENERATED_BODY()

public:
	URangedWeaponItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void Tick(float DeltaTime) override;

	virtual void OnEquipped() override;

	/** Returns the current spread angle (in degrees, diametrical) */
	float GetCalculatedSpreadAngle() const { return CurrentSpreadAngle; }

	float GetCalculatedSpreadAngleMultiplier() const { return bHasFirstShotAccuracy ? 0.0f : CurrentSpreadAngleMultiplier; }

public:

	/** Returns World time that this weapon was last fired (returns 0 if never fired) */
	UFUNCTION(BlueprintCallable, Category="Weapon|Timing")
	float GetTimeLastFired() const { return TimeLastFired; }

	/** Returns World time tath this weapon was last equipped (returns 0 if never equipped) */
	UFUNCTION(BlueprintCallable, Category="Weapon|Timing")
	float GetTimeLastEquipped() const { return TimeLastEquipped; }

	/** Returns whether or not the weapon has a bullet in the chamber */
	UFUNCTION(BlueprintCallable, Category="Weapon|Ammunition", meta=(DisplayName="Has 1 in the Chamber?"))
	bool Has1InTheChamber() const { return bHas1InTheChamber; }

	UFUNCTION(BlueprintCallable, Category=Weapon, meta=(DisplayName="Fire"))
	void OnFired();

	UFUNCTION(BlueprintImplementableEvent, Category=Weapon, meta=(DisplayName="OnFired"))
	void K2_OnFired();

	// Should the weapon have perfect accuracy when both player and weapon spread are at their minimum value
	UPROPERTY(EditAnywhere, Category="Weapon|Spread")
	bool bAllowFirstShotAccuracy = false;

private:
	// Updates the spread and returns true if the spread is at minimum
	bool UpdateSpread(float DeltaSeconds);

	// Updates the multipliers and returns true if they are at minimum
	bool UpdateMultipliers(float DeltaSeconds);

	const UEquipmentFragment_RangedWeaponStats* RangedWeaponStats;

	// World time that this weapon was last fired
	// This is used by in the spread calculation
	float TimeLastFired = 0.0f;

	// World time tath this weapon was last equipped
	// This is used by the auto reload GameplayAbility
	float TimeLastEquipped = 0.0f;

	// The current spread angle (in degrees, diametrical)
	float CurrentSpreadAngle = 0.0f;

	// The current *combined* spread angle multiplier
	float CurrentSpreadAngleMultiplier = 1.0f;

	// Do we currently have first shot accuracy?
	bool bHasFirstShotAccuracy = false;

	// Whether or not the weapon has a bullet in the chamber
	bool bHas1InTheChamber = false;

};
