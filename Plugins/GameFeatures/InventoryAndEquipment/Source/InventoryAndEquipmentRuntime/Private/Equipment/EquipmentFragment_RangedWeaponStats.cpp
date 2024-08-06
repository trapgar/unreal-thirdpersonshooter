#include "Equipment/EquipmentFragment_RangedWeaponStats.h"

#include "Equipment/EquipmentItemInstance.h"
#include "Inventory/InventoryItemInstance.h"
#include "NativeGameplayTags.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_RangedWeaponStats)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Equipment_Weapon_MagazineSize, "Equipment.Weapon.MagazineSize");

#define LOCTEXT_NAMESPACE "Equipment"

void UEquipmentFragment_RangedWeaponStats::OnInstanceCreated(UEquipmentItemInstance* Instance) const
{
	if (UInventoryItemInstance* ItemInstance = Instance->GetAssociatedItem())
	{
		ItemInstance->AddStatTagStack(TAG_Equipment_Weapon_MagazineSize, MagazineSize);
	}
}

#if WITH_EDITOR
EDataValidationResult UEquipmentFragment_RangedWeaponStats::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	return Result;
}
#endif
