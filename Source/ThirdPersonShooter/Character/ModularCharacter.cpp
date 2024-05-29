#include "ModularCharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Components/Ability/ModularAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThirdPersonShooterGameplayTags.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularCharacter)

class UEnhancedInputLocalPlayerSubsystem;

AModularCharacter::AModularCharacter()
{
	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	AbilitySystemComponent = CreateDefaultSubobject<UModularAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	GameplayInputComponent = CreateDefaultSubobject<UGameplayInputComponent>(TEXT("GameplayInputComponent"));
}

void AModularCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// Called when the game starts or when spawned
void AModularCharacter::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AModularCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AModularCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);
}

void AModularCharacter::UnPossessed()
{
	Super::UnPossessed();
}

void AModularCharacter::OnStartCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		ASC->SetLooseGameplayTagCount(ThirdPersonShooterGameplayTags::Status_Crouching, 1);
	}

	Super::OnStartCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

void AModularCharacter::OnEndCrouch(float HalfHeightAdjust, float ScaledHalfHeightAdjust)
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		ASC->SetLooseGameplayTagCount(ThirdPersonShooterGameplayTags::Status_Crouching, 0);
	}

	Super::OnEndCrouch(HalfHeightAdjust, ScaledHalfHeightAdjust);
}

// Called every frame
void AModularCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AModularCharacter::InitializeAbility(TSubclassOf<UGameplayAbility> AbilityToGet, int32 AbilityLevel)
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

UAbilitySystemComponent* AModularCharacter::GetAbilitySystemComponent() const
{
    return AbilitySystemComponent;
}

void AModularCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	// TODO: Why are the gameplay tags all on the ASC instead of the Character?
	// the Character supports tags...
	if (auto ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AModularCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AModularCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AModularCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}
