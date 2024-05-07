// Copyright Epic Games, Inc. All Rights Reserved.

#include "EquipmentItemInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Character.h"
#include "EquipmentItemDefinition.h"
#include "Camera/CameraComponent.h"
#include "GameplayTags.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentItemInstance)

class FLifetimeProperty;
class UClass;
class USceneComponent;

void AEquipmentItemInstance::PreInitializeComponents()
{
	Super::PreInitializeComponents();
	UGameFrameworkComponentManager::AddGameFrameworkComponentReceiver(this);
}

void AEquipmentItemInstance::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameFrameworkComponentManager::RemoveGameFrameworkComponentReceiver(this);
	Super::EndPlay(EndPlayReason);
}

APawn *AEquipmentItemInstance::GetPawn() const
{
	return Cast<APawn>(GetOuter());
}

APawn *AEquipmentItemInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn *Result = nullptr;
	if (UClass *ActualPawnType = PawnType)
	{
		if (GetOuter()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetOuter());
		}
	}
	return Result;
}

void AEquipmentItemInstance::OnEquipped()
{
	K2_OnEquipped();
}

void AEquipmentItemInstance::OnUnequipped()
{
	K2_OnUnequipped();
}

void AEquipmentItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (StatTags.Contains(Tag))
	{
		StatTags.Add(Tag, StatTags[Tag] + StackCount);
	}
	else
	{
		StatTags.Add(Tag, StackCount);
	}
}

void AEquipmentItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (StatTags.Contains(Tag) && StatTags[Tag] - StackCount > 0)
	{
		StatTags.Add(Tag, StatTags[Tag] - StackCount);
	}
	else
	{
		StatTags.Remove(Tag);
	}
}

int32 AEquipmentItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	if (StatTags.Contains(Tag))
	{
		return StatTags[Tag];
	}
	else
	{
		return 0;
	}
}

bool AEquipmentItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.Contains(Tag);
}

void AEquipmentItemInstance::SetItemDef(TSubclassOf<UEquipmentItemDefinition> InDef)
{
	ItemDef = InDef;
}
