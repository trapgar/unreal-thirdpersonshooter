// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayAbilities/Calculations/ExecutionCalculationHealing.h"
#include "GameplayAbilities/Attributes/PawnHealthSet.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExecutionCalculationHealing)


struct FHealingStatics
{
	FGameplayEffectAttributeCaptureDefinition BaseHealingAppliedDef;

	FHealingStatics()
	{
		BaseHealingAppliedDef = FGameplayEffectAttributeCaptureDefinition(UPawnCombatSet::GetBaseHealingAppliedAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FHealingStatics& HealingStatics()
{
	static FHealingStatics Statics;
	return Statics;
}


UExecutionCalculationHealing::UExecutionCalculationHealing()
{
	RelevantAttributesToCapture.Add(HealingStatics().BaseHealingAppliedDef);
}

void UExecutionCalculationHealing::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float HealingApplied = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(HealingStatics().BaseHealingAppliedDef, EvaluateParameters, HealingApplied);

	const float HealingDone = FMath::Max(0.0f, HealingApplied);

	if (HealingDone > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPawnHealthSet::GetHealingAttribute(), EGameplayModOp::Additive, HealingDone));
	}
#endif // #if WITH_SERVER_CODE
}

