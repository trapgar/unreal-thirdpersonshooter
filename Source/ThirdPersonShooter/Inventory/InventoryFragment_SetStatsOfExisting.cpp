// Copyright Epic Games, Inc. All Rights Reserved.

#include "Inventory/InventoryFragment_SetStatsOfExisting.h"

#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryManagerComponent.h"
#include "Inventory/InventoryFragment_EquippableItem.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_SetStatsOfExisting)

void UInventoryFragment_SetStatsOfExisting::OnInstanceCreated(UInventoryItemInstance* Instance) const
{
	UObject* Outer = Instance->GetOuter();
	check(Outer);

	AActor* Owner = Cast<AActor>(Outer);
	check(Owner);

	// Set the stats on the item (ammo box, etc.)
	for (const auto& KVP : AdditionalItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}

	// Find the 1st item in the inventory that matches the item definition
	if (UInventoryManagerComponent* Inventory = Owner->GetComponentByClass<UInventoryManagerComponent>())
	{
		for (UInventoryItemInstance* Item : Inventory->GetAllItems())
		{
			if (Item->GetItemDef()->IsA(ItemDefinition))
			{
				// And transfer all the stats
				// This way, collected items where we don't find any matching item will be retained until one is added
				for (const auto& KVP : AdditionalItemStats)
				{
					// TODO: Check for a max stack count
					int32 StackCount = Item->GetStatTagStackCount(KVP.Key);
					Item->AddStatTagStack(KVP.Key, StackCount + KVP.Value);
					Instance->RemoveStatTagStack(KVP.Key, KVP.Value);
				}

				return;
			}
		}
	}
}
