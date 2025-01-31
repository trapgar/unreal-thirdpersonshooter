// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "InventoryItemDefinition.h"
#include "InventoryManagerComponent.h"
#include "AbilitySystemGlobals.h"
#include "NativeGameplayTags.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"
#include "Kismet/GameplayStatics.h"
#include "ThirdPersonShooterGameplayTags.h"

#include "InventoryItemDefinition_Ammunition.generated.h"

THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Inventory_Item_Classification);
THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Inventory_Item_Classification_Ammunition);

/**
 * Definition type of an inventory item
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Const, Abstract)
class UInventoryItemDefinition_Ammunition : public UInventoryItemDefinition
{
	GENERATED_BODY()

public:

	UInventoryItemDefinition_Ammunition(const FObjectInitializer& ObjectInitializer)
		: Super(ObjectInitializer)
	{
		StaticGameplayTags.AddTag(TAG_Inventory_Item_Classification_Ammunition);
	}

	// Gameplay tag of the weapon this ammo is for
	UPROPERTY(EditDefaultsOnly, Category = Inventory, meta = (Categories = "Item.Classification"))
	FGameplayTag WeaponClassification;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	TSubclassOf<UGameplayEffect> AmmunitionRefillOrSpendEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Inventory)
	int32 StackCount = 1;
};


// --------------------------------------------------------


// @TODO: Make into a subsystem instead?
UCLASS()
class UWeaponAmmunitionFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Ammunition")
	static const int32 GetAmmunitionStackCount(UInventoryItemInstance* ItemInstance)
	{
		if (const AActor* Instigator = ItemInstance->GetInstigator())
		{
			if (const UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Instigator))
			{
				return ASC->GetNumericAttribute(UPawnCombatSet::GetAttributeFromContainer(ItemInstance->GetItemDef()->StaticGameplayTags));
			}
		}

		return 0;
	}

};
