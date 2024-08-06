#pragma once

#include "Equipment/EquipmentItemInstance.h"
#include "Equipment/EquipmentFragment_RangedWeaponStats.h"
#include "NativeGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "RangedWeaponItemInstance.generated.h"


INVENTORYANDEQUIPMENTRUNTIME_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Weapon_Message_StatsChanged);


// --------------------------------------------------------


USTRUCT(BlueprintType)
struct INVENTORYANDEQUIPMENTRUNTIME_API FWeaponStatsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	TObjectPtr<UEquipmentItemInstance> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float TimeLastFired;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float TimeLastEquipped;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float SpreadAngle;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	float SpreadAngleMultiplier;

	UPROPERTY(BlueprintReadOnly, Category=Weapon)
	bool bHas1InTheChamber;
};


// --------------------------------------------------------

UCLASS(Abstract)
class INVENTORYANDEQUIPMENTRUNTIME_API URangedWeaponItemInstance : public UEquipmentItemInstance
{
	GENERATED_BODY()

public:

	virtual void OnEquipped() override;
	virtual void OnUnequipped() override;

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

	/** Returns whether or not the weapon has a bullet in the chamber */
	UFUNCTION(BlueprintCallable, Category="Weapon|Spread")
	float GetSpreadAngle() const { return SpreadAngle; }

	/** Returns whether or not the weapon has a bullet in the chamber */
	UFUNCTION(BlueprintCallable, Category="Weapon|Spread")
	float GetSpreadAngleMultiplier() const { return SpreadAngleMultiplier; }

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
