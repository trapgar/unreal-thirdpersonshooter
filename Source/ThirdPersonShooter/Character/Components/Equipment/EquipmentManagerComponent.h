// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EquipmentInstance.h"
#include "EquipmentDefinition.h"
#include "Character/Components/Ability/ModularAbilitySet.h"
#include "Components/PawnComponent.h"
// #include "Net/Serialization/FastArraySerializer.h"

#include "EquipmentManagerComponent.generated.h"

class UActorComponent;
class UAbilitySystemComponent;
class UEquipmentDefinition;
class AEquipmentInstance;
class UEquipmentManagerComponent;
class UObject;
struct FFrame;
struct FEquipmentList;


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
	TSubclassOf<UEquipmentDefinition> EquipmentDefinition;

	UPROPERTY()
	TObjectPtr<AEquipmentInstance> Instance = nullptr;

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
	AEquipmentInstance* AddEntry(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);
	void RemoveEntry(AEquipmentInstance* Instance);

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
	AEquipmentInstance* EquipItem(TSubclassOf<UEquipmentDefinition> EquipmentDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnequipItem(AEquipmentInstance* ItemInstance);

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
	AEquipmentInstance* GetFirstInstanceOfType(TSubclassOf<AEquipmentInstance> InstanceType);

 	/** Returns all equipped instances of a given type, or an empty array if none are found */
 	UFUNCTION(BlueprintCallable, BlueprintPure)
	TArray<AEquipmentInstance*> GetEquipmentInstancesOfType(TSubclassOf<AEquipmentInstance> InstanceType) const;

	template <typename T>
	T* GetFirstInstanceOfType()
	{
		return (T*)GetFirstInstanceOfType(T::StaticClass());
	}

private:
	UPROPERTY(Replicated)
	FEquipmentList EquipmentList;
};
