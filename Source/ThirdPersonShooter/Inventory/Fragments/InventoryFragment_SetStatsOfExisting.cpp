// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryFragment_SetStatsOfExisting.h"

#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryManagerComponent.h"
#include "InventoryFragment_EquippableItem.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_SetStatsOfExisting)

#define LOCTEXT_NAMESPACE "Inventory"

#if WITH_EDITOR
EDataValidationResult UInventoryFragment_SetStatsOfExisting::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (ItemDefinition == nullptr)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(LOCTEXT("EntryHasNullActor", "Null ItemDefinition"));
	}

	return Result;
}
#endif

void UInventoryFragment_SetStatsOfExisting::OnInstanceCreated(UInventoryItemInstance* Instance) const
{
	UObject* Outer = Instance->GetOuter();
	check(Outer);

	AActor* Owner = Cast<AActor>(Outer);
	check(Owner);

	// Set the stats on the inventory item (ammo box, etc.)
	for (const auto& KVP : AdditionalItemStats)
	{
		Instance->AddStatTagStack(KVP.Key, KVP.Value);
	}

	// Then look through all the items in the inventory
	if (UInventoryManagerComponent* Inventory = Owner->GetComponentByClass<UInventoryManagerComponent>())
	{
		for (FReadOnlyInventoryEntry Entry : Inventory->GetAllItems())
		{
			// Find the 1st item in the inventory that matches the item definition off this fragment
			if (Entry.Instance->GetItemDef()->IsA(ItemDefinition))
			{
				// And transfer all the stats
				// This way, collected items where we don't find any matching item will be retained until one is added
				// ---
				// The B_InventoryManagerComponent listens for new inventory items and scrubs empty ammo boxes by gameplay tag
				for (const auto& KVP : AdditionalItemStats)
				{
					// @TODO: Check for a max stack count
					// @TODO: If the player has more than 1 item of this type, which one should we set stats of?
					Entry.Instance->AddStatTagStack(KVP.Key, KVP.Value);
					Instance->RemoveStatTagStack(KVP.Key, KVP.Value);
				}

				return;
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
