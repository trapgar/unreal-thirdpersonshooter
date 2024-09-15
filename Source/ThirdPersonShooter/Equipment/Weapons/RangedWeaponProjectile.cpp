#include "Equipment/Weapons/RangedWeaponProjectile.h"
#include "Equipment/Weapons/RangedWeaponItemInstance.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

static FName NAME_ProjectileCollisionProfile(TEXT("Projectile"));

ARangedWeaponProjectile::ARangedWeaponProjectile()
{
	RootComponent = CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	CollisionVolume->InitCapsuleSize(5.0f, 5.0f);
	CollisionVolume->SetCollisionProfileName(NAME_ProjectileCollisionProfile);
	CollisionVolume->CanCharacterStepUpOn = ECB_No;
	CollisionVolume->bReturnMaterialOnMove = true;
}

void ARangedWeaponProjectile::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	if (Weapon)
	{
		ProjectileMovementComponent->InitialSpeed = Weapon->MuzzleVelocity;
	}
}

void ARangedWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();

	if (MaxLifeTime > 0)
	{
		UWorld* World = GetWorld();
		World->GetTimerManager().SetTimer(ExpirationTimerHandle, this, &ThisClass::OnExpiration, MaxLifeTime, false);
	}
}

void ARangedWeaponProjectile::OnExpiration()
{
	Destroy();
}

void ARangedWeaponProjectile::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UWorld* World = GetWorld();
	World->GetTimerManager().ClearTimer(ExpirationTimerHandle);

	Super::EndPlay(EndPlayReason);
}

FGameplayEffectContextHandle ARangedWeaponProjectile::MakeEffectContext() const
{
	AActor* AsInstigator = GetInstigator();
	IAbilitySystemInterface* Abilitor = Cast<IAbilitySystemInterface>(AsInstigator);

	check(AsInstigator);
	check(Abilitor);

	UAbilitySystemComponent* ASC = Abilitor->GetAbilitySystemComponent();
	FGameplayEffectContextHandle ContextHandle = ASC->MakeEffectContext();
	FModularGameplayEffectContext* EffectContext = FModularGameplayEffectContext::ExtractEffectContext(ContextHandle);
	const UObject* Source = Cast<const UObject>(Weapon);

	check(EffectContext);
	check(Source);

	EffectContext->AddInstigator(AsInstigator, AsInstigator);
	EffectContext->AddSourceObject(Source);

	if (const IModularAbilityAttenuatorInterface* Attenuator = Cast<const IModularAbilityAttenuatorInterface>(Source))
	{
		EffectContext->SetAbilityAttenuator(Attenuator, 0.0f);
	}

	return ContextHandle;
}
