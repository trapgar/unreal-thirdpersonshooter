// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "ExecutionCalculationAmmunition.generated.h"

class UObject;


/**
 * GameplayEffectExecutionCalculation class used by `GameplayEffect` to apply recovery to the `PawnHealthSet.Health` attribute.
 * 
 * Distance and Surface attenuation is supported.
 */
UCLASS()
class THIRDPERSONSHOOTER_API UExecutionCalculationAmmoRefill : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecutionCalculationAmmoRefill();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};


/**
 * GameplayEffectExecutionCalculation class used by `GameplayEffect` to apply recovery to the `PawnHealthSet.Health` attribute.
 * 
 * Distance and Surface attenuation is supported.
 */
UCLASS()
class THIRDPERSONSHOOTER_API UExecutionCalculationAmmoSpend : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UExecutionCalculationAmmoSpend();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
