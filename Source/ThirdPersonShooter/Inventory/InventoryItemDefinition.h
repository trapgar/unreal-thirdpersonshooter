// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	TObjectPtr<UMaterialInterface> Icon;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default)
	bool bStackable = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Default, Instanced)
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;

public:
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;

	template<class T>
	const T* FindFragmentByClass() const
	{
		static_assert(std::is_base_of<UInventoryItemFragment, T>::value, "T must be a subclass of UInventoryItemFragment");
		return (T*)FindFragmentByClass(T::StaticClass());
	}
};


// --------------------------------------------------------


//@TODO: Make into a subsystem instead?
UCLASS()
class UInventoryFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UInventoryItemFragment* FindItemDefinitionFragment(UInventoryItemDefinition* ItemDef, TSubclassOf<UInventoryItemFragment> FragmentClass);
};
