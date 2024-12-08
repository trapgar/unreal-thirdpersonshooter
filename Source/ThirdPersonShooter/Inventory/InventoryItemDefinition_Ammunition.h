// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InventoryItemDefinition.h"
#include "InventoryManagerComponent.h"
#include "NativeGameplayTags.h"

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

	// Definition of the inventory item to find & set stats for
	UPROPERTY(EditDefaultsOnly, Category = Inventory, meta = (Categories = "Item.Classification"))
	FGameplayTag WeaponClassification;
};


// --------------------------------------------------------


// @TODO: Make into a subsystem instead?
UCLASS()
class UWeaponAmmunitionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Returns the first matching item fragment based on class
	// @TODO: this is called every tick by the AmmoCounter widget, can probably improve
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Ammunition")
	static const UInventoryItemInstance* FindAmmunitionItemInstance(UInventoryItemInstance* ItemInstance)
	{
		if (const AActor* Instigator = ItemInstance->GetInstigator())
		{
			if (const UInventoryManagerComponent* InventoryManager = Instigator->FindComponentByClass<UInventoryManagerComponent>())
			{
				FGameplayTagContainer ItemClassification;
				ItemClassification.AddTag(TAG_Inventory_Item_Classification);
				FGameplayTagContainer WeaponTags = ItemInstance->GetItemDef()->StaticGameplayTags.Filter(ItemClassification);

				for (const FReadOnlyInventoryEntry Entry : InventoryManager->GetAllItems())
				{
					if (const UInventoryItemDefinition_Ammunition* ID_Ammunition = Cast<UInventoryItemDefinition_Ammunition>(Entry.Instance->GetItemDef()))
					{
						if (WeaponTags.HasTag(ID_Ammunition->WeaponClassification))
						{
							return Entry.Instance;
						}
					}
				}
			}
		}

		return nullptr;
	};

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
