// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Common/TaggedActor.h"
#include "Common/GameplayTagStack.h"

#include "InventoryItemInstance.generated.h"

class FLifetimeProperty;
class UInventoryItemDefinition;
struct FFrame;
struct FGameplayTag;

/**
 * UInventoryItemInstance
 */
UCLASS(BlueprintType)
class UInventoryItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Inventory)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category= Inventory)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Inventory)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Inventory)
	bool HasStatTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=true, Category = "Inventory")
	UInventoryItemDefinition* GetItemDef() const
	{
		return ItemDef;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
	{
		// return ItemDef ? ItemDef->FindFragmentByClass(FragmentClass) : nullptr;
		return nullptr;
	}

	void SetItemDef(UInventoryItemDefinition* InDef);

	friend struct FInventoryList;

private:
	UPROPERTY()
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	UInventoryItemDefinition* ItemDef;
};
