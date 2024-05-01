#include "InventoryManagerComponent.h"

#include "Engine/ActorChannel.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemInstance.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

//////////////////////////////////////////////////////////////////////
// FInventoryEntry

FString FInventoryEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(ItemDefinition.Get()));
}

//////////////////////////////////////////////////////////////////////
// FInventoryList

AInventoryItemInstance* FInventoryList::AddEntry(TSubclassOf<UInventoryItemDefinition> ItemDefinition)
{
	AInventoryItemInstance* Result = nullptr;

	check(ItemDefinition != nullptr);
 	check(OwnerComponent);
	check(OwnerComponent->GetOwner()->HasAuthority());
	
	TSubclassOf<AInventoryItemInstance> InstanceType = AInventoryItemInstance::StaticClass();
	
	FInventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.ItemDefinition = ItemDefinition;
	NewEntry.Instance = NewObject<AInventoryItemInstance>(OwnerComponent->GetOwner(), InstanceType);  //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(ItemDefinition);
	Result = NewEntry.Instance;

	// MarkItemDirty(NewEntry);

	return Result;
}

TArray<AInventoryItemInstance*> FInventoryList::GetAllItems() const
{
	TArray<AInventoryItemInstance*> Results;
	Results.Reserve(Entries.Num());
	for (const FInventoryEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}
	return Results;
}

void FInventoryList::RemoveEntry(AInventoryItemInstance *Instance)
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

//////////////////////////////////////////////////////////////////////
// UInventoryManagerComponent

UInventoryManagerComponent::UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}

void UInventoryManagerComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty >& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, InventoryList);
}

AInventoryItemInstance* UInventoryManagerComponent::AddItem(TSubclassOf<UInventoryItemDefinition> ItemClass)
{
	AInventoryItemInstance* Result = nullptr;
	if (ItemClass != nullptr)
	{
		Result = InventoryList.AddEntry(ItemClass);
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

void UInventoryManagerComponent::RemoveItem(AInventoryItemInstance* ItemInstance)
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

TArray<AInventoryItemInstance*> UInventoryManagerComponent::GetAllItems() const
{
	return InventoryList.GetAllItems();
}

bool UInventoryManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FInventoryEntry& Entry : InventoryList.Entries)
	{
		AInventoryItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UInventoryManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UInventoryManagerComponent::UninitializeComponent()
{
	TArray<AInventoryItemInstance*> AllInventoryInstances;

	// gathering all instances before removal to avoid side effects affecting the item list iterator
	for (const FInventoryEntry& Entry : InventoryList.Entries)
	{
		AllInventoryInstances.Add(Entry.Instance);
	}

	for (AInventoryItemInstance* ItemInstance : AllInventoryInstances)
	{
		RemoveItem(ItemInstance);
	}

	Super::UninitializeComponent();
}

void UInventoryManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing ItemInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FInventoryEntry& Entry : InventoryList.Entries)
		{
			AInventoryItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}
