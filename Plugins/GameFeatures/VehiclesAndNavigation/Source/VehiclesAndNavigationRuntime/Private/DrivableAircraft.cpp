#include "DrivableAircraft.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ThirdPersonShooter/Ability/ModularAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThirdPersonShooter/ThirdPersonShooterGameplayTags.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(DrivableAircraft)

class UEnhancedInputLocalPlayerSubsystem;

// Sets default values
ADrivableAircraft::ADrivableAircraft()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	VehicleMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("VehicleMovementComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UModularAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	GameplayInputComponent = CreateDefaultSubobject<UGameplayInputComponent>(TEXT("GameplayInputComponent"));
}

void ADrivableAircraft::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// Called when the game starts or when spawned
void ADrivableAircraft::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void ADrivableAircraft::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void ADrivableAircraft::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		MASC->HandleControllerChanged();
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag("GameplayEvent.Possessed");
		Payload.Instigator = this;
		Payload.Target = this;
		MASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

void ADrivableAircraft::UnPossessed()
{
	Super::UnPossessed();

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		MASC->HandleControllerChanged();
		FGameplayEventData Payload;
		Payload.EventTag = FGameplayTag::RequestGameplayTag("GameplayEvent.Unpossessed");
		Payload.Instigator = this;
		Payload.Target = this;
		MASC->HandleGameplayEvent(Payload.EventTag, &Payload);
	}
}

// Called every frame
void ADrivableAircraft::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADrivableAircraft::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	// TODO: Why are the gameplay tags all on the ASC instead of the Character?
	// the Character supports tags...
	if (auto ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADrivableAircraft::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ADrivableAircraft::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ADrivableAircraft::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}
