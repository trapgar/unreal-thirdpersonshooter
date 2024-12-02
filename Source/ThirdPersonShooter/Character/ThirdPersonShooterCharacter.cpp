#include "ThirdPersonShooterCharacter.h"
#include "Components/CapsuleComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameplayAbilities/ModularAbilitySystemComponent.h"
#include "ThirdPersonShooterCharacterMovementComponent.h"
#include "EnhancedInputSubsystems.h"
#include "ThirdPersonShooterGameplayTags.h"
#include "Combat/HealthComponent.h"
#include "Camera/DynamicCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ThirdPersonShooterCharacter)

UE_DEFINE_GAMEPLAY_TAG(TAG_GameplayEvent_Pawn_ControllerChanged, "GameplayEvent.Pawn.ControllerChanged");

class UEnhancedInputLocalPlayerSubsystem;

static FName NAME_CustomCharacterCollisionProfile_Capsule(TEXT("Pawn_Capsule"));
static FName NAME_CustomCharacterCollisionProfile_Mesh(TEXT("Pawn_Mesh"));

AThirdPersonShooterCharacter::AThirdPersonShooterCharacter(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer.SetDefaultSubobjectClass<UThirdPersonShooterCharacterMovementComponent>(ACharacter::CharacterMovementComponentName))
{
	// Avoid ticking characters if possible.
	PrimaryActorTick.bCanEverTick = false;
	PrimaryActorTick.bStartWithTickEnabled = false;

	SetNetCullDistanceSquared(900000000.0f);

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionProfileName(NAME_CustomCharacterCollisionProfile_Capsule);

	USkeletalMeshComponent* MeshComp = GetMesh();
	check(MeshComp);
	MeshComp->SetCollisionProfileName(NAME_CustomCharacterCollisionProfile_Mesh);
	MeshComp->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, 0.0f, -90.0f));

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	check(Movement);
	Movement->GetNavAgentPropertiesRef().bCanCrouch = true;
	Movement->bCanWalkOffLedgesWhenCrouching = true;
	Movement->SetCrouchedHalfHeight(65.0f);
	Movement->BrakingFrictionFactor = 1.0f;
	Movement->BrakingFriction = 6.0f;
	Movement->GroundFriction = 8.0f;
	Movement->BrakingDecelerationWalking = 1400.0f;
	Movement->RotationRate = FRotator(0.0f, 0.0f, 720.0f);
	Movement->MaxFlySpeed = 3200.0f; // 32 m/s

	AbilitySystemComponent = CreateDefaultSubobject<UModularAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	GameplayInputComponent = CreateDefaultSubobject<UGameplayInputComponent>(TEXT("GameplayInputComponent"));

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	HealthComponent->OnDeathStarted.AddDynamic(this, &ThisClass::OnDeathStarted);
	HealthComponent->OnDeathFinished.AddDynamic(this, &ThisClass::OnDeathFinished);

	CameraComponent = CreateDefaultSubobject<UDynamicCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetRelativeLocation(FVector(-300.0f, 0.0f, 75.0f));
}

void AThirdPersonShooterCharacter::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

// Called when the game starts or when spawned
void AThirdPersonShooterCharacter::BeginPlay()
{
	UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(this, UGameFrameworkComponentManager::NAME_GameActorReady);
	Super::BeginPlay();
}

void AThirdPersonShooterCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

void AThirdPersonShooterCharacter::PossessedBy(AController *NewController)
{
	Super::PossessedBy(NewController);

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		// Need to call ASAP because of the GA_Interact on a timer which will call with stale values
		// TODO: Can't the GameplayAbility listen for this event?
		MASC->HandleControllerChanged();
		HealthComponent->InitializeWithAbilitySystem(AbilitySystemComponent);

		FGameplayEventData Payload;
		Payload.EventTag = TAG_GameplayEvent_Pawn_ControllerChanged;
		Payload.Instigator = this;
		Payload.Target = this;
		Payload.OptionalObject = NewController;
		MASC->HandleGameplayEvent(TAG_GameplayEvent_Pawn_ControllerChanged, &Payload);
	}
}

void AThirdPersonShooterCharacter::UnPossessed()
{
	Super::UnPossessed();

	if (auto MASC = GetAbilitySystemComponent<UModularAbilitySystemComponent>())
	{
		MASC->HandleControllerChanged();
		HealthComponent->UninitializeFromAbilitySystem();

		FGameplayEventData Payload;
		Payload.EventTag = TAG_GameplayEvent_Pawn_ControllerChanged;
		Payload.Instigator = this;
		Payload.Target = this;
		MASC->HandleGameplayEvent(TAG_GameplayEvent_Pawn_ControllerChanged, &Payload);
	}
}

void AThirdPersonShooterCharacter::SetMovementModeTag(EMovementMode MovementMode, uint8 CustomMovementMode, bool bTagEnabled)
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		const FGameplayTag* MovementModeTag = nullptr;

		if (MovementMode != MOVE_Custom)
		{
			MovementModeTag = ThirdPersonShooterGameplayTags::MovementModeTagMap.Find(MovementMode);
		}

		if (MovementModeTag && MovementModeTag->IsValid())
		{
			ASC->SetLooseGameplayTagCount(*MovementModeTag, (bTagEnabled ? 1 : 0));
		}
	}
}

void AThirdPersonShooterCharacter::GetOwnedGameplayTags(FGameplayTagContainer& TagContainer) const
{
	// TODO: Why are the gameplay tags all on the ASC instead of the Character?
	// the Character supports tags...
	if (auto ASC = GetAbilitySystemComponent())
	{
		ASC->GetOwnedGameplayTags(TagContainer);
	}
}

bool AThirdPersonShooterCharacter::HasMatchingGameplayTag(FGameplayTag TagToCheck) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasMatchingGameplayTag(TagToCheck);
	}

	return false;
}

bool AThirdPersonShooterCharacter::HasAllMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAllMatchingGameplayTags(TagContainer);
	}

	return false;
}

bool AThirdPersonShooterCharacter::HasAnyMatchingGameplayTags(const FGameplayTagContainer& TagContainer) const
{
	if (auto ASC = GetAbilitySystemComponent())
	{
		return ASC->HasAnyMatchingGameplayTags(TagContainer);
	}

	return false;
}

void AThirdPersonShooterCharacter::OnDeathStarted(AActor*)
{
	if (Controller)
	{
		Controller->SetIgnoreMoveInput(true);
	}

	UCapsuleComponent* CapsuleComp = GetCapsuleComponent();
	check(CapsuleComp);
	CapsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	CapsuleComp->SetCollisionResponseToAllChannels(ECR_Ignore);

	UCharacterMovementComponent* Movement = GetCharacterMovement();
	Movement->StopMovementImmediately();
	Movement->DisableMovement();
}

void AThirdPersonShooterCharacter::OnDeathFinished(AActor*)
{
	GetWorld()->GetTimerManager().SetTimerForNextTick(this, &ThisClass::DestroyDueToDeath);
}

void AThirdPersonShooterCharacter::DestroyDueToDeath()
{
	K2_OnDeathFinished();

	if (GetLocalRole() == ROLE_Authority)
	{
		DetachFromControllerPendingDestroy();
		SetLifeSpan(0.1f);
	}

	// Uninitialize the ASC if we're still the avatar actor (otherwise another pawn already did it when they became the avatar actor)
	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponent())
	{
		if (ASC->GetAvatarActor() == this)
		{
			// PawnExtComponent->UninitializeAbilitySystem();
		}
	}

	SetActorHiddenInGame(true);
}
