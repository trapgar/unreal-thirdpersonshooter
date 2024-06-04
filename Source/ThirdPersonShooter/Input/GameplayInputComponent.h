#pragma once

#include "EnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "InputActionToGameplayTagBindings.h"
#include "AbilitySystemInterface.h"

#include "GameplayInputComponent.generated.h"

class UEnhancedInputLocalPlayerSubsystem;
class UInputAction;
class UObject;


/**
 * UGameplayInputComponent
 *
 * UEnhancedInputComponent used to manage input mappings and bindings using by the `InputActionToGameplayTagBindings` data assets.
 */
UCLASS(Config = Input)
class THIRDPERSONSHOOTER_API UGameplayInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	UGameplayInputComponent(const FObjectInitializer& ObjectInitializer);

	void AddAdditionalInputBindings(const UInputActionToGameplayTagBindings* InputConfig);
	void RemoveAdditionalInputBindings(const UInputActionToGameplayTagBindings* InputConfig);

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType>
	void BindAbilityActions(const UInputActionToGameplayTagBindings* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, TArray<uint32>& BindHandles);

	void RemoveBinds(TArray<uint32>& BindHandles);

	void Input_AbilityInputTagPressed(FGameplayTag InputTag);
	void Input_AbilityInputTagReleased(FGameplayTag InputTag);
};
