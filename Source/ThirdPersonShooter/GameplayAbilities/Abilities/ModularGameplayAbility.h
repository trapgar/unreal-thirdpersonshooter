// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Abilities/GameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilities/ModularAbilityAttenuatorInterface.h"

#include "ModularGameplayAbility.generated.h"

struct FGameplayAbilityActivationInfo;
struct FGameplayAbilitySpec;
struct FGameplayAbilitySpecHandle;

class AActor;
class AController;
class APlayerController;
class FText;
class IModularAbilityAttenuatorInterface;
class UAnimMontage;
class UModularAbilityCost;
class UAbilitySystemComponent;
class UObject;
struct FFrame;
struct FGameplayAbilityActorInfo;
struct FGameplayEffectSpec;
struct FGameplayEventData;

/**
 * EModularAbilityActivationPolicy
 *
 * Defines how an ability is meant to activate.
 */
UENUM(BlueprintType)
enum class EModularAbilityActivationPolicy : uint8
{
	// Try to activate the ability when the input is triggered.
	OnInputTriggered,

	// Continually try to activate the ability while the input is active.
	// Note that the `InputAction` must NOT have a trigger, or the GameplayAbility will finish immediately after activating.
	WhileInputActive,

	// Try to activate the ability when an avatar is assigned.
	OnSpawn
};


/**
 * EModularAbilityActivationGroup
 *
 * Defines how an ability activates in relation to other abilities.
 */
UENUM(BlueprintType)
enum class EModularAbilityActivationGroup : uint8
{
	// Ability runs independently of all other abilities.
	Independent,

	// Ability is canceled and replaced by other exclusive abilities.
	Exclusive_Replaceable,

	// Ability blocks all other exclusive abilities from activating.
	Exclusive_Blocking,

	MAX	UMETA(Hidden)
};

/**
 * FModularAbilityFailureMessage
 * 
 * Message sent thru the GameplayMessageSubsystem when an ability fails to activate
 */
USTRUCT(BlueprintType)
struct THIRDPERSONSHOOTER_API FModularAbilityFailureMessage
{
	GENERATED_BODY()

public:
	
	// Owning controller of the ability
	UPROPERTY(BlueprintReadWrite)
	TObjectPtr<APlayerController> PlayerController = nullptr;

	// All the reasons why this ability has failed
	UPROPERTY(BlueprintReadWrite)
	FGameplayTagContainer FailureTags;
};

/**
 * UModularGameplayAbility
 *
 * The base gameplay ability class used by this project.
 * 
 * Using a custom override of the `UGameplayAbility` class because the default implementation doesn't support `EModularAbilityActivationPolicy::WhileInputActive` ActivationPolicy.
 * Also added an `AdditionalCosts` prop that is more flexible than the default `Costs`.
 * 
 * The default `Triggers` category is still used when the trigger is a GameplayEvent (triggered programmatically).
 */
UCLASS(Abstract, HideCategories = Input, Meta = (ShortTooltip = "The base gameplay ability class used by this project."))
class THIRDPERSONSHOOTER_API UModularGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:

	UModularGameplayAbility(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	void OnAbilityFailedToActivate(const FGameplayTagContainer& FailedReason) const
	{
		OnAbilityFailedToActivate_Broadcast(FailedReason);
		K2_OnFailedToActivate(FailedReason);
	}

	EModularAbilityActivationPolicy GetActivationPolicy() const { return ActivationPolicy; }
	EModularAbilityActivationGroup GetActivationGroup() const { return ActivationGroup; }

	UFUNCTION(BlueprintCallable, Category = "Ability|Controller")
	AController* GetControllerFromActorInfo() const { return (CurrentActorInfo ? CurrentActorInfo->PlayerController.Get() : nullptr); }

	UFUNCTION(BlueprintCallable, Category = "Ability|Pawn")
	APawn* GetPawnFromActorInfo() const { return (CurrentActorInfo ? Cast<APawn>(CurrentActorInfo->OwnerActor.Get()) : nullptr); }

	void TryActivateAbilityOnSpawn(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) const;

protected:

	// Broadcast to the GameplayMessageSubsystem when the ability fails to activate
	virtual void OnAbilityFailedToActivate_Broadcast(const FGameplayTagContainer& FailedReason) const;

	//~UGameplayAbility interface
	// virtual bool CanActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayTagContainer* SourceTags, const FGameplayTagContainer* TargetTags, FGameplayTagContainer* OptionalRelevantTags) const override;
	// virtual void SetCanBeCanceled(bool bCanBeCanceled) override;
	virtual void OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	virtual void OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec) override;
	// virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	// virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	virtual FGameplayEffectContextHandle MakeEffectContext(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo) const override;
	// TODO: Isn't used - can remove
	virtual void ApplyAbilityTagsToGameplayEffectSpec(FGameplayEffectSpec& Spec, FGameplayAbilitySpec* AbilitySpec) const override;
	virtual bool DoesAbilitySatisfyTagRequirements(const UAbilitySystemComponent& AbilitySystemComponent, const FGameplayTagContainer* SourceTags = nullptr, const FGameplayTagContainer* TargetTags = nullptr, OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	//~End of UGameplayAbility interface

protected:

	virtual void GetAbilitySourceAsAttenuator(FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		float& OutSourceLevel, const IModularAbilityAttenuatorInterface*& OutAbilitySource,
		AActor*& OutEffectCauser) const
	{
		OutSourceLevel = 0.0f;
		OutAbilitySource = nullptr;
		OutEffectCauser = nullptr;

		OutEffectCauser = ActorInfo->AvatarActor.Get();

		// If we were added by something that's an ability info source, use it
		UObject* SourceObject = GetSourceObject(Handle, ActorInfo);

		OutAbilitySource = Cast<IModularAbilityAttenuatorInterface>(SourceObject);
	};

protected:

	// Defines how this ability is meant to activate.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EModularAbilityActivationPolicy ActivationPolicy;

	// Defines the relationship between this ability activating and other abilities activating.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Ability Activation")
	EModularAbilityActivationGroup ActivationGroup;

	// Additional costs that must be paid to activate this ability
	UPROPERTY(EditDefaultsOnly, Instanced, Category = Costs, meta=(DisplayName="Additional Costs"))
	TArray<TObjectPtr<UModularAbilityCost>> AdditionalCosts;

	// Called when the ability fails to activate
	UFUNCTION(BlueprintImplementableEvent, Category=Ability, meta=(DisplayName="OnFailedToActivate"))
	void K2_OnFailedToActivate(const FGameplayTagContainer& FailedReason) const;

	// Called when the ability was added
	UFUNCTION(BlueprintImplementableEvent, Category=Ability, meta=(DisplayName="OnAbilityAdded"))
	void K2_OnAbilityAdded();

	// Called when the ability was removed
	UFUNCTION(BlueprintImplementableEvent, Category=Ability, meta=(DisplayName="OnAbilityRemoved"))
	void K2_OnAbilityRemoved();
};
