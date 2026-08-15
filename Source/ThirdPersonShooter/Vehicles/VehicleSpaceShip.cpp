#include "VehicleSpaceShip.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameplayAbilities/ModularAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThirdPersonShooterGameplayTags.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(VehicleSpaceShip)

class UEnhancedInputLocalPlayerSubsystem;

// Sets default values
AVehicleSpaceShip::AVehicleSpaceShip()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	VehicleMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("VehicleMovementComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UModularAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	GameplayInputComponent = CreateDefaultSubobject<UGameplayInputComponent>(TEXT("GameplayInputComponent"));
}

void AVehicleSpaceShip::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// Called when the game starts or when spawned
void AVehicleSpaceShip::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AVehicleSpaceShip::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AVehicleSpaceShip::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		MASC->HandleControllerChanged();
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag("GameplayEvent.Pawn.ControllerChanged");
		Payload.Instigator = this;
		Payload.Target = this;
		Payload.OptionalObject = NewController;
		MASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

void AVehicleSpaceShip::UnPossessed()
{
	Super::UnPossessed();

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		MASC->HandleControllerChanged();
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag("GameplayEvent.Pawn.ControllerChanged");
		Payload.Instigator = this;
		Payload.Target = this;
		MASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

// Called every frame
void AVehicleSpaceShip::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AVehicleSpaceShip::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	// TODO: Why are the gameplay tags all on the ASC instead of the Character?
	// the Character supports tags...
	if (auto ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AVehicleSpaceShip::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AVehicleSpaceShip::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AVehicleSpaceShip::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}
