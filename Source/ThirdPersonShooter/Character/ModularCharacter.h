#pragma once

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Character.h"
#include "Abilities/GameplayAbility.h"
// #include "Components/Ability/ModularAbilitySystemComponent.h"
#include "Components/Input/GameplayInputComponent.h"
#include "Components/Inventory/InventoryManagerComponent.h"

#include "ModularCharacter.generated.h"

// class AActor;
class AController;
// class FLifetimeProperty;
// class IRepChangedPropertyTracker;
class UAbilitySystemComponent;
class UInputComponent;
// class UObject;
// struct FFrame;
struct FGameplayTag;
struct FGameplayTagContainer;

UCLASS(Config = Game, Meta = (ShortTooltip = "The base character pawn class used by this project."))
class THIRDPERSONSHOOTER_API AModularCharacter : public ACharacter, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AModularCharacter();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//~IAbilitySystemInterface interface
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Character|Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	UFUNCTION(BlueprintCallable, Category = "Character|Abilities")
	void InitializeAbility(TSubclassOf<UGameplayAbility> AbilityToGet, int32 AbilityLevel = 1);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~End of IAbilitySystemInterface interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Input")
	UGameplayInputComponent* GameplayInputComponent;

	// UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character|Inventory")
	// UInventoryManagerComponent* InventoryManagerComponent;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnAbilitySystemInitialized();
	virtual void OnAbilitySystemUninitialized();

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	virtual void OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;
	virtual void OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust) override;

// private:
// 	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Character", Meta = (AllowPrivateAccess = "true"))
// 	TObjectPtr<UGameFrameworkComponent> HealthComponent;

};
