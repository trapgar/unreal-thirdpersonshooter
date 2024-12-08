#include "Inventory/InventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Inventory/InventoryItemDefinition.h"
#include "Inventory/InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Message_Inventory_StackChanged, "Gameplay.Message.Inventory.StackChanged");

// --------------------------------------------------------
// FInventoryEntry

FString FInventoryEntry::GetDebugString() const
{
	UInventoryItemDefinition* ItemDef = nullptr;
	if (Instance != nullptr)
	{
		ItemDef = Instance->GetItemDef();
	}

	return FString::Printf(TEXT("%s (%d x %s)"), *GetNameSafe(Instance), StackCount, *GetNameSafe(ItemDef));
}

// --------------------------------------------------------
// FInventoryList

void FInventoryList::BroadcastChangeMessage(UInventoryItemInstance* Instance, int32 OldCount, int32 NewCount)
{
	FInventoryChangedMessage Message;
	Message.Source = OwnerComponent;
	Message.Instance = Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem &MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Gameplay_Message_Inventory_StackChanged, Message);
}

UInventoryItemInstance *FInventoryList::AddEntry(UInventoryItemDefinition* ItemDef, int32 StackCount)
{
	UInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	// Item is stackable - check to see if we already have items in the inv of the same class
	// if (ItemDef->MaxStackCount > 1)
	// {
	// 	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	// 	{
	// 		FInventoryEntry& Entry = *EntryIt;
	// 		// Don't care about MaxStackCount yet, as that's a display problem, not a functional one
	// 		// All we care about here is IF it's stackable, as stackable items indicate they are all identical,
	// 		//   so we don't really about preserving instance-data
	// 		if (Entry.Instance->GetItemDef()->GetClass() == ItemDef->GetClass())
	// 		{
	// 			Result = Entry.Instance;
	// 			Entry.StackCount += StackCount;

	// 			// MarkArrayDirty();
	// 			BroadcastChangeMessage(Entry.Instance, /*OldCount=*/Entry.StackCount - StackCount, /*NewCount=*/Entry.StackCount);
	// 			break;
	// 		}
	// 	}
	// }

	// Either not stackable, or we didn't find any matches
	if (Result == nullptr)
	{
		TSubclassOf<UInventoryItemInstance> InstanceType = UInventoryItemInstance::StaticClass();

		FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
		NewEntry.Instance = NewObject<UInventoryItemInstance>(OwnerComponent->GetOwner()); //@TODO: Using the actor instead of component as the outer due to UE-127172
		NewEntry.Instance->SetItemDef(ItemDef);

		for (UInventoryItemFragment* Fragment : ItemDef->Fragments)
		{
			if (Fragment != nullptr)
			{
				Fragment->OnInstanceCreated(NewEntry.Instance);
			}
		}

		NewEntry.StackCount = StackCount;
		Result = NewEntry.Instance;

		// MarkItemDirty(NewEntry);
		BroadcastChangeMessage(Result, /*OldCount=*/0, /*NewCount=*/StackCount);
	}

	return Result;
}

void FInventoryList::RemoveEntry(UInventoryItemInstance *Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			int32 OldCount = Entry.StackCount;
			Entry.StackCount -= 1;

			if (Entry.StackCount <= 0)
			{
				EntryIt.RemoveCurrent();
			}

			// MarkArrayDirty();
			BroadcastChangeMessage(Instance, /*OldCount=*/OldCount, /*NewCount=*/OldCount - 1);
		}
	}
}

TArray<FReadOnlyInventoryEntry> FInventoryList::GetAllItems() const
{
	TArray<FReadOnlyInventoryEntry> Results;
	Results.Reserve(Entries.Num());

	for (const FInventoryEntry &Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			FReadOnlyInventoryEntry CopiedEntry;
			CopiedEntry.Instance= Entry.Instance;
			CopiedEntry.StackCount = Entry.StackCount;
			Results.Add(CopiedEntry);
		}
	}

	return Results;
}

// --------------------------------------------------------
// UInventoryManagerComponent

UInventoryManagerComponent::UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, InventoryList(this)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

UInventoryItemInstance* UInventoryManagerComponent::AddItem(UInventoryItemDefinition* ItemDefinition, int32 StackCount)
{
	UInventoryItemInstance* Result = nullptr;
	if (ItemDefinition != nullptr)
	{
		Result = InventoryList.AddEntry(ItemDefinition, StackCount);
		if (Result != nullptr)
		{
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}

			K2_OnItemAdded(Result);
		}
	}

	return Result;
}

UInventoryItemInstance* UInventoryManagerComponent::AddItemByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefinition, int32 StackCount)
{
	UInventoryItemInstance* ItemInstance = nullptr;
	
	if (ItemDefinition != nullptr)
	{
		ItemInstance = AddItem(ItemDefinition->GetDefaultObject<UInventoryItemDefinition>(), StackCount);
	}

	return ItemInstance;
}

void UInventoryManagerComponent::RemoveItem(UInventoryItemInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		InventoryList.RemoveEntry(ItemInstance);
		K2_OnItemRemoved(ItemInstance);
	}
}

TArray<FReadOnlyInventoryEntry> UInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

bool UInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryEntry& Entry : InventoryList.Entries)
	{
		UInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

UInventoryItemInstance* UInventoryManagerComponent::FindFirstItemByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const
{
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		UInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef()->IsA(ItemDef))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

int32 UInventoryManagerComponent::GetItemStackCount(UInventoryItemInstance* ItemInstance) const
{
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		if (Entry.Instance == ItemInstance)
		{
			return Entry.StackCount;
		}
	}

	return 0;
}

int32 UInventoryManagerComponent::GetTotalItemCountByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const
{
	int32 TotalCount = 0;
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		UInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			if (Instance->GetItemDef()->IsA(ItemDef))
			{
				TotalCount += Entry.StackCount;
			}
		}
	}

	return TotalCount;
}

bool UInventoryManagerComponent::ConsumeItemsByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 NumToConsume)
{
	AActor* OwningActor = GetOwner();
	if (!OwningActor || !OwningActor->HasAuthority())
	{
		return false;
	}

	//@TODO: N squared right now as there's no acceleration structure
	int32 TotalConsumed = 0;
	while (TotalConsumed < NumToConsume)
	{
		if (UInventoryItemInstance* Instance = FindFirstItemByDefinition(ItemDef))
		{
			InventoryList.RemoveEntry(Instance);
			++TotalConsumed;
		}
		else
		{
			return false;
		}
	}

	return TotalConsumed == NumToConsume;
}

void UInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing ItemInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FInventoryEntry& Entry : InventoryList.Entries)
		{
			UInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}
