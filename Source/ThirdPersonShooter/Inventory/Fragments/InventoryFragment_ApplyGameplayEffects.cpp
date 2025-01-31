// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryFragment_ApplyGameplayEffects.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

#include "Inventory/InventoryItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_ApplyGameplayEffects)

void UInventoryFragment_ApplyGameplayEffects::OnInstanceCreated(UInventoryItemInstance* Instance) const
{
	// TODO: Pull effect creation from the InventoryItemDefinition_Ammunition.h globals class
	if (AActor* Instigator = Instance->GetInstigator())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator))
		{
			for (const TSubclassOf<UGameplayEffect> GE_Subclass : GameplayEffectsToApply)
			{
				FGameplayEffectSpecHandle GE_Spec_Handle = ASC->MakeOutgoingSpec(GE_Subclass, /*Level=*/0.0f, ASC->MakeEffectContext());
				// GE_Spec_Handle.Data.Get()->SetSetByCallerMagnitude(ThirdPersonShooterGameplayTags::SetByCaller_Damage, Weapon->SingleBulletDamage);

				ASC->ApplyGameplayEffectSpecToTarget(*GE_Spec_Handle.Data.Get(), ASC);
			}
		}
	}
}
