#pragma once

#include "EquipmentItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "EquipmentFragment_AnimationLayers.generated.h"

class UEquipmentItemDefinition;
class UObject;

USTRUCT(BlueprintType)
struct FAnimationLayerRule
{
	GENERATED_BODY()

public:

	// Pawn must have all of these tags to apply the animation
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (Categories = "GameplayCue|Cosmetic"))
	FGameplayTagContainer RequiredTags;

	// Animation to play when the tags are met
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<UAnimInstance> Layer;
};


// --------------------------------------------------------

UCLASS(BlueprintType)
class UEquipmentFragment_AnimationLayers : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:

	// Animation Montage to play when item is equipped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	UAnimMontage* EquipMontage;

	// Animation Montage to play when item is unequipped
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	UAnimMontage* UnequipMontage;

	// Animation Montage to play when item is used to melee
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	UAnimMontage* MeleeMontage;

	// Animation Montage to play when item fails to activate (e.g. on cooldown, or not enough ammo)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	UAnimMontage* AbilityFailedMontage;

	// List of animation layers to apply to the pawn on equip
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	TArray<FAnimationLayerRule> LayerRules;

	// Animation to play when none of the rules match
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Animation")
	TSubclassOf<UAnimInstance> DefaultLayer;

	// Attempts to pick the best animation layer based on the applied tags
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Animation")
	TSubclassOf<UAnimInstance> PickBestAnimationLayer(const FGameplayTagContainer& AppliedTags) const;

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface
};
