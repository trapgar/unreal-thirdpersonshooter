// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryFragment_EquippableItem.h"
#include "Equipment/EquipmentItemDefinition.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_EquippableItem)

#define LOCTEXT_NAMESPACE "Equipment"

#if WITH_EDITOR
EDataValidationResult UInventoryFragment_EquippableItem::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (EquipmentDefinition == nullptr)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(LOCTEXT("EntryHasNullActor", "Null EquipmentDefinition"));
	}

	return Result;
}
#endif

#undef LOCTEXT_NAMESPACE
