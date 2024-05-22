#include "InventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"
#include "NativeGameplayTags.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Inventory_Message_StackChanged, "Inventory.Message.StackChanged");

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
	Message.InventoryOwner = OwnerComponent;
	Message.Instance = Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem &MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Inventory_Message_StackChanged, Message);
}

UInventoryItemInstance *FInventoryList::AddEntry(UInventoryItemDefinition* ItemDef, int32 StackCount)
{
	UInventoryItemInstance* Result = nullptr;

	check(ItemDef != nullptr);
	// TODO: Why is this sometimes null???
	check(OwnerComponent);

	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

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

	return Result;
}

void FInventoryList::RemoveEntry(UInventoryItemInstance *Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FInventoryEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			// Instance->Destroy();

			EntryIt.RemoveCurrent();
			// MarkArrayDirty();
		}
	}
}

TArray<UInventoryItemInstance *> FInventoryList::GetAllItems() const
{
	TArray<UInventoryItemInstance *> Results;
	Results.Reserve(Entries.Num());

	for (const FInventoryEntry &Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
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
		}
	}
	return Result;
}

UInventoryItemInstance* UInventoryManagerComponent::AddItemByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefinition, int32 StackCount)
{
	if (ItemDefinition != nullptr)
	{
		return AddItem(ItemDefinition->GetDefaultObject<UInventoryItemDefinition>(), StackCount);
	}
	else
	{
		return nullptr;
	}
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
	}
}

TArray<UInventoryItemInstance*> UInventoryManagerComponent::GetAllItems() const
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

UInventoryItemInstance* UInventoryManagerComponent::FindFirstItemStackByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const
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
				++TotalCount;
			}
		}
	}

	return TotalCount;
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
