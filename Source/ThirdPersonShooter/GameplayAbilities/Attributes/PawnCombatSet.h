// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Attributes/PawnAttributeSet.h"
#include "NativeGameplayTags.h"

#include "PawnCombatSet.generated.h"

class UObject;
struct FFrame;

THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Classification_Primary_Rifle);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Classification_Primary_Shotgun);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Classification_Primary_Marksman);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Classification_Primary_Heavy);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Item_Classification_Primary_Sidearm);

/**
 * UPawnCombatSet
 *
 *  Class that defines attributes that are necessary for applying damage or healing.
 *	Attribute examples include: damage, healing, attack power, and shield penetrations.
 */
UCLASS(BlueprintType)
class THIRDPERSONSHOOTER_API UPawnCombatSet : public UPawnAttributeSet
{
	GENERATED_BODY()

public:

	UPawnCombatSet();

	ATTRIBUTE_ACCESSORS(UPawnCombatSet, BaseDamageApplied);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, BaseHealingApplied);

	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmunitionRefill);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmunitionSpend);

	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmoStackCount);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmoStackCount_Rifle);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, MaxAmmoStackCount_Rifle);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmoStackCount_Shotgun);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, MaxAmmoStackCount_Shotgun);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmoStackCount_Marksman);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, MaxAmmoStackCount_Marksman);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmoStackCount_Heavy);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, MaxAmmoStackCount_Heavy);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, AmmoStackCount_Sidearm);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, MaxAmmoStackCount_Sidearm);

	ATTRIBUTE_ACCESSORS(UPawnCombatSet, SpreadAngleMultiplier_HipFire);

protected:

	UFUNCTION()
	void OnRep_BaseDamageApplied(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_BaseHealingApplied(const FGameplayAttributeData& OldValue);

	// ---

	UFUNCTION()
	void OnRep_AmmunitionRefill(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AmmunitionSpend(const FGameplayAttributeData& OldValue);

	// ---

	UFUNCTION()
	void OnRep_AmmoStackCount(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AmmoStackCount_Rifle(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAmmoStackCount_Rifle(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AmmoStackCount_Shotgun(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAmmoStackCount_Shotgun(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AmmoStackCount_Marksman(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAmmoStackCount_Marksman(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AmmoStackCount_Heavy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAmmoStackCount_Heavy(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_AmmoStackCount_Sidearm(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_MaxAmmoStackCount_Sidearm(const FGameplayAttributeData& OldValue);

	// ---

	UFUNCTION()
	void OnRep_SpreadAngleMultiplier_HipFire(const FGameplayAttributeData& OldValue);

	// ---

	virtual bool PreGameplayEffectExecute(FGameplayEffectModCallbackData& Data) override;
	virtual void PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data) override;

	virtual void PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const override;
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// Clamp attribute values
	void ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const;

private:

	// Sans equipment, etc - this is the amount of damage to apply in the damage execution.
	// Gameplay effects can modify this with the 'Executions > Calculation Modifiers > Modifier Op'
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseDamageApplied, Category = "Pawn|Ability|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseDamageApplied;

	// Sans equipment, etc - this is the amount of healing to apply in the heal execution.
	// Gameplay effects can modify this with the 'Executions > Calculation Modifiers > Modifier Op'
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_BaseHealingApplied, Category = "Pawn|Ability|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData BaseHealingApplied;

	// ---

	// Incoming ammo stack count to refill
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmunitionRefill, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmunitionRefill;

	// Incoming ammo stack count to spend
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmunitionSpend, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmunitionSpend;

	// ---

	// @TODO: is this even used?
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmoStackCount, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoStackCount;

	// Current amount of rifle ammo
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmoStackCount_Rifle, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoStackCount_Rifle;

	// Maximum amount of rifle ammo that the player can hold
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmoStackCount_Rifle, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoStackCount_Rifle;

	// Current amount of shotgun ammo
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmoStackCount_Shotgun, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoStackCount_Shotgun;

	// Maximum amount of shotgun ammo that the player can hold
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmoStackCount_Shotgun, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoStackCount_Shotgun;

	// Current amount of marksman ammo
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmoStackCount_Marksman, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoStackCount_Marksman;

	// Maximum amount of marksman ammo that the player can hold
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmoStackCount_Marksman, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoStackCount_Marksman;

	// Current amount of heavy ammo
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmoStackCount_Heavy, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoStackCount_Heavy;

	// Maximum amount of heavy ammo that the player can hold
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmoStackCount_Heavy, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoStackCount_Heavy;

	// Current amount of sidearm ammo
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AmmoStackCount_Sidearm, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData AmmoStackCount_Sidearm;

	// Maximum amount of sidearm ammo that the player can hold
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxAmmoStackCount_Sidearm, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData MaxAmmoStackCount_Sidearm;

	// ---

	// Additional multiplier applied to the spread angle when hip firing - 1.0 is no change
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_SpreadAngleMultiplier_HipFire, Category = "Pawn|Ability|Combat|Ranged Weapon", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData SpreadAngleMultiplier_HipFire;

public:

	static FGameplayAttribute GetAttributeFromContainer(const FGameplayTagContainer Container)
	{
		if (Container.HasTagExact(TAG_Item_Classification_Primary_Rifle))
		{
			return UPawnCombatSet::GetAmmoStackCount_RifleAttribute();
		}
		else if (Container.HasTagExact(TAG_Item_Classification_Primary_Shotgun))
		{
			return UPawnCombatSet::GetAmmoStackCount_ShotgunAttribute();
		}
		else if (Container.HasTagExact(TAG_Item_Classification_Primary_Marksman))
		{
			return UPawnCombatSet::GetAmmoStackCount_MarksmanAttribute();
		}
		else if (Container.HasTagExact(TAG_Item_Classification_Primary_Heavy))
		{
			return UPawnCombatSet::GetAmmoStackCount_HeavyAttribute();
		}
		else if (Container.HasTagExact(TAG_Item_Classification_Primary_Sidearm))
		{
			return UPawnCombatSet::GetAmmoStackCount_SidearmAttribute();
		}

		return UPawnCombatSet::GetAmmoStackCountAttribute();
	};
};
