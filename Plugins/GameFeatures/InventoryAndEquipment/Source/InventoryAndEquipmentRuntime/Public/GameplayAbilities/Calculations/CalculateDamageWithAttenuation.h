// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "CalculateDamageWithAttenuation.generated.h"

class UObject;


/**
 * UCalculateDamageWithAttenuation
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS()
class INVENTORYANDEQUIPMENTRUNTIME_API UCalculateDamageWithAttenuation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	UCalculateDamageWithAttenuation();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
