#pragma once

#include "GameplayAbility_FromEquipment.h"
#include "Equipment/Weapons/RangedWeaponItemInstance.h"
#include "CustomLogChannels.h"

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

	virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override
	{
		bool bResult = Super::CanActivateAbility(Handle, ActorInfo, SourceTags, TargetTags, OptionalRelevantTags);

		if (bResult)
		{
			if (GetAssociatedWeapon() == nullptr)
			{
				UE_LOG(LogAbilities, Error, TEXT("Weapon ability %s cannot be activated because there is no associated ranged weapon (equipment instance=%s but needs to be derived from %s)"),
					*GetPathName(),
					*GetPathNameSafe(GetAssociatedEquipment()),
					*URangedWeaponItemInstance::StaticClass()->GetName());
				bResult = false;
			}
		}

		return bResult;
	};
};
