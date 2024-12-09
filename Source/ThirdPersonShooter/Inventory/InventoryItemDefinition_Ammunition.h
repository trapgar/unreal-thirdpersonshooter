// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InventoryItemDefinition.h"
#include "InventoryManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "NativeGameplayTags.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"
#include "Kismet/GameplayStatics.h"
#include "ThirdPersonShooterGameplayTags.h"

#include "InventoryItemDefinition_Ammunition.generated.h"

THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Inventory_Item_Classification);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Inventory_Item_Classification_Ammunition);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Inventory_Weapon_Ammunition);

/**
 * Definition type of an inventory item
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Const, Abstract)
class UInventoryItemDefinition_Ammunition : public UInventoryItemDefinition
{
	GENERATED_BODY()

public:

	UInventoryItemDefinition_Ammunition(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		StaticGameplayTags.AddTag(TAG_Inventory_Item_Classification_Ammunition);
	}

	// Gameplay tag of the weapon this ammo is for
	UPROPERTY(EditDefaultsOnly, Category = Inventory, meta = (Categories = "Item.Classification"))
	FGameplayTag WeaponClassification;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	TSubclassOf<UGameplayEffect> AmmunitionRefillOrSpendEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	int32 StackCount = 1;
};


// --------------------------------------------------------


// @TODO: Make into a subsystem instead?
UCLASS()
class UWeaponAmmunitionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// @TODO: Don't like that this is off the InventoryItemInstance class, but the ID_Ammunition instance doesn't have a reference to the Instigator
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Inventory)
	static const FGameplayEffectSpecHandle MakeOutgoingAmmunitionSpec(UInventoryItemInstance* ItemInstance)
	{
		if (ItemInstance != nullptr)
		{
			if (UInventoryItemDefinition_Ammunition* ItemDef = Cast<UInventoryItemDefinition_Ammunition>(ItemInstance->GetItemDef()))
			{
				if (const AActor* Instigator = ItemInstance->GetInstigator())
				{
					if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator))
					{
						FGameplayEffectContextHandle GE_RefillOrSpend = ASC->MakeEffectContext();
						FGameplayEffectSpecHandle GE_Spec_Handle = ASC->MakeOutgoingSpec(ItemDef->AmmunitionRefillOrSpendEffect, /*Level=*/0.0f, GE_RefillOrSpend);

						if (FGameplayEffectSpec* Spec = GE_Spec_Handle.Data.Get())
						{
							Spec->AddDynamicAssetTag(ItemDef->WeaponClassification);
						}

						GE_Spec_Handle.Data.Get()->SetSetByCallerMagnitude(ThirdPersonShooterGameplayTags::SetByCaller_StackCount, ItemDef->StackCount);

						return GE_Spec_Handle;
						// ASC->ApplyGameplayEffectSpecToTarget(*GE_Spec_Handle.Data.Get(), ASC);
					}
				}
			}
		}

		return FGameplayEffectSpecHandle();
	};

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Ammunition")
	static const int32 GetAmmunitionStackCount(UInventoryItemInstance* ItemInstance)
	{
		if (const AActor* Instigator = ItemInstance->GetInstigator())
		{
			if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator))
			{
				return ASC->GetNumericAttribute(UPawnCombatSet::GetAttributeFromContainer(ItemInstance->GetItemDef()->StaticGameplayTags));
			}
		}

		return 0;
	}

	// @TODO: might be able to re-work this for general use instead of just ammunition
	UFUNCTION(BlueprintCallable, Category = "Inventory|Ammunition")
	static void ConsolidateAmmoStacks(UInventoryManagerComponent* InventoryManager)
	{
		TArray<UInventoryItemInstance*> AmmoItems;

		// Get all ammo items in the inventory
		for (const FReadOnlyInventoryEntry Entry : InventoryManager->GetAllItems())
		{
			if (const UInventoryItemDefinition_Ammunition* ID_Ammunition = Cast<UInventoryItemDefinition_Ammunition>(Entry.Instance->GetItemDef()))
			{
				AmmoItems.Add(Entry.Instance);
			}
		}

		// Consolidate ammo items with the same type def
		for (int32 i = 0; i < AmmoItems.Num(); i++)
		{
			UInventoryItemInstance* CurrentItem = AmmoItems[i];
			int32 MaxStackCount = CurrentItem->GetItemDef()->MaxStackCount;
			int32 AmountInMyStack = CurrentItem->GetStatTagStackCount(TAG_Inventory_Weapon_Ammunition);

			if (AmountInMyStack < MaxStackCount)
			{
				for (int32 j = i + 1; j < AmmoItems.Num(); j++)
				{
					UInventoryItemInstance* OtherItem = AmmoItems[j];

					if (OtherItem->GetItemDef()->GetClass() == CurrentItem->GetItemDef()->GetClass())
					{
						int32 AmountAvailableToAdd = OtherItem->GetStatTagStackCount(TAG_Inventory_Weapon_Ammunition);

						if (AmountAvailableToAdd > 0)
						{
							int32 AmountToAdd = FMath::Min(MaxStackCount - AmountInMyStack, AmountAvailableToAdd);
							CurrentItem->AddStatTagStack(TAG_Inventory_Weapon_Ammunition, AmountToAdd);
							OtherItem->RemoveStatTagStack(TAG_Inventory_Weapon_Ammunition, AmountToAdd);
						}

						// Remove empty ammo stacks
						if (OtherItem->GetStatTagStackCount(TAG_Inventory_Weapon_Ammunition) <= 0)
						{
							InventoryManager->RemoveItem(OtherItem);
						}
					}
				}
			}
		}
	}
};
