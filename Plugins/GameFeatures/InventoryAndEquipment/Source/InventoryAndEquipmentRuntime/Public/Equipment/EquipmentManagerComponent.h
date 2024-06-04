// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
// #include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentItemDefinition.h"
#include "EquipmentItemInstance.h"
#include "Ability/ModularAbilitySet.h"
#include "Inventory/InventoryManagerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"

#include "EquipmentManagerComponent.generated.h"

class AEquipmentItemInstance;
struct FEquipmentList;


USTRUCT(BlueprintType)
struct FEquipmentChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	TObjectPtr<UActorComponent> EquipmentOwner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	TObjectPtr<AEquipmentItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	int32 Delta = 0;
};


// --------------------------------------------------------


USTRUCT(BlueprintType)
struct FEquipmentActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	TObjectPtr<APawn> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	int32 ActiveIndex = 0;
};


// --------------------------------------------------------


/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FEquipmentEntry
{
	GENERATED_BODY()

	FEquipmentEntry() {}

	FString GetDebugString() const;

	bool IsApplied() const { return bIsApplied; }
	void Apply();
	void Unapply();

	UAbilitySystemComponent* GetAbilitySystemComponent() const { return Cast<UAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instance->GetOwner())); }

private:
	friend FEquipmentList;
	friend UEquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<AEquipmentItemInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FModularAbilitySet_GrantedHandles GrantedHandles;

	// Flag indicating if the equipment is 'applied' (e.g.: active vs holstered)
	bool bIsApplied = false;
};


// --------------------------------------------------------


/** List of applied equipment */
USTRUCT(BlueprintType)
struct FEquipmentList
{
	GENERATED_BODY()

	FEquipmentList()
		: OwnerComponent(nullptr)
	{
	}

	FEquipmentList(UActorComponent* InOwningComponent)
		: OwnerComponent(InOwningComponent)
	{
	}

public:
	TArray<AEquipmentItemInstance*> GetAllItems() const;

	AEquipmentItemInstance* AddEntry(TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition);

	void RemoveEntry(AEquipmentItemInstance* Instance);

	// Returns true if the entry was previously unapplied, false otherwise
	bool ApplyEntry(AEquipmentItemInstance* Instance);

	void UnapplyEntry(AEquipmentItemInstance* Instance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return false;
	}

private:
	void BroadcastChangeMessage(AEquipmentItemInstance* Entry, int32 OldCount, int32 NewCount);

private:
	friend UEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FEquipmentList> : public TStructOpsTypeTraitsBase2<FEquipmentList>
{
	enum { WithNetDeltaSerializer = true };
};


// --------------------------------------------------------


/**
 * Manages equipment applied to a pawn. Equipment is inventory that can be seen & potentially 'used' by the player.
 */
UCLASS(Blueprintable, BlueprintType)
class INVENTORYANDEQUIPMENTRUNTIME_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	AEquipmentItemInstance* AddItem(TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	void RemoveItem(AEquipmentItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Equipment, BlueprintPure = false)
	TArray<AEquipmentItemInstance*> GetAllItems() const;

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, Category=Equipment, BlueprintPure)
	AEquipmentItemInstance* GetFirstInstanceOfType(TSubclassOf<AEquipmentItemInstance> InstanceType);
	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, Category=Equipment, BlueprintPure)
	TArray<AEquipmentItemInstance*> GetEquipmentInstancesOfType(TSubclassOf<AEquipmentItemInstance> InstanceType) const;

	// Gets the item in the given slot index
	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category=Equipment)
	AEquipmentItemInstance* GetItemInSlot(int32 SlotIndex);

	// Adds an item to the given slot index
	UFUNCTION(BlueprintCallable, Category=Equipment)
	void AddItemToSlot(int32 SlotIndex, AEquipmentItemInstance* Item);

	// Removes an item from the given slot index
	UFUNCTION(BlueprintCallable, Category=Equipment)
	AEquipmentItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	// Draws the item in the given slot index
	UFUNCTION(BlueprintCallable, Category=Equipment)
	AEquipmentItemInstance* DrawItemInSlot(int32 SlotIndex);

	// Holsters the item in the given slot index
	UFUNCTION(BlueprintCallable, Category=Equipment)
	void HolsterItemInSlot(int32 SlotIndex);

protected:
	// Number that controls the max number of quickslots we have (e.g.: IA_QuickbarSlot 1-5 will try to draw an item)
	UPROPERTY()
	int32 NumSlots = 5;

	virtual void BeginPlay() override;

	//~UObject interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipmentItemAdded"))
	void K2_OnEquipmentItemAdded(AEquipmentItemInstance* EquipmentItemInstance);

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipmentItemRemoved"))
	void K2_OnEquipmentItemRemoved(AEquipmentItemInstance* EquipmentItemInstance);

private:
	UPROPERTY()
	TArray<TObjectPtr<AEquipmentItemInstance>> Slots;

	// List of all equipped items
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;

	FGameplayMessageListenerHandle ListenerHandle;

	void OnInventoryStackChanged(FGameplayTag Channel, const FInventoryChangedMessage& Notification);

};

