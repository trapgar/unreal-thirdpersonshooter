#include "Equipment/Weapons/RangedWeaponItemInstance.h"
#include "NativeGameplayTags.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Equipment/EquipmentManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RangedWeaponItemInstance)

UE_DEFINE_GAMEPLAY_TAG_STATIC(TAG_Movement_AimingDownSights, "Event.Movement.ADS");

URangedWeaponItemInstance::URangedWeaponItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void URangedWeaponItemInstance::Tick(float DeltaSeconds)
{
	APawn* Pawn = GetInstigator();
	check(Pawn != nullptr);

	const bool bMinSpread = UpdateSpread(DeltaSeconds);
	const bool bMinMultipliers = UpdateMultipliers(DeltaSeconds);

	bHasFirstShotAccuracy = bAllowFirstShotAccuracy && bMinMultipliers && bMinSpread;
}

void URangedWeaponItemInstance::OnEquipped()
{
	TimeLastEquipped = GetWorld()->GetTimeSeconds();
	UEquipmentItemDefinition* WeaponDef = GetItemDef();

	check(WeaponDef);

	RangedWeaponStats = WeaponDef->FindFragmentByClass<UEquipmentFragment_RangedWeaponStats>();

	check(RangedWeaponStats);

	bHasFirstShotAccuracy = RangedWeaponStats->bHasFirstShotAccuracy;
	CurrentSpreadAngle = RangedWeaponStats->SpreadAngleAimDownSight;

	Super::OnEquipped();
}

void URangedWeaponItemInstance::OnFired()
{
	TimeLastFired = GetWorld()->GetTimeSeconds();

	if (UInventoryItemInstance* Item = GetAssociatedItem())
	{
		bHas1InTheChamber = Item->GetStatTagStackCount(TAG_Equipment_Weapon_Ammunition) > 0;
	}

	K2_OnFired();
}

bool URangedWeaponItemInstance::UpdateSpread(float DeltaSeconds)
{
	const float TimeSinceFired = GetWorld()->TimeSince(TimeLastFired);

	// if (TimeSinceFired > SpreadRecoveryCooldownDelay)
	// {
	// 	const float CooldownRate = HeatToCoolDownPerSecondCurve.GetRichCurveConst()->Eval(CurrentHeat);
	// 	CurrentHeat = ClampHeat(CurrentHeat - (CooldownRate * DeltaSeconds));
	// 	CurrentSpreadAngle = HeatToSpreadCurve.GetRichCurveConst()->Eval(CurrentHeat);
	// }

	// float MinSpread;
	// float MaxSpread;
	// ComputeSpreadRange(/*out*/ MinSpread, /*out*/ MaxSpread);

	// return FMath::IsNearlyEqual(CurrentSpreadAngle, MinSpread, KINDA_SMALL_NUMBER);
	return true;
}

bool URangedWeaponItemInstance::UpdateMultipliers(float DeltaSeconds)
{
	float RunningMultiplier = 1.0f;

	APawn* Pawn = GetInstigator();
	check(Pawn);

	UCharacterMovementComponent* CharMovementComp = Cast<UCharacterMovementComponent>(Pawn->GetMovementComponent());

	// See if we are standing still, and if so, smoothly apply the bonus
	const float PawnSpeed = Pawn->GetVelocity().Size();

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

	CurrentSpreadAngleMultiplier = RunningMultiplier;

	return FMath::IsNearlyEqual(CurrentSpreadAngleMultiplier, 1.0f, KINDA_SMALL_NUMBER);
}
