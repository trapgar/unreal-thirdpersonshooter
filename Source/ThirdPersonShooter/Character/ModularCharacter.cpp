#include "ModularCharacter.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Ability/ModularAbilitySystemComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThirdPersonShooterGameplayTags.h"
#include "NativeGameplayTags.h"


#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularCharacter)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_GameplayEvent_AvatarPossessed, "GameplayEvent.Possessed");
UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_GameplayEvent_AvatarUnpossessed, "GameplayEvent.Unpossessed");

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

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		MASC->HandleControllerChanged();
		FGameplayEventData Payload;
		Payload.EventTag = TAG_GameplayEvent_AvatarPossessed;
		Payload.Instigator = this;
		Payload.Target = this;
		MASC->HandleGameplayEvent(TAG_GameplayEvent_AvatarPossessed, &Payload);
	}
}

void AModularCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		MASC->HandleControllerChanged();
		FGameplayEventData Payload;
		Payload.EventTag = TAG_GameplayEvent_AvatarUnpossessed;
		Payload.Instigator = this;
		Payload.Target = this;
		MASC->HandleGameplayEvent(TAG_GameplayEvent_AvatarUnpossessed, &Payload);
	}
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
