#include "DrivableSpaceVehicle.h"
#include "Components/GameFrameworkComponentManager.h"
#include "ThirdPersonShooter/Ability/ModularAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThirdPersonShooter/ThirdPersonShooterGameplayTags.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(DrivableSpaceVehicle)

class UEnhancedInputLocalPlayerSubsystem;

// Sets default values
ADrivableSpaceVehicle::ADrivableSpaceVehicle()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("CenterOfMass"));

	VehicleMovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("VehicleMovementComponent"));
	AbilitySystemComponent = CreateDefaultSubobject<UModularAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	GameplayInputComponent = CreateDefaultSubobject<UGameplayInputComponent>(TEXT("GameplayInputComponent"));
}

void ADrivableSpaceVehicle::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// Called when the game starts or when spawned
void ADrivableSpaceVehicle::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void ADrivableSpaceVehicle::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void ADrivableSpaceVehicle::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);
}

void ADrivableSpaceVehicle::UnPossessed()
{
	Super::UnPossessed();
}

// Called every frame
void ADrivableSpaceVehicle::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADrivableSpaceVehicle::InitializeAbility(TSubclassOf<UGameplayAbility> AbilityToGet, int32 AbilityLevel)
{
	if (AbilitySystemComponent)
	{
		if (HasAuthority() && AbilityToGet)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(AbilityToGet, AbilityLevel, 0));
		}

		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}

void ADrivableSpaceVehicle::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	// TODO: Why are the gameplay tags all on the ASC instead of the Character?
	// the Character supports tags...
	if (auto ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool ADrivableSpaceVehicle::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool ADrivableSpaceVehicle::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool ADrivableSpaceVehicle::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}
