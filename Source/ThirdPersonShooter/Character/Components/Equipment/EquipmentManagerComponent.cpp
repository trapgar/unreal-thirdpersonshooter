// Copyright Epic Games, Inc. All Rights Reserved.

#include "EquipmentManagerComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Engine/ActorChannel.h"
#include "EquipmentItemDefinition.h"
#include "EquipmentItemInstance.h"
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

//////////////////////////////////////////////////////////////////////
// FAppliedEquipmentEntry

FString FAppliedEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(Instance->GetItemDef()));
}

//////////////////////////////////////////////////////////////////////
// FEquipmentList

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

	FTransform SpawnTransform = Owner->GetActorTransform();
	USceneComponent* Attachee = Owner->GetRootComponent();
	FName SocketName = "weapon_back_shoulder_l";

	// Find the skeletal mesh to attach to
	// NOTE: Unreal recommends not attaching directly to the SKM, but instead with it's own animations
	if (USkeletalMeshComponent* Skele = Owner->GetComponentByClass<USkeletalMeshComponent>())
	{
		Attachee = Skele;
		FTransform SkeleTransform = Skele->GetComponentTransform();

		if (EquipmentCDO->AttachTransform.IsValid())
		{
			// TODO: Get the socket name from a lookup table
			FTransform SocketTransform = Skele->GetSocketTransform(SocketName, ERelativeTransformSpace::RTS_Actor);
			SpawnTransform = EquipmentCDO->AttachTransform.GetRelativeTransform(SocketTransform);
		}
		else
		{
			SpawnTransform = SkeleTransform;
		}
	}
	
	FActorSpawnParameters SpawnInfo;

	if (APawn* Instigator = Cast<APawn>(Owner))
	{
		SpawnInfo.Instigator = Instigator;
	}

	FAppliedEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = World->SpawnActor<AEquipmentItemInstance>(InstanceType, SpawnTransform, SpawnInfo); //@TODO: Using the actor instead of component as the outer due to UE-127172
	NewEntry.Instance->SetItemDef(EquipmentDefinition);
	Result = NewEntry.Instance;

	// Attach to the appropriate socket if valid
	if (EquipmentCDO->AttachSocket.IsValid())
	{
		// TODO: Get the socket name from a lookup table
		if (NewEntry.Instance->AttachToComponent(Attachee, FAttachmentTransformRules::KeepRelativeTransform, SocketName))
		{
			NewEntry.Instance->SetActorHiddenInGame(false);
		}
		else
		{
			UE_LOG(LogEquipment, Warning, TEXT("Failed to attach equipment %s to %s - most likely SKM was not found."), *EquipmentCDO->GetName(), *Attachee->GetName());
		}
	}

	// Only passive items give their abilities on equip
	if (EquipmentCDO->bIsPassive)
	{
		if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
		{
			for (auto AbilitySet : EquipmentCDO->AbilitySetsToGrant)
			{
				AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &NewEntry.GrantedHandles, Result);
			}
		}
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

TArray<AEquipmentItemInstance*> FEquipmentList::GetAllItems() const
{
	TArray<AEquipmentItemInstance*> Results;
	Results.Reserve(Entries.Num());

	for (const FAppliedEquipmentEntry& Entry : Entries)
	{
		if (Entry.Instance != nullptr) //@TODO: Would prefer to not deal with this here and hide it further?
		{
			Results.Add(Entry.Instance);
		}
	}

	return Results;
}

//////////////////////////////////////////////////////////////////////
// UEquipmentManagerComponent

UEquipmentManagerComponent::UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, EquipmentList(this)
{
	SetIsReplicatedByDefault(true);
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UEquipmentManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, EquipmentList);
}

AEquipmentItemInstance* UEquipmentManagerComponent::AddItem(TSubclassOf<UEquipmentItemDefinition> EquipmentClass)
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

void UEquipmentManagerComponent::RemoveItem(AEquipmentItemInstance* ItemInstance)
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

TArray<AEquipmentItemInstance*> UEquipmentManagerComponent::GetAllItems() const
{
	return EquipmentList.GetAllItems();
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

	for (AEquipmentItemInstance* ItemInstance : AllEquipmentInstances)
	{
		RemoveItem(ItemInstance);
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
		AEquipmentItemInstance* Result = Slots[SlotIndex];

		// TODO

		return Result;
	}

	return nullptr;
}

void UEquipmentManagerComponent::HolsterItemInSlot(int32 SlotIndex)
{
	if (Slots.IsValidIndex(SlotIndex))
	{
		// TODO
	}
}

const bool UEquipmentFunctionLibrary::IsActive(FAppliedEquipmentEntry Entry)
{
	return Entry.IsActive();
}
