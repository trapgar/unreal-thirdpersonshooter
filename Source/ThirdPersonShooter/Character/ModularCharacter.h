#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
#include "Input/GameplayInputComponent.h"

#include "ModularCharacter.generated.h"

class AController;
class UAbilitySystemComponent;
class UInputComponent;
class UPawnHealthComponent;
struct FGameplayTag;
struct FGameplayTagContainer;

/**
 * AModularCharacter
 * 
 * Base class for characters that make use of the Modular ASC, etc.
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class THIRDPERSONSHOOTER_API AModularCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AModularCharacter(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Called every frame
	virtual void Tick(float DeltaTime) override;

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

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

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
	
	// Health attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UPawnHealthSet> HealthSet;

	// Combat attribute set used by this actor.
	UPROPERTY()
	TObjectPtr<const class UPawnCombatSet> CombatSet;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Health", Meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UPawnHealthComponent> HealthComponent;

};
