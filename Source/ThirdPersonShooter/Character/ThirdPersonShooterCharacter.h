#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "NativeGameplayTags.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "ThirdPersonShooterGameplayTags.h"
#include "Input/GameplayInputComponent.h"
#include "Abilities/GameplayAbility.h"
#include "ModularCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

#include "ThirdPersonShooterCharacter.generated.h"

class AController;
class UAbilitySystemComponent;
class UInputComponent;
class UHealthComponent;
class UCustomCameraComponent;
struct FGameplayTag;
struct FGameplayTagContainer;

THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_GameplayEvent_Pawn_ControllerChanged);

/**
 * AThirdPersonShooterCharacter
 * 
 * Base class for characters that make use of the Modular ASC, etc.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class THIRDPERSONSHOOTER_API AThirdPersonShooterCharacter : public AModularCharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AThirdPersonShooterCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~IAbilitySystemInterface interface
	// what is this used for?
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override { return AbilitySystemComponent; }
	template<class T>
	T* GetAbilitySystemComponent() const { return Cast<T>(AbilitySystemComponent); }
	//~End of IAbilitySystemInterface interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Input")
	UGameplayInputComponent* GameplayInputComponent;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode) override
	{
		Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

		// Remove old & add new movement mode tags
		SetMovementModeTag(PrevMovementMode, PreviousCustomMode, /*bTagEnabled=*/false);
		SetMovementModeTag(GetCharacterMovement()->MovementMode, GetCharacterMovement()->CustomMovementMode, /*bTagEnabled=*/true);
	}
	void SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled);

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override
	{
		if (auto ASC = GetAbilitySystemComponent())
		{
			ASC->SetLooseGameplayTagCount(ThirdPersonShooterGameplayTags::Movement_Mode_Crouching, 1);
		}

		Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	}
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override
	{
		if (auto ASC = GetAbilitySystemComponent())
		{
			ASC->SetLooseGameplayTagCount(ThirdPersonShooterGameplayTags::Movement_Mode_Crouching, 0);
		}

		Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
	}

	// Begins the death sequence for the character (disables collision, disables movement, etc...)
	UFUNCTION()
	virtual void OnDeathStarted(AActor* OwningActor);

	// Ends the death sequence for the character (detaches controller, destroys pawn, etc...)
	UFUNCTION()
	virtual void OnDeathFinished(AActor* OwningActor);

	// Called when the death sequence for the character has completed
	UFUNCTION(BlueprintImplementableEvent, meta=(DisplayName="OnDeathFinished"))
	void K2_OnDeathFinished();

private:
	void DestroyDueToDeath();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Health", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

};
