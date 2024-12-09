// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayAbilities/Calculations/ExecutionCalculationAmmunition.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ExecutionCalculationAmmunition)

struct FAmmunitionStatics
{
	FGameplayEffectAttributeCaptureDefinition AmmunitionRefillDef;
	FGameplayEffectAttributeCaptureDefinition AmmunitionSpendDef;

	FAmmunitionStatics()
	{
		AmmunitionRefillDef = FGameplayEffectAttributeCaptureDefinition(UPawnCombatSet::GetAmmunitionRefillAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
		AmmunitionSpendDef = FGameplayEffectAttributeCaptureDefinition(UPawnCombatSet::GetAmmunitionSpendAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FAmmunitionStatics& AmmunitionStatics()
{
	static FAmmunitionStatics Statics;
	return Statics;
}


UExecutionCalculationAmmoRefill::UExecutionCalculationAmmoRefill()
{
	RelevantAttributesToCapture.Add(AmmunitionStatics().AmmunitionRefillDef);
}

void UExecutionCalculationAmmoRefill::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseRefilled = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AmmunitionStatics().AmmunitionRefillDef, EvaluateParameters, BaseRefilled);

	const float AmmoRefilled = FMath::Max(0.0f, BaseRefilled);

	if (AmmoRefilled > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPawnCombatSet::GetAmmunitionRefillAttribute(), EGameplayModOp::Additive, AmmoRefilled));
	}
#endif // #if WITH_SERVER_CODE
}


UExecutionCalculationAmmoSpend::UExecutionCalculationAmmoSpend()
{
	RelevantAttributesToCapture.Add(AmmunitionStatics().AmmunitionSpendDef);
}

void UExecutionCalculationAmmoSpend::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseSpent = 0.0f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(AmmunitionStatics().AmmunitionSpendDef, EvaluateParameters, BaseSpent);

	const float AmmoSpent = FMath::Max(0.0f, BaseSpent);

	if (AmmoSpent > 0.0f)
	{
		// Apply a healing modifier, this gets turned into + health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPawnCombatSet::GetAmmunitionSpendAttribute(), EGameplayModOp::Additive, AmmoSpent));
	}
#endif // #if WITH_SERVER_CODE
}
