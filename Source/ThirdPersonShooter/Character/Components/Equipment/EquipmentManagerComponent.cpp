// Copyright Epic Games, Inc. All Rights Reserved.

#include "EquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
// #include "EquipmentItemDefinition.h"
// #include "EquipmentItemInstance.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Character/Components/Inventory/InventoryFragment_EquippableItem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Message_SlotsChanged, "Equipment.Message.SlotsChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Message_ActiveIndexChanged, "Equipment.Message.ActiveIndexChanged");

//////////////////////////////////////////////////////////////////////
// FAppliedEquipmentEntry

FString FAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(EquipmentDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FEquipmentList

UAbilitySystemComponent* FEquipmentList::GetAbilitySystemComponent() const
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	return Cast<UAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor));
}

AEquipmentItemInstance* FEquipmentList::AddEntry(TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition)
{
	AEquipmentItemInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
 	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	const UEquipmentItemDefinition* EquipmentCDO = GetDefault<UEquipmentItemDefinition>(EquipmentDefinition);

	TSubclassOf<AEquipmentItemInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = AEquipmentItemInstance::StaticClass();
	}
	
	FAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.EquipmentDefinition = EquipmentDefinition;
	NewEntry.Instance = NewObject<AEquipmentItemInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result = NewEntry.Instance;

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (auto AbilitySet : EquipmentCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
		}
	}
	else
	{
		//@TODO: Warning logging?
	}

	// MarkItemDirty(NewEntry);

	return Result;
}

void FEquipmentList::RemoveEntry(AEquipmentItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FAppliedEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				Entry.GrantedHandles.TakeFromAbilitySystem(ASC);
			}

			Instance->Destroy();

			EntryIt.RemoveCurrent();
			// MarkArrayDirty();
		}
	}
}

//////////////////////////////////////////////////////////////////////
// UEquipmentManagerComponent

UEquipmentManagerComponent::UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

AEquipmentItemInstance* UEquipmentManagerComponent::EquipItem(TSubclassOf<UEquipmentItemDefinition> EquipmentClass)
{
	AEquipmentItemInstance* Result = nullptr;
	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result != nullptr)
		{
			Result->OnEquipped();

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}
		}
	}
	return Result;
}

void UEquipmentManagerComponent::UnequipItem(AEquipmentItemInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		ItemInstance->OnUnequipped();
		EquipmentList.RemoveEntry(ItemInstance);
	}
}

bool UEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AEquipmentItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UEquipmentManagerComponent::UninitializeComponent()
{
	TArray<AEquipmentItemInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (AEquipmentItemInstance* EquipInstance : AllEquipmentInstances)
	{
		UnequipItem(EquipInstance);
	}

	Super::UninitializeComponent();
}

void UEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing EquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
		{
			AEquipmentItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

AEquipmentItemInstance* UEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<AEquipmentItemInstance> InstanceType)
{
	for (FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (AEquipmentItemInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<AEquipmentItemInstance*> UEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<AEquipmentItemInstance> InstanceType) const
{
	TArray<AEquipmentItemInstance*> Results;
	for (const FAppliedEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (AEquipmentItemInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}
	return Results;
}

void UEquipmentManagerComponent::AddItemToSlot(int32 SlotIndex, AEquipmentItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
			OnRep_Slots();
		}
	}
}

AEquipmentItemInstance* UEquipmentManagerComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	AEquipmentItemInstance* Result = nullptr;

	if (ActiveSlotIndex == SlotIndex)
	{
		UnequipItem();
		ActiveSlotIndex = -1;
	}

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
			OnRep_Slots();
		}
	}

	return Result;
}

void UEquipmentManagerComponent::OnRep_Slots()
{
	FEquipmentSlotsChangedMessage Message;
	Message.Owner = GetOwner<APawn>();
	Message.Slots = Slots;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Equipment_Message_SlotsChanged, Message);
}

void UEquipmentManagerComponent::OnRep_ActiveSlotIndex()
{
	FEquipmentActiveIndexChangedMessage Message;
	Message.Owner = GetOwner<APawn>();
	Message.ActiveIndex = ActiveSlotIndex;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	MessageSystem.BroadcastMessage(TAG_Equipment_Message_ActiveIndexChanged, Message);
}

void UEquipmentManagerComponent::EquipItem()
{
	// check(Slots.IsValidIndex(ActiveSlotIndex));
	// check(EquippedItem == nullptr);

	// if (ULyraInventoryItemInstance* SlotItem = Slots[ActiveSlotIndex])
	// {
	// 	if (const UInventoryFragment_EquippableItem* EquipInfo = SlotItem->FindFragmentByClass<UInventoryFragment_EquippableItem>())
	// 	{
	// 		TSubclassOf<ULyraEquipmentDefinition> EquipDef = EquipInfo->EquipmentDefinition;
	// 		if (EquipDef != nullptr)
	// 		{
	// 			if (ULyraEquipmentManagerComponent* EquipmentManager = FindEquipmentManager())
	// 			{
	// 				EquippedItem = EquipmentManager->EquipItem(EquipDef);
	// 				if (EquippedItem != nullptr)
	// 				{
	// 					EquippedItem->SetInstigator(SlotItem);
	// 				}
	// 			}
	// 		}
	// 	}
	// }
}

void UEquipmentManagerComponent::UnequipItem()
{
	
}
