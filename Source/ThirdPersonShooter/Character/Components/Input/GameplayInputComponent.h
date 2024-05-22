#pragma once

#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "GameplayInputConfiguration.h"
#include "AbilitySystemInterface.h"

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

	void AddAdditionalInputConfig(const UGameplayInputConfiguration* InputConfig);
	void RemoveAdditionalInputConfig(const UGameplayInputConfiguration* InputConfig);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UGameplayInputConfiguration* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
};
