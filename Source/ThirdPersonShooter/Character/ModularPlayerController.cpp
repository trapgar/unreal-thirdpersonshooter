#include "ModularPlayerController.h"
#include "Ability/ModularAbilitySystemComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularPlayerController)

AModularPlayerController::AModularPlayerController(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AModularPlayerController::BeginPlay()
{
	Super::BeginPlay();
	SetActorHiddenInGame(false);
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
