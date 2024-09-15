// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayAbilities/Calculations/CalculateHealing.h"
#include "GameplayAbilities/Attributes/PawnHealthSet.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CalculateHealing)


struct FHealingStatics
{
	FGameplayEffectAttributeCaptureDefinition HealDef;

	FHealingStatics()
	{
		HealDef = FGameplayEffectAttributeCaptureDefinition(UPawnCombatSet::GetHealAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealingStatics& HealingStatics()
{
	static FHealingStatics Statics;
	return Statics;
}


UCalculateHealing::UCalculateHealing()
{
	RelevantAttributesToCapture.Add(HealingStatics().HealDef);
}

void UCalculateHealing::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseHeal = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealingStatics().HealDef, EvaluateParameters, BaseHeal);

	const float HealingDone = FMath::Max(0.0f, BaseHeal);

	if (HealingDone > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPawnHealthSet::GetHealingAttribute(), EGameplayModOp::Additive, HealingDone));
	}
#endif // #if WITH_SERVER_CODE
}

