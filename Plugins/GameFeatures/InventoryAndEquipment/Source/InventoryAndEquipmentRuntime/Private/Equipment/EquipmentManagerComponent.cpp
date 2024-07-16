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

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Message_StackChanged, "Equipment.Message.StackChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Message_ActiveIndexChanged, "Equipment.Message.ActiveIndexChanged");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Weapon_Ammunition, "Equipment.Weapon.Ammunition");

// --------------------------------------------------------
// FEquipmentEntry

FString FEquipmentEntry::GetDebugString() const
{
	return FString::Printf(TEXT("%s of %s"), *GetNameSafe(Instance), *GetNameSafe(Instance->GetItemDef()));
}

void FEquipmentList::BroadcastChangeMessage(AEquipmentItemInstance* Instance, int32 OldCount, int32 NewCount)
{
	FEquipmentChangedMessage Message;
	Message.Source = OwnerComponent;
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
	TSubclassOf<AEquipmentItemInstance> ActorToSpawn = EquipmentCDO->ActorToSpawn;
	if (ActorToSpawn == nullptr)
	{
		ActorToSpawn = AEquipmentItemInstance::StaticClass();
	}

	FTransform SpawnTransform = FTransform(FQuat::Identity, FVector::ZeroVector, FVector(1.0f, 1.0f, 1.0f));
	USceneComponent* Attachee = Owner->GetRootComponent();
	FName SocketName = "";

	// Find the skeletal mesh to attach to
	// NOTE: Unreal recommends not attaching directly to the SKM, but instead with it's own animations (I think?)
	if (USkeletalMeshComponent* Skele = Owner->GetComponentByClass<USkeletalMeshComponent>())
	{
		Attachee = Skele;
		SocketName = EquipmentCDO->AttachSocketName;
		SpawnTransform = EquipmentCDO->AttachTransform;
	}
	
	FActorSpawnParameters SpawnInfo;
	SpawnInfo.Owner = Owner;

	if (APawn* Instigator = Cast<APawn>(Owner))
	{
		SpawnInfo.Instigator = Instigator;
	}

	// TODO: add to the inventory SpawnedActor list
	FEquipmentEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Instance = World->SpawnActorDeferred<AEquipmentItemInstance>(ActorToSpawn, SpawnTransform, SpawnInfo.Owner, SpawnInfo.Instigator);
	NewEntry.Instance->SetItemDef(EquipmentDefinition);
	NewEntry.Instance->FinishSpawning(SpawnTransform);
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

	// Only passive items give their abilities on equip
	const UEquipmentItemDefinition* EquipmentDefinitionCDO = GetDefault<UEquipmentItemDefinition>(EquipmentDefinition);

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		for (auto AbilitySet : EquipmentDefinitionCDO->AbilitySetsToGrant)
		{
			AbilitySet->GiveToAbilitySystem(ASC, /*inout*/ &GrantedHandles, Result);
		}
	}
	else
	{
		UE_LOG(LogEquipment, Warning, TEXT("Cannot grant abilities from %s when the UAbilitySystemComponent is missing."), *GetNameSafe(Result));
	}

	Result->OnEquipped();

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
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
			{
				GrantedHandles.TakeFromAbilitySystem(ASC);
			}
			else
			{
				UE_LOG(LogEquipment, Warning, TEXT("Cannot remove abilities from %s when the UAbilitySystemComponent is missing."), *GetNameSafe(Instance));
			}

			Instance->OnUnequipped();
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
	if (EquipmentClass != nullptr)
	{
		if (AEquipmentItemInstance* Result = EquipmentList.AddEntry(EquipmentClass))
		{
			if (IsUsingRegisteredSubObjectList() && IsReadyForReplication())
			{
				AddReplicatedSubObject(Result);
			}

			K2_OnEquipmentItemAdded(Result);

			return Result;
		}
	}

	return nullptr;
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
