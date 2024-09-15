// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "CalculateDamageWithAttenuation.generated.h"

class UObject;


/**
 * TODO: Replace me with `ExecutionCalculationDamage`
 * 
 * GameplayEffectExecutionCalculation class used by `GameplayEffect` to apply healing to the `PawnHealthSet` health attributes.
 * Distance and Surface attenuation is supported.
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class THIRDPERSONSHOOTER_API UCalculateDamageWithAttenuation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UCalculateDamageWithAttenuation();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
