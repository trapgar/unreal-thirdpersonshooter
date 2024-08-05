// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Common/TaggedActor.h"
#include "Common/GameplayTagStack.h"
#include "Inventory/InventoryItemDefinition.h"

#include "InventoryItemInstance.generated.h"

class FLifetimeProperty;
class UInventoryItemDefinition;
struct FFrame;
struct FGameplayTag;

/**
 * UInventoryItemInstance
 * 
 * Represents an instance of an inventory item.
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORYANDEQUIPMENTRUNTIME_API UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginDestroy() override;

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category=Inventory)
	APawn* GetInstigator() const { return Cast<APawn>(GetOuter()); };

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount) { StatTags.AddStack(Tag, StackCount); };

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount) { StatTags.RemoveStack(Tag, StackCount); };

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const { return StatTags.GetStackCount(Tag); };

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const { return StatTags.ContainsTag(Tag); };

	UFUNCTION(BlueprintCallable, BlueprintPure=true, Category=Inventory)
	UInventoryItemDefinition* GetItemDef() const { return ItemDef; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
	{
		return ItemDef ? ItemDef->FindFragmentByClass(FragmentClass) : nullptr;
	}

	template <typename TResultClass>
	const TResultClass* FindFragmentByClass() const { return (TResultClass*)FindFragmentByClass(TResultClass::StaticClass()); }

	void SetItemDef(UInventoryItemDefinition* InDef) { ItemDef = InDef; };

	void AddSpawnedActor(AActor* Actor) { SpawnedActors.Emplace(Actor); }

	UFUNCTION(BlueprintCallable, BlueprintPure=true, Category=Inventory)
	TArray<AActor*>& GetSpawnedActors() { return SpawnedActors; }

	template <typename TResultClass>
	TResultClass* FindSpawnedActorByClass() { return (TResultClass*)FindSpawnedActorByClass(TResultClass::StaticClass()); }

	UFUNCTION(BlueprintCallable, BlueprintPure=true, meta=(DeterminesOutputType=ActorClass))
	AActor* FindSpawnedActorByClass(TSubclassOf<AActor> ActorClass)
	{
		for (AActor* Actor : SpawnedActors)
		{
			if (Actor && Actor->GetClass()->IsChildOf(ActorClass))
			{
				return Actor;
			}
		}

		return nullptr;
	}

	void DestroySpawnedActors()
	{
		for (AActor* Actor : SpawnedActors)
		{
			if (Actor)
			{
				Actor->Destroy();
			}
		}
	}

	friend struct FInventoryList;

private:

	UPROPERTY()
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	UInventoryItemDefinition* ItemDef;

	TArray<AActor*> SpawnedActors;
};
