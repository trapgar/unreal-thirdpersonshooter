#include "Abilities/GameplayAbility_RangedWeapon_Fire.h"
#include "Physics/CustomCollisionChannels.h"
#include "Inventory/InventoryItemInstance.h"
#include "Equipment/EquipmentManagerComponent.h"
#include "NativeGameplayTags.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayAbility_RangedWeapon_Fire)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Movement_AimingDownSights, "Event.Movement.ADS");

void UGameplayAbility_RangedWeapon_Fire::OnGiveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnGiveAbility(ActorInfo, Spec);

	TimeLastEquipped = GetWorld()->GetTimeSeconds();
	UInventoryItemInstance* Item = GetAssociatedItem();
	UEquipmentItemInstance* Equipment = GetAssociatedEquipment();

	check(Item);
	check(Equipment);

	UEquipmentItemDefinition* Definition = Equipment->GetItemDef();

	check(Definition);

	RangedWeaponStats = Definition->FindFragmentByClass<UEquipmentFragment_RangedWeaponStats>();

	check(RangedWeaponStats);

	URangedWeaponItemInstance* RangedWeapon = GetAssociatedWeapon();

	check(RangedWeapon);
	
	RangedWeapon->SetTimeLastEquipped(TimeLastEquipped);

	if (AActor* EActor = Equipment->FindSpawnedActorByClass<AActor>())
	{
		WeaponComponent = EActor->GetRootComponent();
	}
	else if (AActor* IActor = Item->FindSpawnedActorByClass<AActor>())
	{
		WeaponComponent = IActor->GetRootComponent();
	}

	GetWorld()->GetTimerManager().SetTimer(TimerHandleSpread, this, &UGameplayAbility_RangedWeapon_Fire::OnTickSpreadCheck, 0.1f, true);
}

void UGameplayAbility_RangedWeapon_Fire::OnRemoveAbility(const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilitySpec& Spec)
{
	Super::OnRemoveAbility(ActorInfo, Spec);

	GetWorld()->GetTimerManager().ClearTimer(TimerHandleSpread);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandleSpreadDecay);
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

		UWorld* World = GetWorld();
		TimeLastFired = World->GetTimeSeconds();

		UInventoryItemInstance* Item = GetAssociatedItem();
		bHas1InTheChamber = Item->GetStatTagStackCount(TAG_Equipment_Weapon_Ammunition) > 0;

		AccumulatedSpreadAngleMultiplier += 1.49f;

		URangedWeaponItemInstance* RangedWeapon = GetAssociatedWeapon();
		RangedWeapon->SetHas1InTheChamber(bHas1InTheChamber);
		OnTickSpreadCheck();

		World->GetTimerManager().ClearTimer(TimerHandleSpreadDecay);
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

	if (!bWasCancelled)
	{
		UWorld* World = GetWorld();
		// NOTE - timers are invalidated implicitly on ActivateAbility, so we need to set them again
		World->GetTimerManager().SetTimer(TimerHandleSpread, this, &UGameplayAbility_RangedWeapon_Fire::OnTickSpreadCheck, 0.1f, true);
		World->GetTimerManager().SetTimer(TimerHandleSpreadDecay,
			this,
			&UGameplayAbility_RangedWeapon_Fire::OnTickSpreadDecay,
			// 60s / rounds per minute (e.g.: 800rpm) * 1.25 == min time to pass before the accumulated spread decays
			// Would *like* this to just be 60/rpm, but depending on timing the decay may occur before the activation resets the handle
			(60 / RangedWeaponStats->RoundsPerMinute) * 1.25f,
			false
		);
	}
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
	TraceParams.bReturnPhysicalMaterial = true;

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

	float BaseSpread = RangedWeaponStats->SpreadAngleAimDownSight;
	float Multiplier = CalculateSpreadAngleMultiplier();
	float Max = BaseSpread * Multiplier;
	float Min = Max * -1;

	float Pitch = FMath::RandRange(Min, Max);
	float Yaw = FMath::RandRange(Min, Max);

	return FRotator(Pitch, Yaw, 0.0f);
}

float UGameplayAbility_RangedWeapon_Fire::CalculateSpreadAngleMultiplier() const
{
	float RunningMultiplier = 0.0f;

	if (AccumulatedSpreadAngleMultiplier > 1.0f)
	{
		RunningMultiplier += AccumulatedSpreadAngleMultiplier;
	}

	// See if we are moving, and if so, smoothly apply the penalty
	APawn* Pawn = GetPawnFromActorInfo();
	UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	const float PawnSpeed = Pawn->GetVelocity().Size();

	// TODO: want "close" to 0 speed to count as standing still, but idk what uom this is - need to check (think lyra has 80.0f?)
	if (PawnSpeed > 10.0f)
	{
		RunningMultiplier += RangedWeaponStats->SpreadAngleMultiplierWhileMoving;
	}

	if (IGameplayTagAssetInterface* Taggable = Cast<IGameplayTagAssetInterface>(Pawn))
	{
		if (!Taggable->HasMatchingGameplayTag(TAG_Movement_AimingDownSights))
		{
			RunningMultiplier += RangedWeaponStats->SpreadAngleMultiplierHipFire;
		}
	}

	float BaseSpread = RangedWeaponStats->SpreadAngleAimDownSight;
	float Max = BaseSpread * RunningMultiplier;
	
	return RunningMultiplier;
}

void UGameplayAbility_RangedWeapon_Fire::OnTickSpreadCheck()
{
	URangedWeaponItemInstance* RangedWeapon = GetAssociatedWeapon();

	// TODO: Not a huge fan of how this is done, but idk how else to get this info to the reticle widget
	// Maybe the GameplayMessageRouter?
	float BaseSpreadAngle = RangedWeaponStats->SpreadAngleAimDownSight;
	float CurrentSpreadMultiplier = CalculateSpreadAngleMultiplier();
	RangedWeapon->SetSpreadAngle(BaseSpreadAngle);
	RangedWeapon->SetSpreadAngleMultiplier(CurrentSpreadMultiplier);
}

void UGameplayAbility_RangedWeapon_Fire::OnTickSpreadDecay()
{
	UWorld* World = GetWorld();
	// TODO: keep track of when this func was last called, so if it's been longer than the expected 100ms,
	// we can decrement the accumulated multiplier by the correct amount
	const float TimeSinceFired = World->TimeSince(TimeLastFired);
	const float DeltaSeconds = World->DeltaTimeSeconds;

	AccumulatedSpreadAngleMultiplier = FMath::Max(1.0f, AccumulatedSpreadAngleMultiplier - 1.11f);

	// Spread is at minimum, set to zero and stop the timer
	if (FMath::IsNearlyEqual(AccumulatedSpreadAngleMultiplier, 1.0f, KINDA_SMALL_NUMBER))
	{
		AccumulatedSpreadAngleMultiplier = 1.0f;
	}
	// Queue another tick to update the spread again
	else
	{
		World->GetTimerManager().SetTimer(TimerHandleSpreadDecay,
			this,
			&UGameplayAbility_RangedWeapon_Fire::OnTickSpreadDecay,
			0.1f,
			false
		);
	}
}
