// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemComponent.h"
#include "GameplayAbilities/Attributes/PawnAttributeSet.h"

#include "PawnCombatSet.generated.h"

class UObject;
struct FFrame;


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

	ATTRIBUTE_ACCESSORS(UPawnCombatSet, Damage);
	ATTRIBUTE_ACCESSORS(UPawnCombatSet, Heal);

protected:

	UFUNCTION()
	void OnRep_Damage(const FGameplayAttributeData& OldValue);

	UFUNCTION()
	void OnRep_Heal(const FGameplayAttributeData& OldValue);

private:

	// The base amount of damage to apply in the damage execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Damage, Category = "Pawn|Ability|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;

	// The base amount of healing to apply in the heal execution.
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Heal, Category = "Pawn|Ability|Combat", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Heal;
};
