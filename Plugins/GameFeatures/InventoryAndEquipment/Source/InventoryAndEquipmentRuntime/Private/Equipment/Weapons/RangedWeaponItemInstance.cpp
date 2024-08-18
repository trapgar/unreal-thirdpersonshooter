#include "Equipment/Weapons/RangedWeaponItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RangedWeaponItemInstance)

UE_DEFINE_GAMEPLAY_TAG(TAG_Weapon_Message_StatsChanged, "Equipment.Weapon.Message.StatsChanged");
UE_DEFINE_GAMEPLAY_TAG(TAG_Equipment_Weapon_MagazineSize, "Equipment.Weapon.MagazineSize");

void URangedWeaponItemInstance::OnEquipped()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(World);
		Handle_WeaponStatsChanged = MessageSystem.RegisterListener(TAG_Weapon_Message_StatsChanged, this, &ThisClass::OnWeaponStatsChanged);
	}

	Super::OnEquipped();
}

void URangedWeaponItemInstance::OnUnequipped()
{
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(World);
		MessageSystem.UnregisterListener(Handle_WeaponStatsChanged);
	}

	Super::OnUnequipped();
}

void URangedWeaponItemInstance::OnWeaponStatsChanged(const FGameplayTag Channel, const FWeaponStatsChangedMessage& Payload)
{
	if (Payload.Owner == this)
	{
		TimeLastFired = Payload.TimeLastFired;
		TimeLastEquipped = Payload.TimeLastEquipped;
		SpreadAngle = Payload.SpreadAngle;
		SpreadAngleMultiplier = Payload.SpreadAngleMultiplier;
		bHas1InTheChamber = Payload.bHas1InTheChamber;
	}
}
