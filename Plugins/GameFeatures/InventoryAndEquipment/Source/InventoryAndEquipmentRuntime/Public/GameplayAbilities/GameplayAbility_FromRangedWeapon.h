#pragma once

#include "GameplayAbility_FromEquipment.h"
#include "Equipment/Weapons/RangedWeaponItemInstance.h"

#include "GameplayAbility_FromRangedWeapon.generated.h"

class URangedWeaponItemInstance;

/**
 * UGameplayAbility_FromRangedWeapon
 *
 * An ability granted by and associated with an equipment instance
 */
UCLASS()
class INVENTORYANDEQUIPMENTRUNTIME_API UGameplayAbility_FromRangedWeapon : public UGameplayAbility_FromEquipment
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category="Ability|Weapon")
	URangedWeaponItemInstance* GetAssociatedWeapon() const { return Cast<URangedWeaponItemInstance>(GetAssociatedEquipment()); }
};
