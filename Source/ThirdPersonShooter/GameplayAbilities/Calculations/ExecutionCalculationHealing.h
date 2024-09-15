// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "ExecutionCalculationHealing.generated.h"

class UObject;


/**
 * GameplayEffectExecutionCalculation class used by `GameplayEffect` to apply recovery to the `PawnHealthSet.Health` attribute.
 * 
 * Distance and Surface attenuation is supported.
 */
UCLASS()
class THIRDPERSONSHOOTER_API UExecutionCalculationHealing : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecutionCalculationHealing();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
