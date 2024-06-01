// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/EquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "Engine/ActorChannel.h"
#include "Equipment/EquipmentItemDefinition.h"
#include "Equipment/EquipmentItemInstance.h"
#include "Equipment/EquipmentFragment_AttachmentInfo.h"
#include "Inventory/InventoryFragment_EquippableItem.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"
#include "Net/UnrealNetwork.h"
#include "CustomLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentManagerComponent)

class FLifetimeProperty;
struct FReplicationFlags;

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Message_StackChanged, "Equipment.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Message_ActiveIndexChanged, "Equipment.Message.ActiveIndexChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Weapon_Ammunition, "Equipment.Weapon.Ammunition");

// --------------------------------------------------------
// FEquipmentEntry

FString FEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(Instance->GetItemDef()));
}

void FEquipmentEntry::Apply()
{
	if (bIsApplied)
	{
		return;
	}

	const UEquipmentItemDefinition* EquipmentDefinition = GetDefault<UEquipmentItemDefinition>(Instance->GetItemDef());

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (auto AbilitySet : EquipmentDefinition->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &GrantedHandles, Instance);
		}
	}
	else
	{
		UE_LOG(LogEquipment, Warning, TEXT("Cannot grant abilities from %s when the UAbilitySystemComponent is missing."), *GetNameSafe(Instance));
	}

	bIsApplied = true;
	Instance->OnEquipped();
}

void FEquipmentEntry::Unapply()
{
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		GrantedHandles.TakeFromAbilitySystem(ASC);
	}
	else
	{
		UE_LOG(LogEquipment, Warning, TEXT("Cannot remove abilities from %s when the UAbilitySystemComponent is missing."), *GetNameSafe(Instance));
	}

	bIsApplied = false;
	Instance->OnUnequipped();
}

void FEquipmentList::BroadcastChangeMessage(AEquipmentItemInstance* Instance, int32 OldCount, int32 NewCount)
{
	FEquipmentChangedMessage Message;
	Message.EquipmentOwner = OwnerComponent;
	Message.Instance = Instance;
	Message.NewCount = NewCount;
	Message.Delta = NewCount - OldCount;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(OwnerComponent->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Equipment_Message_StackChanged, Message);
}

AEquipmentItemInstance* FEquipmentList::AddEntry(TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition)
{
	AEquipmentItemInstance* Result = nullptr;

	check(EquipmentDefinition != nullptr);
	check(OwnerComponent);
	AActor* Owner = OwnerComponent->GetOwner();
	check(Owner->HasAuthority());
	UWorld* World = Owner->GetWorld();
	check(World);

	const UEquipmentItemDefinition* EquipmentCDO = GetDefault<UEquipmentItemDefinition>(EquipmentDefinition);

	// If for some reason the equipment class isn't set, use the default (NOTE: I think this is abstract so idk if this will break?)
	TSubclassOf<AEquipmentItemInstance> InstanceType = EquipmentCDO->InstanceType;
	if (InstanceType == nullptr)
	{
		InstanceType = AEquipmentItemInstance::StaticClass();
	}

	FTransform SpawnTransform = FTransform(FQuat::Identity, FVector::ZeroVector, FVector(1.0f, 1.0f, 1.0f));
	USceneComponent* Attachee = Owner->GetRootComponent();
	FName SocketName = "";

	// Find the skeletal mesh to attach to
	// NOTE: Unreal recommends not attaching directly to the SKM, but instead with it's own animations (I think?)
	if (USkeletalMeshComponent* Skele = Owner->GetComponentByClass<USkeletalMeshComponent>())
	{
		Attachee = Skele;
		FTransform SkeleTransform = Skele->GetComponentTransform();

		if (UEquipmentFragment_AttachmentInfo* AttachmentInfo = EquipmentCDO->FindFragmentByClass<UEquipmentFragment_AttachmentInfo>())
		{
			SocketName = AttachmentInfo->HolsteredSocketName;
			FTransform SocketTransform = Skele->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_Actor);
			SpawnTransform = AttachmentInfo->HolsteredTransform;
		}
	}
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = Owner;

	if (APawn* Instigator = Cast<APawn>(Owner))
	{
		SpawnInfo.Instigator = Instigator;
	}

	FEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = World->SpawnActor<AEquipmentItemInstance>(InstanceType, SpawnTransform, SpawnInfo); //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(EquipmentDefinition);
	Result = NewEntry.Instance;

	// Attach to the appropriate socket if valid
	if (SocketName.IsValid())
	{
		if (Result->AttachToComponent(Attachee, FAttachmentTransformRules::KeepRelativeTransform, SocketName))
		{
			Result->SetActorHiddenInGame(false);
		}
		else
		{
			UE_LOG(LogEquipment, Warning, TEXT("Failed to attach equipment %s to %s - most likely SKM was not found."), *EquipmentCDO->GetName(), *Attachee->GetName());
		}
	}

	Result->OnReady();

	// Only passive items give their abilities on equip
	if (EquipmentCDO->bIsPassive)
	{
		NewEntry.Apply();
	}

	// MarkItemDirty(NewEntry);

	return Result;
}

void FEquipmentList::RemoveEntry(AEquipmentItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance)
		{
			Entry.Unapply();
			Instance->Destroy();
			EntryIt.RemoveCurrent();
			// MarkArrayDirty();
		}
	}
}

bool FEquipmentList::ApplyEntry(AEquipmentItemInstance* Instance)
{
	bool bApplied = false;

	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance && !Entry.IsApplied())
		{
			Entry.Apply();
			bApplied = true;
			return bApplied;
		}
	}

	return bApplied;
}

void FEquipmentList::UnapplyEntry(AEquipmentItemInstance* Instance)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		FEquipmentEntry& Entry = *EntryIt;
		if (Entry.Instance == Instance && Entry.IsApplied())
		{
			Entry.Unapply();
		}
	}
}

TArray<AEquipmentItemInstance*> FEquipmentList::GetAllItems() const
{
	TArray<AEquipmentItemInstance*> Results;
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
	PrimaryComponentTick.bStartWithTickEnabled = false;

	// Make sure to initialize the slots array
	if (Slots.Num() < NumSlots)
	{
		Slots.AddDefaulted(NumSlots - Slots.Num());
	}
}

void UEquipmentManagerComponent::BeginPlay()
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	FGameplayTag TAG_Inventory_Message_StackChanged = FGameplayTag::RequestGameplayTag(FName("Inventory.Message.StackChanged"));
	ListenerHandle = MessageSubsystem.RegisterListener(TAG_Inventory_Message_StackChanged, this, &ThisClass::OnInventoryStackChanged);

	Super::BeginPlay();
}

void UEquipmentManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UEquipmentManagerComponent::UninitializeComponent()
{
	TArray<AEquipmentItemInstance*> AllEquipmentInstances;

	// gathering all instances before removal to avoid side effects affecting the equipment list iterator	
	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
	{
		AllEquipmentInstances.Add(Entry.Instance);
	}

	for (AEquipmentItemInstance* ItemInstance : AllEquipmentInstances)
	{
		RemoveItem(ItemInstance);
	}

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);

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
		AEquipmentItemInstance* Instance = Entry.Instance;

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
			AEquipmentItemInstance* Instance = Entry.Instance;

			if (IsValid(Instance))
			{
				AddReplicatedSubObject(Instance);
			}
		}
	}
}

AEquipmentItemInstance* UEquipmentManagerComponent::AddItem(TSubclassOf<UEquipmentItemDefinition> EquipmentClass)
{
	AEquipmentItemInstance* Result = nullptr;

	if (EquipmentClass != nullptr)
	{
		Result = EquipmentList.AddEntry(EquipmentClass);
		if (Result != nullptr)
		{
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}

			K2_OnEquipmentItemAdded(Result);
		}
	}

	return Result;
}

void UEquipmentManagerComponent::RemoveItem(AEquipmentItemInstance* ItemInstance)
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

TArray<AEquipmentItemInstance*> UEquipmentManagerComponent::GetAllItems() const
{
	return EquipmentList.GetAllItems();
}

// TODO: implies that every equippable item in the inventory will be automatically equipped... probably not the case
void UEquipmentManagerComponent::OnInventoryStackChanged(FGameplayTag Channel, const FInventoryChangedMessage& Notification)
{
	// Item added
	if (Notification.NewCount > 0)
	{
		if (const UInventoryItemDefinition* ItemDefinition = Notification.Instance->GetItemDef())
		{
			if (const UInventoryFragment_EquippableItem* Fragment = ItemDefinition->FindFragmentByClass<UInventoryFragment_EquippableItem>())
			{
				AEquipmentItemInstance* EquipmentItem = AddItem(Fragment->EquipmentDefinition);

				// Transfer any equipped weapon ammunition to the new item
				Notification.Instance->AddSpawnedActor(EquipmentItem);
				int32 Ammunition = Notification.Instance->GetStatTagStackCount(TAG_Equipment_Weapon_Ammunition);
				EquipmentItem->AddStatTagStack(TAG_Equipment_Weapon_Ammunition, Ammunition);
				Notification.Instance->RemoveStatTagStack(TAG_Equipment_Weapon_Ammunition, Ammunition);
			}
		}
	}
	// Item removed
	else
	{
		for (AActor* Actor : Notification.Instance->GetSpawnedActors())
		{
			if (AEquipmentItemInstance* EquipmentItem = Cast<AEquipmentItemInstance>(Actor))
			{
				RemoveItem(EquipmentItem);
			}
		}
	}
}

AEquipmentItemInstance* UEquipmentManagerComponent::GetFirstInstanceOfType(TSubclassOf<AEquipmentItemInstance> InstanceType)
{
	for (FEquipmentEntry& Entry : EquipmentList.Entries)
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

	for (const FEquipmentEntry& Entry : EquipmentList.Entries)
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

AEquipmentItemInstance *UEquipmentManagerComponent::GetItemInSlot(int32 SlotIndex)
{
	AEquipmentItemInstance* Result = nullptr;

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];
	}

	return Result;
}

void UEquipmentManagerComponent::AddItemToSlot(int32 SlotIndex, AEquipmentItemInstance* Item)
{
	if (Slots.IsValidIndex(SlotIndex) && (Item != nullptr))
	{
		if (Slots[SlotIndex] == nullptr)
		{
			Slots[SlotIndex] = Item;
		}
	}
}

AEquipmentItemInstance* UEquipmentManagerComponent::RemoveItemFromSlot(int32 SlotIndex)
{
	AEquipmentItemInstance* Result = nullptr;

	if (Slots.IsValidIndex(SlotIndex))
	{
		Result = Slots[SlotIndex];

		if (Result != nullptr)
		{
			Slots[SlotIndex] = nullptr;
		}
	}

	return Result;
}

AEquipmentItemInstance* UEquipmentManagerComponent::DrawItemInSlot(int32 SlotIndex)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		if (AEquipmentItemInstance* Result = Slots[SlotIndex])
		{
			EquipmentList.ApplyEntry(Result);

			return Result;
		}
	}

	return nullptr;
}

void UEquipmentManagerComponent::HolsterItemInSlot(int32 SlotIndex)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		if (AEquipmentItemInstance* Result = Slots[SlotIndex])
		{
			EquipmentList.UnapplyEntry(Result);
		}
	}
}
