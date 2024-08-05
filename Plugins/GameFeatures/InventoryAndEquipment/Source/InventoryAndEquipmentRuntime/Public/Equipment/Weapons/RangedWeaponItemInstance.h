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

public:

	/** Returns World time that this weapon was last fired (returns 0 if never fired) */
	UFUNCTION(BlueprintCallable, Category="Weapon|Timing")
	float GetTimeLastFired() const { return TimeLastFired; }
	void SetTimeLastFired(float NewValue) { TimeLastFired = NewValue; }

	/** Returns World time tath this weapon was last equipped (returns 0 if never equipped) */
	UFUNCTION(BlueprintCallable, Category="Weapon|Timing")
	float GetTimeLastEquipped() const { return TimeLastEquipped; }
	void SetTimeLastEquipped(float NewValue) { TimeLastEquipped = NewValue; }

	/** Returns whether or not the weapon has a bullet in the chamber */
	UFUNCTION(BlueprintCallable, Category="Weapon|Ammunition", meta=(DisplayName="Has 1 in the Chamber?"))
	bool Has1InTheChamber() const { return bHas1InTheChamber; }
	void SetHas1InTheChamber(bool bNewValue) { bHas1InTheChamber = bNewValue; };

	float GetSpreadAngle() const { return SpreadAngle; };
	void SetSpreadAngle(float NewAngle) { SpreadAngle = NewAngle; };

	float GetSpreadAngleMultiplier() const { return SpreadAngleMultiplier; };
	void SetSpreadAngleMultiplier(float NewMultiplier) { SpreadAngleMultiplier = NewMultiplier; };

private:
	float TimeLastFired = 0.0f;
	float TimeLastEquipped = 0.0f;
	float SpreadAngle = 0.0f;
	float SpreadAngleMultiplier = 1.0f;
	bool bHas1InTheChamber = false;

};
