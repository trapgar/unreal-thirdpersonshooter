// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayAbilities/Abilities/ModularAbilityCost.h"
#include "ScalableFloat.h"
#include "Templates/SubclassOf.h"

#include "GameplayEffectCost_InventoryItem.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UModularGameplayAbility;
class UInventoryItemDefinition;
class UObject;
struct FGameplayAbilityActorInfo;
struct FGameplayTagContainer;

/**
 * Represents a cost that requires expending a quantity of an inventory item
 */
UCLASS(meta=(DisplayName="Inventory Item"))
class UGameplayEffectCost_InventoryItem : public UModularAbilityCost
{
	GENERATED_BODY()

public:
	UGameplayEffectCost_InventoryItem();

	//~UModularAbilityCost interface
	virtual bool CheckCost(const UModularGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UModularGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UModularAbilityCost interface

protected:
	/** How much of the item to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	FScalableFloat Quantity;

	/** Which item to consume */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category=AbilityCost)
	TSubclassOf<UInventoryItemDefinition> ItemDefinition;
};
