// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/EquipmentItemDefinition.h"

#include "Templates/SubclassOf.h"
#include "UObject/ObjectPtr.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentItemDefinition)

UEquipmentItemDefinition::UEquipmentItemDefinition(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

const UEquipmentItemFragment* UEquipmentItemDefinition::FindFragmentByClass(TSubclassOf<UEquipmentItemFragment> FragmentClass) const
{
	if (FragmentClass != nullptr)
	{
		for (UEquipmentItemFragment* Fragment : Fragments)
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


const UEquipmentItemFragment *UEquipmentFunctionLibrary::FindItemDefinitionFragment(UEquipmentItemDefinition *ItemDef, TSubclassOf<UEquipmentItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return ItemDef->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
