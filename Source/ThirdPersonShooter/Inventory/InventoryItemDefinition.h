// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Common/GameplayTagStack.h"
#include "Kismet/BlueprintFunctionLibrary.h"

#include "InventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UInventoryItemInstance;


// --------------------------------------------------------


// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class THIRDPERSONSHOOTER_API UInventoryItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UInventoryItemInstance* Instance) const {}
};


// --------------------------------------------------------


/**
 * Definition type of an inventory item
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Const, Abstract)
class UInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Display name of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	// Display icon of the item
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	TObjectPtr<UMaterialInterface> Icon;

	// Max number of items in a single inventory stack
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default, meta=(ClampMin=1, UIMin=1))
	int32 MaxStackCount = 1;

	// Gameplay-related tags associated with this inventory item (e.g.: "Primary", "Secondary", "Utility", etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default, meta=(Categories="Item.Classification"))
	FGameplayTagContainer StaticGameplayTags;

	// List of fragments for the item
	// @see `UInventoryItemFragment` for what fragments do
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default, Instanced)
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;

public:
	// Returns the first matching item fragment based on class
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;

	template<class T>
	const T* FindFragmentByClass() const
	{
		static_assert(std::is_base_of<UInventoryItemFragment, T>::value, "T must be a subclass of UInventoryItemFragment");
		return (T*)FindFragmentByClass(T::StaticClass());
	}
};


// --------------------------------------------------------


// @TODO: Make into a subsystem instead?
UCLASS()
class UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	// Returns the first matching item fragment based on class
	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UInventoryItemFragment* FindItemDefinitionFragment(UInventoryItemDefinition* ItemDef, TSubclassOf<UInventoryItemFragment> FragmentClass);
};
