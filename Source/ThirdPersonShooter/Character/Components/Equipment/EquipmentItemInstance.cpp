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
	return Cast<APawn>(GetInstigator());
}

APawn *AEquipmentItemInstance::GetTypedPawn(TSubclassOf<APawn> PawnType) const
{
	APawn *Result = nullptr;
	if (UClass *ActualPawnType = PawnType)
	{
		if (GetInstigator()->IsA(ActualPawnType))
		{
			Result = Cast<APawn>(GetInstigator());
		}
	}
	return Result;
}

void AEquipmentItemInstance::OnReady()
{
	K2_OnReady();
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
	StatTags.AddStack(Tag, StackCount);
}

void AEquipmentItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	StatTags.RemoveStack(Tag, StackCount);
}

int32 AEquipmentItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags.GetStackCount(Tag);
}

bool AEquipmentItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.ContainsTag(Tag);
}

void AEquipmentItemInstance::SetItemDef(TSubclassOf<UEquipmentItemDefinition> InDef)
{
	ItemDef = InDef;
}
