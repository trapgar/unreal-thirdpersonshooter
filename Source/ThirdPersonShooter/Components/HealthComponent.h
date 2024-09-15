// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Components/GameFrameworkComponent.h"
#include "GameplayAbilities/Attributes/PawnHealthSet.h"

#include "HealthComponent.generated.h"

class UHealthComponent;
class UModularAbilitySystemComponent;
class UPawnHealthSet;
class UObject;
struct FFrame;
struct FGameplayEffectSpec;

THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Message_Elimination);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FPawnHealth_DeathEvent, AActor*, OwningActor);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FPawnHealth_AttributeChanged, UHealthComponent*, HealthComponent, float, OldValue, float, NewValue, AActor*, Instigator);

/**
 * EPlayerDeathState
 *
 * Defines current state of death.
 */
UENUM(BlueprintType)
enum class EPlayerDeathState : uint8
{
	NotDead = 0,
	DeathStarted,
	DeathFinished
};


/**
 * UHealthComponent
 *
 * An actor component used to handle anything related to health.
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class THIRDPERSONSHOOTER_API UHealthComponent : public UGameFrameworkComponent
{
	GENERATED_BODY()

public:

	UHealthComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the health component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Pawn|Health")
	static UHealthComponent* FindHealthComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UHealthComponent>() : nullptr); }

	// Initialize the component using an ability system component.
	UFUNCTION(BlueprintCallable, Category = "Pawn|Health")
	void InitializeWithAbilitySystem(UAbilitySystemComponent* InASC);

	// Uninitialize the component, clearing any references to the ability system.
	UFUNCTION(BlueprintCallable, Category = "Pawn|Health")
	void UninitializeFromAbilitySystem();

	// Returns the current health value.
	UFUNCTION(BlueprintCallable, Category = "Pawn|Health")
	float GetHealth() const;

	// Returns the current maximum health value.
	UFUNCTION(BlueprintCallable, Category = "Pawn|Health")
	float GetMaxHealth() const;

	// Returns the current health in the range [0.0, 1.0].
	UFUNCTION(BlueprintCallable, Category = "Pawn|Health")
	float GetHealthNormalized() const;

	UFUNCTION(BlueprintCallable, Category = "Pawn|Health")
	EPlayerDeathState GetDeathState() const { return DeathState; }

	UFUNCTION(BlueprintCallable, BlueprintPure = false, Category = "Pawn|Health", Meta = (ExpandBoolAsExecs = "ReturnValue"))
	bool IsDeadOrDying() const { return (DeathState > EPlayerDeathState::NotDead); }

	// Begins the death sequence for the owner.
	virtual void StartDeath();

	// Ends the death sequence for the owner.
	virtual void FinishDeath();

	// Applies enough damage to kill the owner.
	virtual void DamageSelfDestruct(bool bFellOutOfWorld = false);

public:

	// Delegate fired when the health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FPawnHealth_AttributeChanged OnHealthChanged;

	// Delegate fired when the max health value has changed. This is called on the client but the instigator may not be valid
	UPROPERTY(BlueprintAssignable)
	FPawnHealth_AttributeChanged OnMaxHealthChanged;

	// Delegate fired when the death sequence has started.
	UPROPERTY(BlueprintAssignable)
	FPawnHealth_DeathEvent OnDeathStarted;

	// Delegate fired when the death sequence has finished.
	UPROPERTY(BlueprintAssignable)
	FPawnHealth_DeathEvent OnDeathFinished;

	virtual void InitializeComponent() override
	{
		if (IAbilitySystemInterface* WithAbilities = Cast<IAbilitySystemInterface>(GetOwner()))
		{
			if (UAbilitySystemComponent* ASC = WithAbilities->GetAbilitySystemComponent())
			{
				ASC->AddSet<UPawnHealthSet>();
			}
		}

		Super::InitializeComponent();
	};

protected:

	virtual void OnUnregister() override;

	void ClearGameplayTags();

	virtual void HandleHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleMaxHealthChanged(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);
	virtual void HandleOutOfHealth(AActor* DamageInstigator, AActor* DamageCauser, const FGameplayEffectSpec* DamageEffectSpec, float DamageMagnitude, float OldValue, float NewValue);

	UFUNCTION()
	virtual void OnRep_DeathState(EPlayerDeathState OldDeathState);

protected:

	// Ability system used by this component.
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// Health set used by this component.
	UPROPERTY()
	TObjectPtr<const UPawnHealthSet> HealthSet;

	// Replicated state used to handle dying.
	UPROPERTY(ReplicatedUsing = OnRep_DeathState)
	EPlayerDeathState DeathState;
};
