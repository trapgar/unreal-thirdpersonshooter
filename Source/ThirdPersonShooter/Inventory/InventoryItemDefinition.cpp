// Copyright Epic Games, Inc. All Rights Reserved.

#include "Inventory/InventoryItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryItemDefinition)



// --------------------------------------------------------


UInventoryItemDefinition::UInventoryItemDefinition(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UInventoryItemFragment* UInventoryItemDefinition::FindFragmentByClass(TSubclassOf<UInventoryItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UInventoryItemFragment* Fragment : Fragments)
		{
			if (Fragment && Fragment->IsA(FragmentClass))
			{
				return Fragment;
			}
		}
	}

	return nullptr;
}


// --------------------------------------------------------


const UInventoryItemFragment *UInventoryFunctionLibrary::FindItemDefinitionFragment(UInventoryItemDefinition *ItemDef, TSubclassOf<UInventoryItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return ItemDef->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
