// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/QuickBarComponent.h"

#include "Equipment/EquipmentItemDefinition.h"
#include "Equipment/EquipmentItemInstance.h"
#include "Inventory/Fragments/InventoryFragment_EquippableItem.h"
#include "Inventory/Fragments/InventoryFragment_QuickbarCosmeticInfo.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/Pawn.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(QuickBarComponent)

class FLifetimeProperty;
class UEquipmentItemDefinition;

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Message_QuickBar_SlotsChanged, "Gameplay.Message.QuickBar.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Message_QuickBar_ActiveIndexChanged, "Gameplay.Message.QuickBar.ActiveIndexChanged");

UQuickBarComponent::UQuickBarComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
}

void UQuickBarComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, Slots);
	DOREPLIFETIME(ThisClass, ActiveSlotIndex);
}

void UQuickBarComponent::BeginPlay()
{
	if (Slots.Num() < NumberOfSlots)
	{
		Slots.AddDefaulted(NumberOfSlots - Slots.Num());
	}

	Super::BeginPlay();
}

void UQuickBarComponent::CycleActiveSlotForward()
{
	// tfw when you only have 1 quickslot
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;

	do
	{
		NewIndex = (NewIndex + 1) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UQuickBarComponent::CycleActiveSlotBackward()
{
	// tfw when you only have 1 quickslot
	if (Slots.Num() < 2)
	{
		return;
	}

	const int32 OldIndex = (ActiveSlotIndex < 0 ? Slots.Num() - 1 : ActiveSlotIndex);
	int32 NewIndex = ActiveSlotIndex;

	do
	{
		NewIndex = (NewIndex - 1 + Slots.Num()) % Slots.Num();
		if (Slots[NewIndex] != nullptr)
		{
			SetActiveSlotIndex(NewIndex);
			return;
		}
	} while (NewIndex != OldIndex);
}

void UQuickBarComponent::EquipItemInSlot()
{
	check(Slots.IsValidIndex(ActiveSlotIndex));
	check(EquippedItem == nullptr);

	if (UInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	{
		if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
		{
			TSubclassOf<UEquipmentItemDefinition> EquipDef = EquipInfo->EquipmentDefinition;
			if (EquipDef != nullptr)
			{
				if (UEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
				{
					EquippedItem = EquipmentManager->AddItemByDefinition(EquipDef, SlotItem);
				}
			}
		}
	}
}

void UQuickBarComponent::UnequipItemInSlot()
{
	if (UEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	{
		if (EquippedItem != nullptr)
		{
			EquipmentManager->RemoveItem(EquippedItem);
			EquippedItem = nullptr;
		}
	}
}

void UQuickBarComponent::SetActiveSlotIndex_Implementation(int32 NewIndex)
{
	if (Slots.IsValidIndex(NewIndex) && (ActiveSlotIndex != NewIndex))
	{
		// don't allow swapping to an empty slot
		if (UInventoryItemInstance* SlotItem = Slots[NewIndex])
		{
			UnequipItemInSlot();
			ActiveSlotIndex = NewIndex;
			EquipItemInSlot();

			OnRep_ActiveSlotIndex();
		}
	}
}

UInventoryItemInstance* UQuickBarComponent::GetActiveSlotItem() const
{
	return Slots.IsValidIndex(ActiveSlotIndex) ? Slots[ActiveSlotIndex] : nullptr;
}

int32 UQuickBarComponent::GetNextFreeItemSlot() const
{
	int32 SlotIndex = 0;
	for (TObjectPtr<UInventoryItemInstance> ItemPtr : Slots)
	{
		if (ItemPtr == nullptr)
		{
			return SlotIndex;
		}
		++SlotIndex;
	}

	return INDEX_NONE;
}

void UQuickBarComponent::AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;

			UInventoryItemDefinition* ItemDef = Item->GetItemDef();

			// TODO: Interface?
			for (UInventoryItemFragment* Fragment : ItemDef->Fragments)
			{
				if (UInventoryFragment_QuickbarCosmeticInfo* Cosmetic = Cast<UInventoryFragment_QuickbarCosmeticInfo>(Fragment))
				{
					Cosmetic->OnInstanceAdded(Item);
				}
			}

			OnRep_Slots();

			// Item in active slot ran out of charges etc, but was just restocked - re-equip
			if (ActiveSlotIndex == SlotIndex)
			{
				UnequipItemInSlot();
				EquipItemInSlot();
			}
		}
	}
}

UInventoryItemInstance* UQuickBarComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	UInventoryItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItemInSlot();
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			Result->DestroySpawnedActors();
			OnRep_Slots();
		}
	}

	return Result;
}

void UQuickBarComponent::OnRep_Slots()
{
	FQuickBarSlotsChangedMessage Message;
	Message.Source = this;
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Gameplay_Message_QuickBar_SlotsChanged, Message);
}

void UQuickBarComponent::OnRep_ActiveSlotIndex()
{
	FQuickBarActiveIndexChangedMessage Message;
	Message.Source = this;
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Gameplay_Message_QuickBar_ActiveIndexChanged, Message);
}

