// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "InputActionToGameplayTagBindings.generated.h"

class UInputAction;
class UObject;
struct FFrame;

/**
 * FInputTagActionBinding
 *
 *	Struct used to map a input action to a gameplay input tag.
 */
USTRUCT(BlueprintType)
struct THIRDPERSONSHOOTER_API FInputTagActionBinding
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UInputAction> InputAction = nullptr;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

/**
 * UInputActionToGameplayTagBindings
 *
 *	Non-mutable data asset that contains input configuration properties.
 *	This is where we define the relationship between input tags and input actions that GameplayAbilities will use.
 */
UCLASS(BlueprintType, Const)
class THIRDPERSONSHOOTER_API UInputActionToGameplayTagBindings : public UDataAsset
{
	GENERATED_BODY()

public:

	UInputActionToGameplayTagBindings(const FObjectInitializer& ObjectInitializer);

	UFUNCTION(BlueprintCallable, Category = "Pawn|Input")
	const UInputAction* FindNativeInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

	UFUNCTION(BlueprintCallable, Category = "Pawn|Input")
	const UInputAction* FindAbilityInputActionForTag(const FGameplayTag& InputTag, bool bLogNotFound = true) const;

public:
	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and must be manually bound.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FInputTagActionBinding> NativeInputActions;

	// List of input actions used by the owner.  These input actions are mapped to a gameplay tag and are automatically bound to abilities with matching input tags.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (TitleProperty = "InputAction"))
	TArray<FInputTagActionBinding> AbilityInputActions;
};
