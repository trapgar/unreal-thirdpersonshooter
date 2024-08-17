// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "AbilitySystemGlobals.h"

#include "ModularAbilitySystemGlobals.generated.h"

class UObject;
struct FGameplayEffectContext;

/**
 * UModularAbilitySystemGlobals
 * 
 * Global library functions for the GameplayAbility System.
 */
UCLASS(Config=Game)
class UModularAbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_UCLASS_BODY()

	//~UAbilitySystemGlobals interface
	// Override the default context allocator so all effects use the `FModularGameplayEffectContext` instead, which supports attenuation.
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
	//~End of UAbilitySystemGlobals interface
};
