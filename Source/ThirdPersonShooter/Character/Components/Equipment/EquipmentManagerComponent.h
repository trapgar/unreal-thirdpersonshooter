// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
// #include "Net/Serialization/FastArraySerializer.h"
#include "EquipmentItemDefinition.h"
#include "EquipmentItemInstance.h"
#include "Character/Components/Ability/ModularAbilitySet.h"

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
struct FAppliedEquipmentEntry
{
	GENERATED_BODY()

	FAppliedEquipmentEntry() {}

	FString GetDebugString() const;

	bool IsActive() const { return bIsActive; }

private:
	friend FEquipmentList;
	friend UEquipmentManagerComponent;

	UPROPERTY()
	TObjectPtr<AEquipmentItemInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FModularAbilitySet_GrantedHandles GrantedHandles;

	// Flag indicating if the equipment is active vs holstered
	bool bIsActive = false;
};

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

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return false;
	}

private:
	UAbilitySystemComponent* GetAbilitySystemComponent() const;

private:
	void BroadcastChangeMessage(AEquipmentItemInstance* Entry, int32 OldCount, int32 NewCount);

private:
	friend UEquipmentManagerComponent;

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FAppliedEquipmentEntry> Entries;

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
 * Manages equipment applied to a pawn
 */
UCLASS(Blueprintable, BlueprintType)
class THIRDPERSONSHOOTER_API UEquipmentManagerComponent : public UActorComponent
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

	// Adds an item to the given slot index
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, AEquipmentItemInstance* Item);

	// Removes an item from the given slot index
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	AEquipmentItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	// Draws the item in the given slot index
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	AEquipmentItemInstance* DrawItemInSlot(int32 SlotIndex);

	// Holsters the item in the given slot index
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	void HolsterItemInSlot(int32 SlotIndex);

protected:
	//~UObject interface
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	virtual void ReadyForReplication() override;
	//~End of UObject interface

private:
	UPROPERTY()
	TArray<TObjectPtr<AEquipmentItemInstance>> Slots;

	// List of all equipped items
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;

};

//@TODO: Make into a subsystem instead?
UCLASS()
class UEquipmentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Returns a flag indicating if the equipment is active or holstered
	UFUNCTION(BlueprintCallable, BlueprintPure)
	static const bool IsActive(FAppliedEquipmentEntry Entry);
};
