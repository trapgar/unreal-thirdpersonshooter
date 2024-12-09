// Copyright Epic Games, Inc. All Rights Reserved.

#include "PawnCombatSet.h"

#include "PawnAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "Net/UnrealNetwork.h"
#include "CustomLogChannels.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PawnCombatSet)

class FLifetimeProperty;

UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Classification_Primary_Rifle, "Item.Classification.Primary.Rifle");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Classification_Primary_Shotgun, "Item.Classification.Primary.Shotgun");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Classification_Primary_Marksman, "Item.Classification.Primary.Marksman");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Classification_Primary_Heavy, "Item.Classification.Primary.Heavy");
UE_DEFINE_GAMEPLAY_TAG(TAG_Item_Classification_Primary_Sidearm, "Item.Classification.Primary.Sidearm");

UPawnCombatSet::UPawnCombatSet()
	: MaxAmmoStackCount_Rifle(90.0f)
	, MaxAmmoStackCount_Shotgun(72.0f)
	, MaxAmmoStackCount_Marksman(15.0f)
	, MaxAmmoStackCount_Heavy(6.0f)
	, MaxAmmoStackCount_Sidearm(24.0f)
	, SpreadAngleMultiplier_HipFire(1.0f)
{
}

void UPawnCombatSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, BaseDamageApplied, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, BaseHealingApplied, COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmunitionRefill, COND_OwnerOnly, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmunitionSpend, COND_OwnerOnly, REPNOTIFY_Always);

	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmoStackCount, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmoStackCount_Rifle, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmoStackCount_Shotgun, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmoStackCount_Marksman, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmoStackCount_Heavy, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, AmmoStackCount_Sidearm, COND_OwnerOnly, REPNOTIFY_OnChanged);
	DOREPLIFETIME_CONDITION_NOTIFY(UPawnCombatSet, SpreadAngleMultiplier_HipFire, COND_OwnerOnly, REPNOTIFY_OnChanged);
}

void UPawnCombatSet::OnRep_BaseDamageApplied(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, BaseDamageApplied, OldValue);
}

void UPawnCombatSet::OnRep_BaseHealingApplied(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, BaseHealingApplied, OldValue);
}

void UPawnCombatSet::OnRep_AmmunitionRefill(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmunitionRefill, OldValue);
}

void UPawnCombatSet::OnRep_AmmunitionSpend(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmunitionSpend, OldValue);
}

void UPawnCombatSet::OnRep_AmmoStackCount(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmoStackCount, OldValue);
}

void UPawnCombatSet::OnRep_AmmoStackCount_Rifle(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmoStackCount_Rifle, OldValue);
}

void UPawnCombatSet::OnRep_MaxAmmoStackCount_Rifle(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, MaxAmmoStackCount_Rifle, OldValue);
}

void UPawnCombatSet::OnRep_AmmoStackCount_Shotgun(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmoStackCount_Shotgun, OldValue);
}

void UPawnCombatSet::OnRep_MaxAmmoStackCount_Shotgun(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, MaxAmmoStackCount_Shotgun, OldValue);
}

void UPawnCombatSet::OnRep_AmmoStackCount_Marksman(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmoStackCount_Marksman, OldValue);
}

void UPawnCombatSet::OnRep_MaxAmmoStackCount_Marksman(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, MaxAmmoStackCount_Marksman, OldValue);
}

void UPawnCombatSet::OnRep_AmmoStackCount_Heavy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmoStackCount_Heavy, OldValue);
}

void UPawnCombatSet::OnRep_MaxAmmoStackCount_Heavy(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, MaxAmmoStackCount_Heavy, OldValue);
}

void UPawnCombatSet::OnRep_AmmoStackCount_Sidearm(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, AmmoStackCount_Sidearm, OldValue);
}

void UPawnCombatSet::OnRep_MaxAmmoStackCount_Sidearm(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, MaxAmmoStackCount_Sidearm, OldValue);
}

void UPawnCombatSet::OnRep_SpreadAngleMultiplier_HipFire(const FGameplayAttributeData& OldValue)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UPawnCombatSet, SpreadAngleMultiplier_HipFire, OldValue);
}

bool UPawnCombatSet::PreGameplayEffectExecute(FGameplayEffectModCallbackData &Data)
{
	if (!Super::PreGameplayEffectExecute(Data))
	{
		return false;
	}

	float& NewValue = Data.EvaluatedData.Magnitude;

	ClampAttribute(Data.EvaluatedData.Attribute, NewValue);

	if (NewValue == 0.0f)
	{
		return false;
	}

	return true;
}

void UPawnCombatSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	float& NewValue = Data.EvaluatedData.Magnitude;

	if (Data.EvaluatedData.Attribute == GetAmmunitionRefillAttribute())
	{
		NewValue = NewValue * -1;
	}

	// @TODO: Can broadcast a GameplayMessageSubsystem message here, but I don't really have any need right now
	if (Data.EvaluatedData.Attribute == GetAmmunitionRefillAttribute() || Data.EvaluatedData.Attribute == GetAmmunitionSpendAttribute())
	{
		const FGameplayTagContainer* SourceTags = Data.EffectSpec.CapturedSourceTags.GetAggregatedTags();
		FGameplayAttribute CalculatedAttribute = UPawnCombatSet::GetAttributeFromContainer(*SourceTags);
		NewValue = NewValue * -1;

		if (CalculatedAttribute == GetAmmoStackCount_RifleAttribute())
		{
			SetAmmoStackCount_Rifle(GetAmmoStackCount_Rifle() + NewValue);
		}
		else if (CalculatedAttribute == GetAmmoStackCount_ShotgunAttribute())
		{
			SetAmmoStackCount_Shotgun(GetAmmoStackCount_Shotgun() + NewValue);
		}
		else if (CalculatedAttribute == GetAmmoStackCount_MarksmanAttribute())
		{
			SetAmmoStackCount_Marksman(GetAmmoStackCount_Marksman() + NewValue);
		}
		else if (CalculatedAttribute == GetAmmoStackCount_HeavyAttribute())
		{
			SetAmmoStackCount_Heavy(GetAmmoStackCount_Heavy() + NewValue);
		}
		else if (CalculatedAttribute == GetAmmoStackCount_SidearmAttribute())
		{
			SetAmmoStackCount_Sidearm(GetAmmoStackCount_Sidearm() + NewValue);
		}
		else
		{
			// @TODO: can't figure out how to interpolate source tags
			UE_LOG(LogAbilities, Error, TEXT("ExecutionCalculationAmmunition: Source tags do not match any ammunition type."));
			return;
		}

		SetAmmoStackCount(0.0f);
		SetAmmunitionRefill(0.0f);
		SetAmmunitionSpend(0.0f);
	}

	if (Data.EvaluatedData.Attribute == GetAmmoStackCount_RifleAttribute())
	{
		SetAmmoStackCount_Rifle(GetAmmoStackCount_Rifle() + NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetAmmoStackCount_ShotgunAttribute())
	{
		SetAmmoStackCount_Shotgun(GetAmmoStackCount_Shotgun() + NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetAmmoStackCount_MarksmanAttribute())
	{
		SetAmmoStackCount_Marksman(GetAmmoStackCount_Marksman() + NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetAmmoStackCount_HeavyAttribute())
	{
		SetAmmoStackCount_Heavy(GetAmmoStackCount_Heavy() + NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetAmmoStackCount_SidearmAttribute())
	{
		SetAmmoStackCount_Sidearm(GetAmmoStackCount_Sidearm() + NewValue);
	}

	if (Data.EvaluatedData.Attribute == GetMaxAmmoStackCount_RifleAttribute())
	{
		SetMaxAmmoStackCount_Rifle(NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxAmmoStackCount_ShotgunAttribute())
	{
		SetMaxAmmoStackCount_Shotgun(NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxAmmoStackCount_MarksmanAttribute())
	{
		SetMaxAmmoStackCount_Marksman(NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxAmmoStackCount_HeavyAttribute())
	{
		SetMaxAmmoStackCount_Heavy(NewValue);
	}
	else if (Data.EvaluatedData.Attribute == GetMaxAmmoStackCount_SidearmAttribute())
	{
		SetMaxAmmoStackCount_Sidearm(NewValue);
	}
}

void UPawnCombatSet::PreAttributeBaseChange(const FGameplayAttribute& Attribute, float& NewValue) const
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPawnCombatSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeBaseChange(Attribute, NewValue);

	ClampAttribute(Attribute, NewValue);
}

void UPawnCombatSet::ClampAttribute(const FGameplayAttribute& Attribute, float& NewValue) const
{
	if (Attribute == GetAmmoStackCountAttribute())
	{
		NewValue = FMath::Min(NewValue, 0.0f);
	}

	if (Attribute == GetAmmoStackCount_RifleAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmoStackCount_Rifle());
	}
	else if (Attribute == GetAmmoStackCount_ShotgunAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmoStackCount_Shotgun());
	}
	else if (Attribute == GetAmmoStackCount_MarksmanAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmoStackCount_Marksman());
	}
	else if (Attribute == GetAmmoStackCount_HeavyAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmoStackCount_Heavy());
	}
	else if (Attribute == GetAmmoStackCount_SidearmAttribute())
	{
		NewValue = FMath::Clamp(NewValue, 0.0f, GetMaxAmmoStackCount_Sidearm());
	}

	if (Attribute == GetMaxAmmoStackCount_RifleAttribute())
	{
		NewValue = FMath::Min(0.0f, NewValue);
	}
	else if (Attribute == GetMaxAmmoStackCount_ShotgunAttribute())
	{
		NewValue = FMath::Min(0.0f, NewValue);
	}
	else if (Attribute == GetMaxAmmoStackCount_MarksmanAttribute())
	{
		NewValue = FMath::Min(0.0f, NewValue);
	}
	else if (Attribute == GetMaxAmmoStackCount_HeavyAttribute())
	{
		NewValue = FMath::Min(0.0f, NewValue);
	}
	else if (Attribute == GetMaxAmmoStackCount_SidearmAttribute())
	{
		NewValue = FMath::Min(0.0f, NewValue);
	}
}
