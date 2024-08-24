// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "CalculateHealing.generated.h"

class UObject;


/**
 * UCalculateHealing
 *
 *	Execution used by `GameplayEffects` to apply healing to the `UPawnHealthSet` attribute.
 */
UCLASS()
class INVENTORYANDEQUIPMENTRUNTIME_API UCalculateHealing : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UCalculateHealing();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
