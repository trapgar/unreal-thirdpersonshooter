// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "ExecutionCalculationDamage.generated.h"

class UObject;


/**
 * GameplayEffectExecutionCalculation class used by `GameplayEffect` to apply deterioration to the `PawnHealthSet.Health` attribute.
 * 
 * Distance and Surface attenuation is supported.
 */
UCLASS()
class THIRDPERSONSHOOTER_API UExecutionCalculationDamage : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecutionCalculationDamage();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
