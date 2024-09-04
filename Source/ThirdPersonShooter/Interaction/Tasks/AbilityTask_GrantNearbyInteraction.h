// Copyright Epic Games, Inc. All Rights Reserved.
#pragma once

#include "Abilities/Tasks/AbilityTask.h"

#include "AbilityTask_GrantNearbyInteraction.generated.h"

class UGameplayAbility;
class UObject;
struct FFrame;
struct FGameplayAbilitySpecHandle;
struct FObjectKey;

/**
 * UAbilityTask_GrantNearbyInteraction
 * 
 * A Gameplay Task that periodically searches the surrounding world for nearby interactables.
 * Task is scheduled by the `GameplayAbility_Interact` GameplayAbility.
 */
UCLASS()
class UAbilityTask_GrantNearbyInteraction : public UAbilityTask
{
	GENERATED_UCLASS_BODY()

	virtual void Activate() override;

	/** Wait until an overlap occurs. This will need to be better fleshed out so we can specify game specific collision requirements */
	UFUNCTION(BlueprintCallable, Category="Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_GrantNearbyInteraction* GrantAbilitiesForNearbyInteractors(UGameplayAbility* OwningAbility, float InteractionScanRange, float InteractionScanRate);

private:

	void AvatarControllerChanged(const FGameplayEventData* Payload)
	{
		if (Payload->OptionalObject && Payload->OptionalObject.IsA<AController>())
		{
			AvatarPossessed(Payload);
		}
		else
		{
			AvatarUnpossessed(Payload);
		}
	};

	void AvatarPossessed(const FGameplayEventData* Payload);
	void AvatarUnpossessed(const FGameplayEventData* Payload);

	virtual void OnDestroy(bool AbilityEnded) override;

	void QueryInteractables();

	float InteractionScanRange = 100;
	float InteractionScanRate = 0.100;

	FTimerHandle QueryTimerHandle;
	FDelegateHandle Handle_AvatarControllerChanged;

	TMap<FObjectKey, FGameplayAbilitySpecHandle> InteractionAbilityCache;
};
