// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectExecutionCalculation.h"

#include "RuntimeAttenuatorExecutionCalculation.generated.h"

class UObject;


/**
 * URuntimeAttenuatorExecutionCalculation
 *
 *	Execution used by gameplay effects to apply damage to the health attributes.
 */
UCLASS(meta=(DisplayName="Pawn Damage Calculator With Runtime Attenuation"))
class URuntimeAttenuatorExecutionCalculation : public UGameplayEffectExecutionCalculation
{
	GENERATED_BODY()

public:

	URuntimeAttenuatorExecutionCalculation();

protected:

	virtual void Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const override;
};
