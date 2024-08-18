// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayAbilities/Attributes/PawnCombatSet.h"

#include "GameplayAbilities/Attributes/PawnAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PawnCombatSet)

class FLifetimeProperty;


UPawnCombatSet::UPawnCombatSet()
	: Damage(0.0f)
	, Heal(0.0f)
{
}

void UPawnCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, Damage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, Heal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UPawnCombatSet::OnRep_Damage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, Damage, OldValue);
}

void UPawnCombatSet::OnRep_Heal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, Heal, OldValue);
}

