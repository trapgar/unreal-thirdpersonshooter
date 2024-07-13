#include "Equipment/Weapons/RangedWeaponProjectile.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"

ARangedWeaponProjectile::ARangedWeaponProjectile()
{
	RootComponent = CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));
	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));

	CollisionVolume->InitCapsuleSize(5.0f, 5.0f);
	CollisionVolume->SetCollisionProfileName(TEXT("PhysicsActor"));
	CollisionVolume->CanCharacterStepUpOn = ECB_No;
}

void ARangedWeaponProjectile::OnConstruction(const FTransform & Transform)
{
	ProjectileMovementComponent->InitialSpeed = InitialSpeed;
}

void ARangedWeaponProjectile::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	World->GetTimerManager().SetTimer(ExpirationTimerHandle, this, &ThisClass::OnExpiration, MaxLifeTime, false);
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
