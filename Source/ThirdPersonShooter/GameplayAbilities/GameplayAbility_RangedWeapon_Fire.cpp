#include "GameplayAbilities/GameplayAbility_RangedWeapon_Fire.h"
#include "Physics/CustomCollisionChannels.h"
#include "Inventory/InventoryItemInstance.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "NativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Messages/GameplayVerbMessage.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbility_RangedWeapon_Fire)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Message_Weapon_Fired, "Gameplay.Message.Weapon.Fired");

void UGameplayAbility_RangedWeapon_Fire::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	UInventoryItemInstance* Item = GetAssociatedItem();
	UEquipmentItemInstance* Equipment = GetAssociatedEquipment();

	check(Item);
	check(Equipment);

	UEquipmentItemDefinition* Definition = Equipment->GetItemDef();

	check(Definition);

	URangedWeaponItemInstance* RangedWeapon = GetAssociatedWeapon();

	check(RangedWeapon);

	if (AActor* EActor = Equipment->FindSpawnedActorByClass<AActor>())
	{
		WeaponComponent = EActor->GetRootComponent();
	}
	else if (AActor* IActor = Item->FindSpawnedActorByClass<AActor>())
	{
		WeaponComponent = IActor->GetRootComponent();
	}
}

void UGameplayAbility_RangedWeapon_Fire::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);
}

void UGameplayAbility_RangedWeapon_Fire::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData
)
{
	if (CommitAbility(Handle, ActorInfo, ActivationInfo, nullptr))
	{
		Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

		URangedWeaponItemInstance* RangedWeapon = GetAssociatedWeapon();
		RangedWeapon->Fire();
		FGameplayVerbMessage Message;
		Message.Instigator = GetPawnFromActorInfo();
		Message.Target = RangedWeapon;
		Message.Magnitude = GetWorld()->GetTimeSeconds();

		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
		MessageSystem.BroadcastMessage(TAG_Gameplay_Message_Weapon_Fired, Message);
	}
	else
	{
		constexpr bool bReplicateEndAbility = true;
		constexpr bool bWasCancelled = true;
		EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	}
}

void UGameplayAbility_RangedWeapon_Fire::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility,
	bool bWasCancelled
)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

FTransform UGameplayAbility_RangedWeapon_Fire::GetBaseProjectileSpawnTransform(float Radius, float MaxTraceDistanceInCm) const
{
	check(WeaponComponent);
	APawn* Pawn = GetPawnFromActorInfo();
	check(Pawn);

	AController* Controller = Pawn->Controller;
	check(Controller);

	FVector ViewLocation;
	FRotator ViewRotation;
	Controller->GetPlayerViewPoint(/*OutViewLocation*/ ViewLocation, /*OutViewRotation*/ ViewRotation);

	TArray<FHitResult> HitResults;
	FVector StartTrace = ViewLocation;
	FVector EndTrace = ViewLocation + UKismetMathLibrary::GetForwardVector(ViewRotation) * MaxTraceDistanceInCm;
	ECollisionChannel TraceChannel = Custom_TraceChannel_Projectile;
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(GetBaseProjectileSpawnTransform), /*bTraceComplex=*/ true, /*IgnoreActor=*/ Pawn);

	if (Radius > 0.0f)
	{
		GetWorld()->SweepMultiByChannel(HitResults, StartTrace, EndTrace, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(Radius), TraceParams);
	}
	else
	{
		GetWorld()->LineTraceMultiByChannel(HitResults, StartTrace, EndTrace, TraceChannel, TraceParams);
	}

	FHitResult Hit(ForceInit);

	if (HitResults.Num() > 0)
	{
		// Filter the output list to prevent multiple hits on the same actor;
		// this is to prevent a single bullet dealing damage multiple times to
		// a single actor if using an overlap trace
		for (FHitResult& CurHitResult : HitResults)
		{
			auto Pred = [&CurHitResult](const FHitResult& Other)
			{
				return Other.HitObjectHandle == CurHitResult.HitObjectHandle;
			};

			if (!HitResults.ContainsByPredicate(Pred))
			{
				HitResults.Add(CurHitResult);
			}
		}

		Hit = HitResults.Last();
	}
	else
	{
		Hit.TraceStart = StartTrace;
		Hit.TraceEnd = EndTrace;
		Hit.ImpactPoint = EndTrace;
	}

	FTransform Transform = WeaponComponent->GetSocketTransform("Muzzle", ERelativeTransformSpace::RTS_World);
	FVector Location = Transform.GetLocation();
	FRotator Rotation = Transform.Rotator();
	FVector TwoCentimeters = UKismetMathLibrary::GetForwardVector(Rotation) * 2.0f;
	FVector BulletSpawnLocation = Location + TwoCentimeters;
	FRotator BulletSpawnRotation = UKismetMathLibrary::FindLookAtRotation(BulletSpawnLocation, Hit.ImpactPoint);

	return UKismetMathLibrary::MakeTransform(BulletSpawnLocation, BulletSpawnRotation, FVector(1.0f, 1.0f, 1.0f));
}

FRotator UGameplayAbility_RangedWeapon_Fire::GetProjectileSpreadRotator() const
{
	APawn* Pawn = GetPawnFromActorInfo();
	check(Pawn);

	URangedWeaponItemInstance* RangedWeapon = GetAssociatedWeapon();
	float SpreadAngle = RangedWeapon->GetSpreadAngle();
	float Multiplier = RangedWeapon->GetSpreadAngleMultiplier();

	if (SpreadAngle == RangedWeapon->SpreadAngleBase && RangedWeapon->HasFirstShotAccuracy())
	{
		Multiplier = 0.0f;
	}

	float Accumulated = SpreadAngle;
	float Max = Accumulated * Multiplier;
	float Min = Max * -1;

	float Pitch = FMath::RandRange(Min, Max);
	float Yaw = FMath::RandRange(Min, Max);

	return FRotator(Pitch, Yaw, 0.0f);
}
