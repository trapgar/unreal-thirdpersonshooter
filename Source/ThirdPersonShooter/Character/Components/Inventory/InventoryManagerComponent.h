#pragma once

#include "CoreMinimal.h"
#include "Components/PawnComponent.h"
// #include "Net/Serialization/FastArraySerializer.h"
#include "InventoryItemDefinition.h"
#include "InventoryItemInstance.h"

#include "InventoryManagerComponent.generated.h"

class AInventoryItemInstance;

/** A single piece of applied equipment */
USTRUCT(BlueprintType)
struct FInventoryEntry
{
	GENERATED_BODY()

	FInventoryEntry()
	{}

	FString GetDebugString() const;

private:
	friend FInventoryList;
	friend UInventoryManagerComponent;

	// The equipment class that got equipped
	UPROPERTY()
	TSubclassOf<UInventoryItemDefinition> ItemDefinition;

	UPROPERTY()
	TObjectPtr<AInventoryItemInstance> Instance = nullptr;
};

/** List of applied equipment */
USTRUCT(BlueprintType)
struct FInventoryList
{
	GENERATED_BODY()

	TArray<AInventoryItemInstance*> GetAllItems() const;

public:
	AInventoryItemInstance* AddEntry(TSubclassOf<UInventoryItemDefinition> InventoryItemDefinition);
	void RemoveEntry(AInventoryItemInstance* Instance);

	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FInventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return false;
	}
};

template<>
struct TStructOpsTypeTraits<FInventoryList> : public TStructOpsTypeTraitsBase2<FInventoryList>
{
	enum { WithNetDeltaSerializer = true };
};

UCLASS(BlueprintType, Const)
class THIRDPERSONSHOOTER_API UInventoryManagerComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInventoryManagerComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	AInventoryItemInstance* AddItem(TSubclassOf<UInventoryItemDefinition> ItemDefinition);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveItem(AInventoryItemInstance* ItemInstance);

	UFUNCTION(BlueprintCallable, Category=Inventory, BlueprintPure=false)
	TArray<AInventoryItemInstance*> GetAllItems() const;

	//~UObject interface
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	//~End of UObject interface

	//~UActorComponent interface
	//virtual void EndPlay() override;
	virtual void InitializeComponent() override;
	virtual void UninitializeComponent() override;
	virtual void ReadyForReplication() override;
	//~End of UActorComponent interface

private:
	// Array to store the inventory items
	UPROPERTY(Replicated)
	FInventoryList InventoryList;
};
