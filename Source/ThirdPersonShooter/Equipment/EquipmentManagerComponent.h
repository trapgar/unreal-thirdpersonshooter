// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"
// #include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentItemDefinition.h"
#include "EquipmentItemInstance.h"
#include "GameplayAbilities/ModularAbilitySet.h"
#include "Inventory/InventoryManagerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"

#include "EquipmentManagerComponent.generated.h"

class UEquipmentItemInstance;
struct FEquipmentList;


THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Equipment_Message_StackChanged);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Equipment_Message_ActiveIndexChanged);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Equipment_Weapon_Ammunition);

// --------------------------------------------------------


USTRUCT(BlueprintType)
struct FEquipmentChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	TObjectPtr<UActorComponent> Source = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	TObjectPtr<UEquipmentItemInstance> Instance = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	int32 Delta = 0;
};


// --------------------------------------------------------


/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FEquipmentEntry
{
	GENERATED_BODY()

	FEquipmentEntry() {}

	FString GetDebugString() const;

private:
	friend FEquipmentList;
	friend UEquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<UEquipmentItemInstance> Instance = nullptr;
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
	TArray<UEquipmentItemInstance*> GetAllItems() const;

	UEquipmentItemInstance* AddEntry(UEquipmentItemDefinition* EquipmentDefinition, UInventoryItemInstance* Source);

	void RemoveEntry(UEquipmentItemInstance* Instance);

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return false;
	}

private:
	void BroadcastChangeMessage(UEquipmentItemInstance* Entry, int32 OldCount, int32 NewCount);

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FModularAbilitySet_GrantedHandles GrantedHandles;

private:

	UAbilitySystemComponent* GetAbilitySystemComponent() const
	{
		return Cast<UAbilitySystemComponent>(UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwnerComponent->GetOwner()));
	}

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
 * UEquipmentManagerComponent
 * 
 * Manages equipment applied to a pawn. Equipment is inventory that can be seen & potentially 'used' by the player.
 */
UCLASS(Blueprintable, BlueprintType)
class THIRDPERSONSHOOTER_API UEquipmentManagerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Adds an equipment item to the inventory
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	UEquipmentItemInstance* AddItem(UEquipmentItemDefinition* EquipmentDefinition) { return AddItem(EquipmentDefinition, nullptr); };
	UEquipmentItemInstance* AddItem(UEquipmentItemDefinition* EquipmentDefinition, UInventoryItemInstance* Source);

	// Adds an equipment item by definition to the inventory
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	UEquipmentItemInstance* AddItemByDefinition(TSubclassOf<UEquipmentItemDefinition> ItemDefinition) { return AddItemByDefinition(ItemDefinition, nullptr); };
	UEquipmentItemInstance* AddItemByDefinition(TSubclassOf<UEquipmentItemDefinition> ItemDefinition, UInventoryItemInstance* Source);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	void RemoveItem(UEquipmentItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Equipment, BlueprintPure = false)
	TArray<UEquipmentItemInstance*> GetAllItems() const;

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, Category=Equipment, BlueprintPure)
	UEquipmentItemInstance* GetFirstInstanceOfType(TSubclassOf<UEquipmentItemInstance> InstanceType);
	template <typename T>
	T* GetFirstInstanceOfType() { return (T*)GetFirstInstanceOfType(T::StaticClass()); }

	/** Returns all equipped instances of a given type, or an empty array if none are found */
	UFUNCTION(BlueprintCallable, Category=Equipment, BlueprintPure)
	TArray<UEquipmentItemInstance*> GetEquipmentInstancesOfType(TSubclassOf<UEquipmentItemInstance> InstanceType) const;

public:
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

protected:
	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipmentItemAdded"))
	void K2_OnEquipmentItemAdded(UEquipmentItemInstance* EquipmentItemInstance);

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipmentItemRemoved"))
	void K2_OnEquipmentItemRemoved(UEquipmentItemInstance* EquipmentItemInstance);

private:
	// List of all equipped items
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;

};
