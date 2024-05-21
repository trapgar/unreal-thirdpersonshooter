// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "InventoryItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class UInventoryItemInstance;
struct FFrame;


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
 * UInventoryItemDefinition
 * ---
 * This isn't used, I'm experimenting with this being a struct instead of a class so I don't have to create a new subclass for every unique item
 */
USTRUCT(Blueprintable)
struct FInventoryItemDefinitionAlt
{
	GENERATED_BODY()

public:
	FInventoryItemDefinitionAlt() {}

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;
};


// --------------------------------------------------------


/**
 * UInventoryItemDefinition
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Const, Abstract)
class UInventoryItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UInventoryItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display)
	FText DisplayName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Display, Instanced)
	TArray<TObjectPtr<UInventoryItemFragment>> Fragments;

public:
	const UInventoryItemFragment* FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const;
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
