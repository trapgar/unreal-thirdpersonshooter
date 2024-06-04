// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Ability/Abilities/ModularGameplayAbility.h"
#include "Interaction/InteractionOption.h"

#include "GameplayAbility_Interact.generated.h"

class UIndicatorDescriptor;
class UObject;
class UUserWidget;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEventData;

/**
 * UGameplayAbility_Interact
 *
 * Gameplay ability used by characters for interacting with `AWorldInteractables`.
 * 
 * Should be granted to a player with the ActivationPolicy set to `OnSpawn`.
 */
UCLASS(Abstract)
class UGameplayAbility_Interact : public UModularGameplayAbility
{
	GENERATED_BODY()

public:

	UGameplayAbility_Interact(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	UFUNCTION(BlueprintCallable)
	void UpdateInteractions(const TArray<FInteractionOption>& InteractiveOptions);

	UFUNCTION(BlueprintCallable)
	void TriggerInteraction();

protected:
	UPROPERTY(BlueprintReadWrite)
	TArray<FInteractionOption> CurrentOptions;

	// UPROPERTY()
	// TArray<TObjectPtr<UIndicatorDescriptor>> Indicators;

protected:

	// Tick rate for activating the ability
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ForceUnits="s"))
	float InteractionScanRate = 0.1f;

	// Max distance from player to look for interactables
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(ForceUnits="cm"))
	float InteractionScanRange = 500;

	// Class of widget to show when presented with an interactable
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSoftClassPtr<UUserWidget> DefaultInteractionWidgetClass;
};
