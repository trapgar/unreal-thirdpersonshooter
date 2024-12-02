// Copyright Epic Games, Inc. All Rights Reserved.

#include "RangedWeaponStateComponent.h"

#include "Equipment/EquipmentManagerComponent.h"
#include "RangedWeaponItemInstance.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RangedWeaponStateComponent)

URangedWeaponStateComponent::URangedWeaponStateComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	SetIsReplicatedByDefault(true);

	PrimaryComponentTick.bStartWithTickEnabled = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void URangedWeaponStateComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (APawn* Pawn = GetPawn<APawn>())
	{
		if (UEquipmentManagerComponent* EquipmentManager = Pawn->FindComponentByClass<UEquipmentManagerComponent>())
		{
			if (URangedWeaponItemInstance* CurrentWeapon = Cast<URangedWeaponItemInstance>(EquipmentManager->GetFirstInstanceOfType(URangedWeaponItemInstance::StaticClass())))
			{
				CurrentWeapon->Tick(DeltaTime);
			}
		}
	}
}

