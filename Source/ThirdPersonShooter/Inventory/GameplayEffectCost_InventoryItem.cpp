// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayEffectCost_InventoryItem.h"
#include "GameplayAbilities/Abilities/ModularGameplayAbility.h"
#include "Inventory/InventoryManagerComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayAbilitySpecHandle.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayEffectCost_InventoryItem)

UGameplayEffectCost_InventoryItem::UGameplayEffectCost_InventoryItem()
{
	Quantity.SetValue(1.0f);
}

bool UGameplayEffectCost_InventoryItem::CheckCost(const UModularGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (APawn* Pawn = Ability->GetPawnFromActorInfo())
	{
		if (UInventoryManagerComponent* InventoryComponent = Pawn->GetComponentByClass<UInventoryManagerComponent>())
		{
			const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

			const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
			const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

			return InventoryComponent->GetTotalItemCountByDefinition(ItemDefinition) >= NumItemsToConsume;
		}
	}
	return false;
}

void UGameplayEffectCost_InventoryItem::ApplyCost(const UModularGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	if (ActorInfo->IsNetAuthority())
	{
		if (APawn* Pawn = Ability->GetPawnFromActorInfo())
		{
			if (UInventoryManagerComponent* InventoryComponent = Pawn->GetComponentByClass<UInventoryManagerComponent>())
			{
				const int32 AbilityLevel = Ability->GetAbilityLevel(Handle, ActorInfo);

				const float NumItemsToConsumeReal = Quantity.GetValueAtLevel(AbilityLevel);
				const int32 NumItemsToConsume = FMath::TruncToInt(NumItemsToConsumeReal);

				InventoryComponent->ConsumeItemsByDefinition(ItemDefinition, NumItemsToConsume);
			}
		}
	}
}
