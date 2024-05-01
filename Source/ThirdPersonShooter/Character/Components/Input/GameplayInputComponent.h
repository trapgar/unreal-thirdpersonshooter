// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "EnhancedInputComponent.h"
#include "GameplayInputConfiguration.h"
#include "AbilitySystemInterface.h"
// #include "Character/Components/Ability/ModularAbilitySystemComponent.h"

#include "GameplayInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;


/**
 * UGameplayInputComponent
 *
 *	Component used to manage input mappings and bindings using an input config data asset.
 */
UCLASS(Config = Input)
class UGameplayInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UGameplayInputComponent(const FObjectInitializer& ObjectInitializer);

	void InitializePlayerInput(UInputComponent* PlayerInputComponent);

	void AddInputMappings(const UGameplayInputConfiguration* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;
	void RemoveInputMappings(const UGameplayInputConfiguration* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const;

	template<class UserClass, typename FuncType>
	void BindNativeAction(const UGameplayInputConfiguration* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound);

	template<class UserClass>
	void BindAbilityActions(const UGameplayInputConfiguration* InputConfig, UserClass* Object, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);

	// void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	// void Input_AbilityInputTagReleased(FGameplayTag InputTag);
	
	/** The name of the extension event sent via UGameFrameworkComponentManager when ability inputs are ready to bind */
	static const FName NAME_BindInputsNow;

	/** The name of this component-implemented feature */
	static const FName NAME_ActorFeatureName;
};


template<class UserClass, typename FuncType>
void UGameplayInputComponent::BindNativeAction(const UGameplayInputConfiguration* InputConfig, const FGameplayTag& InputTag, ETriggerEvent TriggerEvent, UserClass* Object, FuncType Func, bool bLogIfNotFound)
{
	check(InputConfig);
	if (const UInputAction* IA = InputConfig->FindNativeInputActionForTag(InputTag, bLogIfNotFound))
	{
		BindAction(IA, TriggerEvent, Object, Func);
	}
}

template<class UserClass>
void UGameplayInputComponent::BindAbilityActions(const UGameplayInputConfiguration* InputConfig, UserClass* Object, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FRuntimeInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			// BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, &ThisClass::Input_AbilityInputTagPressed, Action.InputTag).GetHandle());
			// BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, &ThisClass::Input_AbilityInputTagReleased, Action.InputTag).GetHandle());
		}
	}
}

// void UGameplayInputComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
// {
// 	if (const IAbilitySystemInterface* Pawn = GetOwner<IAbilitySystemInterface>())
// 	{
// 		if (UAbilitySystemComponent* ASC = Pawn->GetAbilitySystemComponent())
// 		{
// 			if (UModularAbilitySystemComponent* ModularASC = Cast<UModularAbilitySystemComponent>(ASC))
// 			{
// 				ModularASC->AbilityInputTagPressed(InputTag);
// 			}
// 		}
// 	}
// }

// void UGameplayInputComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
// {
// 	if (const IAbilitySystemInterface* Pawn = GetOwner<IAbilitySystemInterface>())
// 	{
// 		if (UAbilitySystemComponent* ASC = Pawn->GetAbilitySystemComponent())
// 		{
// 			if (UModularAbilitySystemComponent* ModularASC = Cast<UModularAbilitySystemComponent>(ASC))
// 			{
// 				ModularASC->AbilityInputTagReleased(InputTag);
// 			}
// 		}
// 	}
// }
