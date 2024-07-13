#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// #include "Net/Serialization/FastArraySerializer.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemInstance.h"

#include "InventoryManagerComponent.generated.h"

class UInventoryItemInstance;
struct FInventoryList;

/** A message when an item is added to the inventory */
USTRUCT(BlueprintType)
struct FInventoryChangedMessage
{
	GENERATED_BODY()

	//@TODO: Tag based names+owning actors for inventories instead of directly exposing the component?
	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UActorComponent> Source = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	TObjectPtr<UInventoryItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category = Inventory)
	int32 Delta = 0;
};


// --------------------------------------------------------


/** A single inventory stack */
USTRUCT(BlueprintType)
struct FInventoryEntry
{
	GENERATED_BODY()

	FInventoryEntry() {}

	FString GetDebugString() const;

private:
	friend FInventoryList;
	friend UInventoryManagerComponent;

	// Total count of the item
	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY()
	TObjectPtr<UInventoryItemInstance> Instance = nullptr;
};


// --------------------------------------------------------


/** List of inventory items */
USTRUCT(BlueprintType)
struct FInventoryList
{
	GENERATED_BODY()

	FInventoryList()
		: OwnerComponent(nullptr)
	{
	}

	FInventoryList(UActorComponent *InOwningComponent)
		: OwnerComponent(InOwningComponent)
	{
	}

public:
	TArray<UInventoryItemInstance *> GetAllItems() const;

	UInventoryItemInstance* AddEntry(UInventoryItemDefinition* ItemDef, int32 StackCount);
	// UInventoryItemInstance* AddEntry(TSubclassOf<UInventoryItemDefinition> ItemDef, int32 StackCount);
	void RemoveEntry(UInventoryItemInstance *Instance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo &DeltaParms)
	{
		return false;
	}

private:
	void BroadcastChangeMessage(UInventoryItemInstance *Entry, int32 OldCount, int32 NewCount);

private:
	friend UInventoryManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};


// --------------------------------------------------------


template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};

/** 
 * Manages inventory held by a pawn. Inventory is any item that the pawn is capable of carrying.
 * While all equipment is technically part of the inventory, it is handled separately by the `EquipmentManagerComponent`
 * as it may be worn by the pawn or have abilities to use.
 * 
 * TODO: There may be a better way to do this:
 * Can be a bit weird as we may need to juggle the stats of items applied to an `UInventoryItemInstance` vs an `AEquipmentItemInstance`
 * e.g.:
 * 	1. You pick up a rifle with 30 rounds and put it in your inventory (`UInventoryItemInstance`)
 * 	2. You 'equip' the rifle and we transfer it to the `EquipmentManagerComponent` which also transfers the stats to the
 * 	`AEquipmentItemInstance` because it's now spawned as an actor
 * 	3. You 'unequip' the rifle and we put it back in your inventory, which also transfers the stats back from the `AEquipmentItemInstance`
 * 	to the `UInventoryItemInstance` as the equipment is despawned
 */
UCLASS(Blueprintable, BlueprintType)
class INVENTORYANDEQUIPMENTRUNTIME_API UInventoryManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UInventoryItemInstance* AddItem(UInventoryItemDefinition* ItemDefinition, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	UInventoryItemInstance* AddItemByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDefinition, int32 StackCount = 1);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItem(UInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<UInventoryItemInstance*> GetAllItems() const;

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure)
	UInventoryItemInstance* FindFirstItemStackByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const;

	int32 GetTotalItemCountByDefinition(TSubclassOf<UInventoryItemDefinition> ItemDef) const;

protected:
	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

	UFUNCTION(BlueprintImplementableEvent, Category=Inventory, meta=(DisplayName="OnItemAdded"))
	void K2_OnItemAdded(UInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintImplementableEvent, Category=Inventory, meta=(DisplayName="OnItemRemoved"))
	void K2_OnItemRemoved(UInventoryItemInstance* ItemInstance);

private:
	// Array to store the inventory items
	UPROPERTY(Replicated)
	FInventoryList InventoryList;
};
