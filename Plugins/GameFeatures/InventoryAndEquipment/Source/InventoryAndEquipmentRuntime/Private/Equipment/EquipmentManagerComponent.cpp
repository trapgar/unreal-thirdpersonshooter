// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/EquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "Engine/ActorChannel.h"
#include "Equipment/EquipmentItemDefinition.h"
#include "Equipment/EquipmentItemInstance.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "CustomLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG(TAG_Equipment_Message_StackChanged, "Equipment.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_Equipment_Message_ActiveIndexChanged, "Equipment.Message.ActiveIndexChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_Equipment_Weapon_Ammunition, "Equipment.Weapon.Ammunition");

// --------------------------------------------------------
// FEquipmentEntry

FString FEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(Instance->GetItemDef()));
}

void FEquipmentList::BroadcastChangeMessage(UEquipmentItemInstance* Instance, int32 OldCount, int32 NewCount)
{
	FEquipmentChangedMessage Message;
	Message.Source = OwnerComponent;
	Message.Instance = Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Equipment_Message_StackChanged, Message);
}

UEquipmentItemInstance* FEquipmentList::AddEntry(UEquipmentItemDefinition* EquipmentDef, UInventoryItemInstance* Source)
{
	UEquipmentItemInstance* Result = nullptr;

	check(EquipmentDef != nullptr);
	check(OwnerComponent);

	APawn* Owner = Cast<APawn>(OwnerComponent->GetOwner());
	check(Owner->HasAuthority());
	
	TSubclassOf<UEquipmentItemInstance> InstanceType = EquipmentDef->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = UEquipmentItemInstance::StaticClass();
	}

	FEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	Result = NewEntry.Instance = NewObject<UEquipmentItemInstance>(OwnerComponent->GetOwner(), InstanceType); //@TODO: Using the actor instead of component as the outer due to UE-127172
	Result->SetItemDef(EquipmentDef);
	Result->SetInstigator(Owner);
	Result->SetAssociatedItem(Source);

	for (UEquipmentItemFragment* Fragment : EquipmentDef->Fragments)
	{
		if (Fragment != nullptr)
		{
			Fragment->OnInstanceCreated(Result);
		}
	}

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (auto AbilitySet : EquipmentDef->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &GrantedHandles, Result);
		}
	}
	else
	{
		UE_LOG(LogEquipment, Warning, TEXT("Cannot grant abilities from %s when the UAbilitySystemComponent is missing."), *GetNameSafe(Result));
	}

	// MarkItemDirty(NewEntry);
	// BroadcastChangeMessage(Result, /*OldCount=*/0, /*NewCount=*/StackCount);

	return Result;
}

void FEquipmentList::RemoveEntry(UEquipmentItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				GrantedHandles.TakeFromAbilitySystem(ASC);
			}
			else
			{
				UE_LOG(LogEquipment, Warning, TEXT("Cannot remove abilities from %s when the UAbilitySystemComponent is missing."), *GetNameSafe(Instance));
			}

			Instance->OnUnequipped();

			for (UEquipmentItemFragment* Fragment : Instance->GetItemDef()->Fragments)
			{
				Fragment->OnInstanceDestroyed(Instance);
			}

			Instance->DestroySpawnedActors();

			EntryIt.RemoveCurrent();
			// MarkArrayDirty();
		}
	}
}

TArray<UEquipmentItemInstance*> FEquipmentList::GetAllItems() const
{
	TArray<UEquipmentItemInstance*> Results;
	Results.Reserve(Entries.Num());

	for (const FEquipmentEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}

	return Results;
}

// --------------------------------------------------------
// UEquipmentManagerComponent

UEquipmentManagerComponent::UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = true;
}

void UEquipmentManagerComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	for (UEquipmentItemInstance* Instance : EquipmentList.GetAllItems())
	{
		Instance->Tick(DeltaTime);
	}
}

void UEquipmentManagerComponent::UninitializeComponent()
{
	TArray<UEquipmentItemInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (UEquipmentItemInstance* ItemInstance : AllEquipmentInstances)
	{
		RemoveItem(ItemInstance);
	}

	Super::UninitializeComponent();
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

bool UEquipmentManagerComponent::ReplicateSubobjects(UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags)
{
	bool WroteSomething = Super::ReplicateSubobjects(Channel, Bunch, RepFlags);

	for (FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		UEquipmentItemInstance* Instance = Entry.Instance;

		if (IsValid(Instance))
		{
			WroteSomething |= Channel->ReplicateSubobject(Instance, *Bunch, *RepFlags);
		}
	}

	return WroteSomething;
}

void UEquipmentManagerComponent::ReadyForReplication()
{
	Super::ReadyForReplication();

	// Register existing EquipmentInstances
	if (IsUsingRegisteredSubObjectList())
	{
		for (const FEquipmentEntry& Entry : EquipmentList.Entries)
		{
			UEquipmentItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

UEquipmentItemInstance* UEquipmentManagerComponent::AddItem(UEquipmentItemDefinition* EquipmentClass, UInventoryItemInstance* Source)
{
	if (EquipmentClass != nullptr)
	{
		if (UEquipmentItemInstance* Result = EquipmentList.AddEntry(EquipmentClass, Source))
		{
			Result->OnEquipped();
			K2_OnEquipmentItemAdded(Result);

			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}

			return Result;
		}
	}

	return nullptr;
}

UEquipmentItemInstance* UEquipmentManagerComponent::AddItemByDefinition(TSubclassOf<UEquipmentItemDefinition> ItemDefinition, UInventoryItemInstance* Source)
{
	UEquipmentItemInstance* ItemInstance = nullptr;

	if (ItemDefinition != nullptr)
	{
		ItemInstance = AddItem(ItemDefinition->GetDefaultObject<UEquipmentItemDefinition>(), Source);
	}

	return ItemInstance;
}

void UEquipmentManagerComponent::RemoveItem(UEquipmentItemInstance* ItemInstance)
{
	if (ItemInstance != nullptr)
	{
		if (IsUsingRegisteredSubObjectList())
		{
			RemoveReplicatedSubObject(ItemInstance);
		}

		EquipmentList.RemoveEntry(ItemInstance);
		K2_OnEquipmentItemRemoved(ItemInstance);
	}
}

TArray<UEquipmentItemInstance*> UEquipmentManagerComponent::GetAllItems() const
{
	return EquipmentList.GetAllItems();
}

UEquipmentItemInstance* UEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<UEquipmentItemInstance> InstanceType)
{
	for (FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentItemInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				return Instance;
			}
		}
	}

	return nullptr;
}

TArray<UEquipmentItemInstance*> UEquipmentManagerComponent::GetEquipmentInstancesOfType(TSubclassOf<UEquipmentItemInstance> InstanceType) const
{
	TArray<UEquipmentItemInstance*> Results;

	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		if (UEquipmentItemInstance* Instance = Entry.Instance)
		{
			if (Instance->IsA(InstanceType))
			{
				Results.Add(Instance);
			}
		}
	}

	return Results;
}
