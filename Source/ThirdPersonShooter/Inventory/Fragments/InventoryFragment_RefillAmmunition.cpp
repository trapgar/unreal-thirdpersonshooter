// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryFragment_RefillAmmunition.h"

#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryItemDefinition_Ammunition.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameplayAbilities/ModularAbilityAttenuatorInterface.h"
#include "GameplayAbilities/ModularGameplayEffectContext.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_RefillAmmunition)

FGameplayEffectSpecHandle UInventoryFragment_RefillAmmunition::MakeOutgoingAmmunitionSpec(UInventoryItemInstance* ItemInstance) const
{
	if (UInventoryItemDefinition_Ammunition* ItemDef = Cast<UInventoryItemDefinition_Ammunition>(ItemInstance->GetItemDef()))
	{
		if (AActor* Instigator = ItemInstance->GetInstigator())
		{
			if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator))
			{
				FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
				FModularGameplayEffectContext* EffectContext = FModularGameplayEffectContext::ExtractEffectContext(ContextHandle);
				EffectContext->AddInstigator(Instigator, Instigator);
				EffectContext->AddSourceObject(ItemInstance);

				FGameplayEffectSpecHandle GE_Spec_Handle = ASC->MakeOutgoingSpec(ItemDef->AmmunitionRefillOrSpendEffect, /*Level=*/0.0f, ContextHandle);

				if (FGameplayEffectSpec* Spec = GE_Spec_Handle.Data.Get())
				{
					Spec->AddDynamicAssetTag(ItemDef->WeaponClassification);
				}

				GE_Spec_Handle.Data.Get()->SetSetByCallerMagnitude(ThirdPersonShooterGameplayTags::SetByCaller_StackCount, ItemDef->StackCount);

				return GE_Spec_Handle;
			}
		}
	}

	return FGameplayEffectSpecHandle();
}

void UInventoryFragment_RefillAmmunition::OnInstanceCreated(UInventoryItemInstance* Instance) const
{
	if (AActor* Instigator = Instance->GetInstigator())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator))
		{
			FGameplayEffectSpecHandle GE_Spec_Handle = MakeOutgoingAmmunitionSpec(Instance);
			ASC->ApplyGameplayEffectSpecToTarget(*GE_Spec_Handle.Data.Get(), ASC);
		}
	}
}
