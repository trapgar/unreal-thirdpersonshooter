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

	if (UInventoryManagerComponent* Inventory = Owner->GetComponentByClass<UInventoryManagerComponent>())
	{
		for (UInventoryItemInstance* Item : Inventory->GetAllItems())
		{
			if (Item->GetItemDef()->IsA(ItemDefinition))
			{
				for (const auto& KVP : AdditionalItemStats)
				{
					int32 StackCount = Item->GetStatTagStackCount(KVP.Key);
					Item->AddStatTagStack(KVP.Key, StackCount + KVP.Value);
				}

				return;
			}
		}
	}
}
