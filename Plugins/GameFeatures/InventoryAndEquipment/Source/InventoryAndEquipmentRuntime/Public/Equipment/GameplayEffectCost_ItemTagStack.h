#pragma once

#include "Ability/Abilities/ModularAbilityCost.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"

#include "GameplayEffectCost_ItemTagStack.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UModularGameplayAbility;
class UObject;
struct FGameplayAbilityActorInfo;

/**
 * UGameplayEffectCost_ItemTagStack
 * 
 * Represents a cost that requires expending a quantity of a tag stack on the associated item instance
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Const, meta=(DisplayName="Item Tag Stack"))
class INVENTORYANDEQUIPMENTRUNTIME_API UGameplayEffectCost_ItemTagStack : public UModularAbilityCost
{
	GENERATED_BODY()

public:
	UGameplayEffectCost_ItemTagStack();

	//~UModularAbilityCost interface
	virtual bool CheckCost(const UModularGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const override;
	virtual void ApplyCost(const UModularGameplayAbility* Ability, const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) override;
	//~End of UModularAbilityCost interface

protected:
	/** How much of the tag to spend (keyed on ability level) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FScalableFloat Quantity;

	/** Which tag to spend some of */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag Tag;

	/** Which tag to send back as a response if this cost cannot be applied */
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FGameplayTag FailureTag;
};
