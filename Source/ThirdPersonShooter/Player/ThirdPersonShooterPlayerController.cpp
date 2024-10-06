#include "ThirdPersonShooterPlayerController.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameplayAbilities/ModularAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ThirdPersonShooterPlayerController)

AThirdPersonShooterPlayerController::AThirdPersonShooterPlayerController(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AThirdPersonShooterPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
}

void AThirdPersonShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void AThirdPersonShooterPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void AThirdPersonShooterPlayerController::AttachToPawn(APawn *InPawn)
{
	if (InPawn)
	{
		AbilitySystemComponent = InPawn->FindComponentByClass<UModularAbilitySystemComponent>();
	}

	Super::AttachToPawn(InPawn);
}

void AThirdPersonShooterPlayerController::DetachFromPawn()
{
	Super::DetachFromPawn();
}

void AThirdPersonShooterPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
