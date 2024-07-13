// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/GameplayAbility_FromEquipment.h"
#include "Equipment/EquipmentItemInstance.h"
#include "Inventory/InventoryItemInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbility_FromEquipment)

UGameplayAbility_FromEquipment::UGameplayAbility_FromEquipment(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

AEquipmentItemInstance* UGameplayAbility_FromEquipment::GetAssociatedEquipment() const
{
	if (FGameplayAbilitySpec* Spec = UGameplayAbility::GetCurrentAbilitySpec())
	{
		return Cast<AEquipmentItemInstance>(Spec->SourceObject.Get());
	}

	return nullptr;
}

UInventoryItemInstance* UGameplayAbility_FromEquipment::GetAssociatedItem() const
{
	if (AEquipmentItemInstance* Equipment = GetAssociatedEquipment())
	{
		return Cast<UInventoryItemInstance>(Equipment->GetSource());
	}

	return nullptr;
}


#if WITH_EDITOR
EDataValidationResult UGameplayAbility_FromEquipment::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = Super::IsDataValid(Context);

	if (InstancingPolicy == EGameplayAbilityInstancingPolicy::NonInstanced)
	{
		Context.AddError(NSLOCTEXT("Abilities", "EquipmentAbilityMustBeInstanced", "Equipment ability must be instanced"));
		Result = EDataValidationResult::Invalid;
	}

	return Result;
}

#endif
