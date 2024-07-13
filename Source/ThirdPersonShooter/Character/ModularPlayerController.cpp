#include "ModularPlayerController.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Ability/ModularAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularPlayerController)

AModularPlayerController::AModularPlayerController(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AModularPlayerController::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AModularPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
}

void AModularPlayerController::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AModularPlayerController::AttachToPawn(APawn *InPawn)
{
	if (InPawn)
	{
		AbilitySystemComponent = InPawn->FindComponentByClass<UModularAbilitySystemComponent>();
	}

	Super::AttachToPawn(InPawn);
}

void AModularPlayerController::DetachFromPawn()
{
	Super::DetachFromPawn();
}

void AModularPlayerController::PostProcessInput(const float DeltaTime, const bool bGamePaused)
{
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->ProcessAbilityInput(DeltaTime, bGamePaused);
	}

	Super::PostProcessInput(DeltaTime, bGamePaused);
}
