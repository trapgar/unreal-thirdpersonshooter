#pragma once

#include "ModularAbilityCost.h"
#include "GameplayTagContainer.h"
#include "ScalableFloat.h"

#include "GameplayEffectCost_ItemTagStack.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpecHandle;

class UModularGameplayAbility;
class UObject;
struct FGameplayAbilityActorInfo;

/**
 * Represents a cost that requires expending a quantity of a tag stack on the associated item instance
 */
UCLASS(Blueprintable, DefaultToInstanced, EditInlineNew, Const, meta=(DisplayName="Item Tag Stack"))
class THIRDPERSONSHOOTER_API UGameplayEffectCost_ItemTagStack : public UModularAbilityCost
{
	GENERATED_BODY()

public:
	UGameplayEffectCost_ItemTagStack();

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
