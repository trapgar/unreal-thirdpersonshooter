// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInputComponent)

class UGameplayInputConfiguration;

UGameplayInputComponent::UGameplayInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGameplayInputComponent::AddAdditionalInputConfig(const UGameplayInputConfiguration *InputConfig)
{
	check(InputConfig);

	AActor* Owner = GetOwner();
	APawn* Pawn = Cast<APawn>(Owner);

	if (!Pawn)
	{
		UE_LOG(LogInput, Error, TEXT("Owner is NOT a Pawn - unable to bind input ([%s])."), *GetNameSafe(Owner));
		return;
	}

	const APlayerController* PC = Pawn->GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
	// be triggered directly by these input actions Triggered events. 
	TArray<uint32> BindHandles;
	BindAbilityActions(InputConfig, this, &UGameplayInputComponent::Input_AbilityInputTagPressed, &UGameplayInputComponent::Input_AbilityInputTagReleased, /*out*/ BindHandles);
}

void UGameplayInputComponent::RemoveAdditionalInputConfig(const UGameplayInputConfiguration* InputConfig)
{
	// TODO: Not even implemented in Lyra
}

void UGameplayInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}

	BindHandles.Reset();
}

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
void UGameplayInputComponent::BindAbilityActions(const UGameplayInputConfiguration* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles)
{
	check(InputConfig);

	for (const FRuntimeInputAction& Action : InputConfig->AbilityInputActions)
	{
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, PressedFunc, Action.InputTag).GetHandle());
			BindHandles.Add(BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag).GetHandle());
		}
	}
}

void UGameplayInputComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
	if (const IAbilitySystemInterface* Pawn = GetOwner<IAbilitySystemInterface>())
	{
		if (UAbilitySystemComponent* ASC = Pawn->GetAbilitySystemComponent())
		{
			// ASC->AbilityInputTagPressed(InputTag);
			if (InputTag.IsValid())
			{
				for (const FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
				{
					if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
					{
						// FGameplayAbilitySpec NonConstAbilitySpec = AbilitySpec;
						// ASC->AbilitySpecInputPressed(NonConstAbilitySpec);
						ASC->TryActivateAbility(AbilitySpec.Handle);
					}
				}
			}
		}
	}
}

void UGameplayInputComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
	if (const IAbilitySystemInterface* Pawn = GetOwner<IAbilitySystemInterface>())
	{
		if (UAbilitySystemComponent* ASC = Pawn->GetAbilitySystemComponent())
		{
			// ASC->AbilityInputTagReleased(InputTag);
			if (InputTag.IsValid())
			{
				for (const FGameplayAbilitySpec& AbilitySpec : ASC->GetActivatableAbilities())
				{
					if (AbilitySpec.Ability && (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)))
					{
						// FGameplayAbilitySpec NonConstAbilitySpec = AbilitySpec;
						// ASC->AbilitySpecInputReleased(NonConstAbilitySpec);
					}
				}
			}
		}
	}
}
