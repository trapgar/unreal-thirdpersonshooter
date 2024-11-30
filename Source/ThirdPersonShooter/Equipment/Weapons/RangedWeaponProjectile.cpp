#include "Equipment/Weapons/RangedWeaponProjectile.h"
#include "Equipment/Weapons/RangedWeaponItemInstance.h"
#include "Components/CapsuleComponent.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "AbilitySystemGlobals.h"
#include "ThirdPersonShooterGameplayTags.h"
#include "GameplayCueFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "CoreMinimal.h"

static FName NAME_ProjectileCollisionProfile(TEXT("Projectile"));

ARangedWeaponProjectile::ARangedWeaponProjectile()
{
	RootComponent = CollisionVolume = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CollisionVolume"));

	CollisionVolume->InitCapsuleSize(1.0f, 1.0f);
	CollisionVolume->SetCollisionProfileName(NAME_ProjectileCollisionProfile);
	CollisionVolume->SetWalkableSlopeOverride(FWalkableSlopeOverride(WalkableSlope_Unwalkable, 0.f));
	CollisionVolume->CanCharacterStepUpOn = ECB_No;
	CollisionVolume->bReturnMaterialOnMove = true;

	ProjectileMovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileMovementComponent"));
	ProjectileMovementComponent->UpdatedComponent = CollisionVolume;
	ProjectileMovementComponent->bRotationFollowsVelocity = true;
	ProjectileMovementComponent->bShouldBounce = false;

	InitialLifeSpan = 3.0f;
	bReplicates = true;

	CollisionVolume->OnComponentHit.AddDynamic(this, &ARangedWeaponProjectile::OnHit);
}

void ARangedWeaponProjectile::OnConstruction(const FTransform & Transform)
{
	Super::OnConstruction(Transform);

	StartingLocation = Transform.GetLocation();

	if (Weapon)
	{
		ProjectileMovementComponent->InitialSpeed = Weapon->MuzzleVelocity;
		ProjectileMovementComponent->MaxSpeed = Weapon->MuzzleVelocity;
	}
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
	EffectContext->AddOrigin(StartingLocation);

	if (const IModularAbilityAttenuatorInterface* Attenuator = Cast<const IModularAbilityAttenuatorInterface>(Source))
	{
		EffectContext->SetAbilityAttenuator(Attenuator, 0.0f);
	}

	return ContextHandle;
}

void ARangedWeaponProjectile::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	K2_OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);

	FGameplayEffectContextHandle GE_Handle_Damage = MakeEffectContext();

	// is damageable - add the GameplayEffect
	if (UAbilitySystemComponent* ASC = OtherActor->GetComponentByClass<UAbilitySystemComponent>())
	{
		GE_Handle_Damage.AddHitResult(Hit, /*bReset=*/true);

		// May not have a value if it doesn't apply direct damage (e.g.: grenade launcher)
		if (Weapon->DamageType)
		{
			if (HasAuthority())
			{
				UAbilitySystemComponent* MyASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetInstigator());
				FGameplayEffectSpecHandle GE_Spec_Handle = MyASC->MakeOutgoingSpec(Weapon->DamageType, /*Level=*/0.0f, GE_Handle_Damage);
				GE_Spec_Handle.Data.Get()->SetSetByCallerMagnitude(ThirdPersonShooterGameplayTags::SetByCaller_Damage, Weapon->SingleBulletDamage);

				MyASC->ApplyGameplayEffectSpecToTarget(*GE_Spec_Handle.Data.Get(), ASC);
			}
		}
	}

	FGameplayCueParameters GCN_Params = UGameplayCueFunctionLibrary::MakeGameplayCueParametersFromHitResult(Hit);
	GCN_Params.Instigator = GetInstigator();
	GCN_Params.EffectCauser = this;
	GCN_Params.EffectContext = GE_Handle_Damage;

	// Burst to tell anyone listening we hit something
	UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(
		this,
		ThirdPersonShooterGameplayTags::GameplayCue_Projectile_Impact,
		GCN_Params
	);

	// Destroy/Ignore based on the physical material of the object we hit
	switch (UGameplayStatics::GetSurfaceType(Hit))
	{
	// Character
	case EPhysicalSurface::SurfaceType1:
	// Concrete
	case EPhysicalSurface::SurfaceType2:
	// Glass
	case EPhysicalSurface::SurfaceType3:
	// Metal
	case EPhysicalSurface::SurfaceType4:
		// TODO: Allow for penetration with char, glass, thin wood, etc.
		Destroy();
		break;
	default:
		Destroy();
		break;
	}

	// Only add impulse and destroy projectile if we hit a physics object
	if ((OtherActor != nullptr) && (OtherActor != this) && (OtherComp != nullptr) && OtherComp->IsSimulatingPhysics())
	{
		// TODO: calculate force based on mass & velocity, keeping in mind it will probably just tear right through the obj
		OtherComp->AddImpulseAtLocation(GetVelocity() * 0.5f, GetActorLocation());
	}
	else if (OtherActor != nullptr && OtherActor->CanBeDamaged())
	{
		if (HasAuthority())
		{
			UGameplayStatics::ApplyDamage(OtherActor, Weapon->SingleBulletDamage, GetInstigatorController(), this, UDamageType::StaticClass());
		}
	}
}
