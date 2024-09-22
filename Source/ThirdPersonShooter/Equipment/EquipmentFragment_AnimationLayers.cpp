#include "Equipment/EquipmentFragment_AnimationLayers.h"
#include "Animation/AnimInstance.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_AnimationLayers)

#define LOCTEXT_NAMESPACE "Equipment"

#if WITH_EDITOR
EDataValidationResult UEquipmentFragment_AnimationLayers::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FAnimationLayerRule& Entry : LayerRules)
	{
		if (Entry.RequiredTags.IsEmpty())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("EntryIsEmpty", "Empty RequiredTags at index {0} in LayerRules"), FText::AsNumber(EntryIndex)));
		}
		if (!Entry.Layer)
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("EntryHasNullActor", "Null Layer at index {0} in LayerRules"), FText::AsNumber(EntryIndex)));
		}
		++EntryIndex;
	}

	return Result;
}
#endif

TSubclassOf<UAnimInstance> UEquipmentFragment_AnimationLayers::PickBestAnimationLayer(const FGameplayTagContainer& AppliedTags) const
{
	for (const FAnimationLayerRule& Rule : LayerRules)
	{
		if ((Rule.Layer != nullptr) && AppliedTags.HasAll(Rule.RequiredTags))
		{
			return Rule.Layer;
		}
	}

	return DefaultLayer;
}

#undef LOCTEXT_NAMESPACE
