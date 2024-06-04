// Copyright Epic Games, Inc. All Rights Reserved.

#include "InputActionToGameplayTagBindings.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InputActionToGameplayTagBindings)


UInputActionToGameplayTagBindings::UInputActionToGameplayTagBindings(const FObjectInitializer& ObjectInitializer)
{
}

const UInputAction* UInputActionToGameplayTagBindings::FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FInputTagActionBinding& Action : NativeInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogInput, Error, TEXT("Can't find NativeInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}

const UInputAction* UInputActionToGameplayTagBindings::FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound) const
{
	for (const FInputTagActionBinding& Action : AbilityInputActions)
	{
		if (Action.InputAction && (Action.InputTag == InputTag))
		{
			return Action.InputAction;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogInput, Error, TEXT("Can't find AbilityInputAction for InputTag [%s] on InputConfig [%s]."), *InputTag.ToString(), *GetNameSafe(this));
	}

	return nullptr;
}
