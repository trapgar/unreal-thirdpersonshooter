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
	// Let others know a reset has occurred
	// FLyraPlayerResetMessage Message;
	// Message.OwnerPlayerState = CurrentActorInfo->OwnerActor.Get();
	// UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	// MessageSystem.BroadcastMessage(LyraGameplayTags::GameplayEvent_Reset, Message);

	K2_OnEquipped();
}

void AEquipmentItemInstance::OnUnequipped()
{
	K2_OnUnequipped();
}
