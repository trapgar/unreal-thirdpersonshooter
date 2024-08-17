// Copyright Epic Games, Inc. All Rights Reserved.

#include "PawnCombatSet.h"

#include "GameplayAbilities/Attributes/PawnAttributeSet.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PawnCombatSet)

class FLifetimeProperty;


UPawnCombatSet::UPawnCombatSet()
	: BaseDamage(0.0f)
	, BaseHeal(0.0f)
{
}

void UPawnCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, BaseDamage, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, BaseHeal, COND_OwnerOnly, REPNOTIFY_Always);
}

void UPawnCombatSet::OnRep_BaseDamage(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, BaseDamage, OldValue);
}

void UPawnCombatSet::OnRep_BaseHeal(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, BaseHeal, OldValue);
}

