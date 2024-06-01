#include "Equipment/EquipmentItemDefinition.h"

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

// UEquipmentItemDefinition

const UEquipmentItemFragment* UEquipmentFunctionLibrary::FindItemDefinitionFragment(TSubclassOf<UEquipmentItemDefinition> ItemDef, TSubclassOf<UEquipmentItemFragment> FragmentClass)
{
	if ((ItemDef != nullptr) && (FragmentClass != nullptr))
	{
		return GetDefault<UEquipmentItemDefinition>(ItemDef)->FindFragmentByClass(FragmentClass);
	}
	return nullptr;
}
