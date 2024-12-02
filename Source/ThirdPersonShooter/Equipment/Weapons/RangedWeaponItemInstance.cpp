#include "Equipment/Weapons/RangedWeaponItemInstance.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Camera/DynamicCameraComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RangedWeaponItemInstance)

UE_DEFINE_GAMEPLAY_TAG(TAG_Equipment_Weapon_Ammunition, "Equipment.Weapon.Ammunition");
UE_DEFINE_GAMEPLAY_TAG(TAG_Equipment_Weapon_MagazineSize, "Equipment.Weapon.MagazineSize");
UE_DEFINE_GAMEPLAY_TAG(TAG_Movement_AimingDownSights, "Movement.Mode.ADS");

// The speed threshold below which the player is considered "standing still" (cm/s)
const float StandingStillSpeedThreshold = 80.0f;

void URangedWeaponItemInstance::OnEquipped()
{
	if (UWorld* World = GetWorld())
	{
		TimeLastEquipped = World->GetTimeSeconds();
	}

	Super::OnEquipped();
}

void URangedWeaponItemInstance::OnUnequipped()
{
	Super::OnUnequipped();
}

void URangedWeaponItemInstance::Tick(float DeltaSeconds)
{
	APawn* Pawn = GetInstigator();
	check(Pawn != nullptr);
	
	const bool bMinSpread = UpdateSpread(DeltaSeconds);
	const bool bMinMultipliers = UpdateMultipliers(DeltaSeconds);

	bApplyFirstShotAccuracy = bHasFirstShotAccuracy && bMinMultipliers && bMinSpread;
}

bool URangedWeaponItemInstance::UpdateSpread(float DeltaSeconds)
{
	const float TimeSinceLastFired = GetWorld()->TimeSince(TimeLastFired);
	const float StartSpreadDecayThreshold = 60 / RoundsPerMinute * 1.5f;

	// Don't decay the spread if we have just fired
	if (TimeLastFired > 0.0f && TimeSinceLastFired > StartSpreadDecayThreshold)
	{
		AccumulatedSpreadAngle = FMath::Max(0.0f, AccumulatedSpreadAngle - SpreadAngleAccumulationDecayPerSecond / (TimeSinceLastFired - StartSpreadDecayThreshold));

		// Spread is at minimum, set to zero and stop the timer
		if (FMath::IsNearlyEqual(AccumulatedSpreadAngle, 0.0f, KINDA_SMALL_NUMBER))
		{
			AccumulatedSpreadAngle = 0.0f;
		}
	}

	return FMath::IsNearlyEqual(AccumulatedSpreadAngle, 0.0f, KINDA_SMALL_NUMBER);
}

bool URangedWeaponItemInstance::UpdateMultipliers(float DeltaSeconds)
{
	const float MultiplierNearlyEqualThreshold = 5.0f;

	APawn* Pawn = GetInstigator();
	check(Pawn != nullptr);

	// See if we are standing still, and if so, smoothly apply the bonus
	const float PawnSpeed = Pawn->GetVelocity().Size();
	const float MovementTargetValue = FMath::GetMappedRangeValueClamped(
		/*InputRange=*/ FVector2D(StandingStillSpeedThreshold, StandingStillSpeedThreshold + 20.0f),
		/*OutputRange=*/ FVector2D(1.0f, SpreadAngleMultiplier_Moving),
		/*Alpha=*/ PawnSpeed
	);
	CurrentMultiplier_StandingStill = FMath::FInterpTo(CurrentMultiplier_StandingStill, MovementTargetValue, DeltaSeconds, 5.0f);
	const bool bStandingStillMultiplierAtMin = FMath::IsNearlyEqual(CurrentMultiplier_StandingStill, 1.0f, 0.1f);

	// See if we are crouching, and if so, smoothly apply the bonus
	UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());
	const bool bIsCrouching = (CharMovementComp != nullptr) && CharMovementComp->IsCrouching();
	const float CrouchingTargetValue = bIsCrouching ? SpreadAngleMultiplier_Crouching : 1.0f;
	CurrentMultiplier_Crouching = FMath::FInterpTo(CurrentMultiplier_Crouching, CrouchingTargetValue, DeltaSeconds, 5.0f);
	const bool bCrouchingMultiplierAtTarget = FMath::IsNearlyEqual(CurrentMultiplier_Crouching, CrouchingTargetValue, MultiplierNearlyEqualThreshold);

	// See if we are in the air (jumping/falling), and if so, smoothly apply the penalty
	const bool bIsJumpingOrFalling = (CharMovementComp != nullptr) && CharMovementComp->IsFalling();
	const float JumpFallTargetValue = bIsJumpingOrFalling ? SpreadAngleMultiplier_Falling : 1.0f;
	CurrentMultiplier_Falling = FMath::FInterpTo(CurrentMultiplier_Falling, JumpFallTargetValue, DeltaSeconds, 5.0f);
	const bool bJumpFallMultiplerIs1 = FMath::IsNearlyEqual(CurrentMultiplier_Falling, 1.0f, MultiplierNearlyEqualThreshold);

	// Determine if we are aiming down sights, and apply the bonus based on how far into the camera transition we are
	float AimingAlpha = 0.0f;
	if (const UDynamicCameraComponent* CameraComponent = UDynamicCameraComponent::FindCameraComponent(Pawn))
	{
		float TopCameraWeight;
		FGameplayTag TopCameraTag;
		CameraComponent->GetBlendInfo(/*out*/ TopCameraWeight, /*out*/ TopCameraTag);

		if (IGameplayTagAssetInterface* Taggable = Cast<IGameplayTagAssetInterface>(Pawn))
		{
			if (Taggable->HasMatchingGameplayTag(TAG_Movement_AimingDownSights))
			{
				AimingAlpha = TopCameraWeight;
			}
		}
	}
	const float AimingMultiplier = FMath::GetMappedRangeValueClamped(
		/*InputRange=*/ FVector2D(0.0f, 1.0f),
		/*OutputRange=*/ FVector2D(SpreadAngleMultiplier_HipFire, SpreadAngleMultiplier_AimDownSight),
		/*Alpha=*/ AimingAlpha
	);
	const bool bAimingMultiplierAtTarget = FMath::IsNearlyEqual(AimingMultiplier, SpreadAngleMultiplier_AimDownSight, KINDA_SMALL_NUMBER);

	// Combine all the multipliers
	const float CombinedMultiplier = AimingMultiplier * CurrentMultiplier_StandingStill * CurrentMultiplier_Crouching * CurrentMultiplier_Falling;
	AccumulatedSpreadAngleMultiplier = CombinedMultiplier;

	// need to handle these spread multipliers indicating we are not at min spread
	return bStandingStillMultiplierAtMin && bCrouchingMultiplierAtTarget && bJumpFallMultiplerIs1 && bAimingMultiplierAtTarget;
}
