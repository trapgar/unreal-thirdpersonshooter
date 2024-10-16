#pragma once

#include "GameplayAbilities/ModularAbilitySystemComponent.h"
#include "AbilitySystemComponent.h"
#include "CommonPlayerController.h"

#include "ThirdPersonShooterPlayerController.generated.h"

class APawn;
class APlayerState;
class FPrimitiveComponentId;
class IInputInterface;
class UModularAbilitySystemComponent;
class UObject;
class UPlayer;
struct FFrame;

/**
 * AThirdPersonShooterPlayerController
 *
 * The base player controller class used by this project.
 * 
 * <i>Overridden so we can tell the ASC to process input on tick (`PostProcessInput`), so it can handle 'while input active' if need be.</i>
 */
UCLASS(Config = Game, Meta = (ShortTooltip = "The base player controller class used by this project."))
class THIRDPERSONSHOOTER_API AThirdPersonShooterPlayerController : public ACommonPlayerController
{
	GENERATED_BODY()

public:

	AThirdPersonShooterPlayerController(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~AActor interface
	virtual void BeginPlay() override;
	//~End of AActor interface

	//~APlayerController interface
	virtual void AttachToPawn(APawn* InPawn) override;
	virtual void DetachFromPawn() override;
	virtual void PostProcessInput(const float DeltaTime, const bool bGamePaused) override;
	//~End of APlayerController interface

protected:

	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:

	UModularAbilitySystemComponent* AbilitySystemComponent;

};
