// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EquipmentItemInstance.h"
#include "EquipmentItemDefinition.h"
#include "Character/Components/Ability/ModularAbilitySet.h"
#include "Components/PawnComponent.h"
// #include "Net/Serialization/FastArraySerializer.h"

#include "EquipmentManagerComponent.generated.h"

class UActorComponent;
class UAbilitySystemComponent;
class UEquipmentItemDefinition;
class AEquipmentItemInstance;
class UEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FEquipmentList;


USTRUCT(BlueprintType)
struct FEquipmentSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Equipment)
	TObjectPtr<APawn> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category = Equipment)
	TArray<TObjectPtr<AEquipmentItemInstance>> Slots;
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

	FAppliedEquipmentEntry()
	{}

	FString GetDebugString() const;

private:
	friend FEquipmentList;
	friend UEquipmentManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<AEquipmentItemInstance> Instance = nullptr;

	// Authority-only list of granted handles
	UPROPERTY(NotReplicated)
	FModularAbilitySet_GrantedHandles GrantedHandles;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FEquipmentList
{
	GENERATED_BODY()

public:
	AEquipmentItemInstance* AddEntry(TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition);
	void RemoveEntry(AEquipmentItemInstance* Instance);

	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FAppliedEquipmentEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return false;
	}

private:
	UAbilitySystemComponent* GetAbilitySystemComponent() const;
	
	friend UEquipmentManagerComponent;
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
UCLASS(BlueprintType, Const)
class UEquipmentManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UEquipmentManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	AEquipmentItemInstance* EquipItem(TSubclassOf<UEquipmentItemDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(AEquipmentItemInstance* ItemInstance);

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

	/** Returns the first equipped instance of a given type, or nullptr if none are found */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	AEquipmentItemInstance* GetFirstInstanceOfType(TSubclassOf<AEquipmentItemInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AEquipmentItemInstance*> GetEquipmentInstancesOfType(TSubclassOf<AEquipmentItemInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, AEquipmentItemInstance* Item);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	AEquipmentItemInstance* RemoveItemFromSlot(int32 SlotIndex);

protected:

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<AEquipmentItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	void UnequipItem();
	void EquipItem();

private:

	// List of all equipped items
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;

	// Need a also have a list of holstered items or drawn items
};
