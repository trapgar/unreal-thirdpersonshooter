#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayCueInterface.h"
#include "GameplayTagAssetInterface.h"
#include "GameFramework/Pawn.h"
#include "Abilities/GameplayAbility.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "ThirdPersonShooter/Character/Components/Input/GameplayInputComponent.h"

#include "DrivableSpaceVehicle.generated.h"

UCLASS(Blueprintable, BlueprintType)
class VEHICLESANDNAVIGATIONRUNTIME_API ADrivableSpaceVehicle : public APawn, public IAbilitySystemInterface, public IGameplayCueInterface, public IGameplayTagAssetInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	ADrivableSpaceVehicle();

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//~IAbilitySystemInterface interface
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Abilities")
	UAbilitySystemComponent* AbilitySystemComponent;

	// what is this used for?
	UFUNCTION(BlueprintCallable, Category = "Vehicle|Abilities")
	void InitializeAbility(TSubclassOf<UGameplayAbility> AbilityToGet, int32 AbilityLevel = 1);

	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override
	{
		return AbilitySystemComponent;
	}
	//~End of IAbilitySystemInterface interface

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Vehicle|Input")
	UGameplayInputComponent* GameplayInputComponent;

	virtual void GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const override;
	virtual bool HasMatchingGameplayTag(FGameplayTag TagToCheck) const override;
	virtual bool HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;
	virtual bool HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const override;

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Default", meta=(DisplayName="Vehicle Movement Component"))
	TObjectPtr<UFloatingPawnMovement> VehicleMovementComponent;

protected:
	virtual void PreInitializeComponents() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void PossessedBy(AController* NewController) override;
	virtual void UnPossessed() override;

	// Parts that move on gimbals (thruster roots, etc) - set on ctor
	UPROPERTY(BlueprintReadWrite, Category="Thrusters")
	TArray<USceneComponent *> Gimbals;

};
