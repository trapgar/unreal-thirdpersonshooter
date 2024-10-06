// Copyright Epic Games, Inc. All Rights Reserved.

#include "Equipment/EquipmentItemInstance.h"

#include "Components/SkeletalMeshComponent.h"
#include "Components/GameFrameworkComponentManager.h"
#include "GameFramework/Character.h"
#include "Equipment/EquipmentItemDefinition.h"
#include "Camera/CameraComponent.h"
#include "GameplayTags.h"
#include "Net/UnrealNetwork.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentItemInstance)

#define LOCTEXT_NAMESPACE "Equipment"

class FLifetimeProperty;
class UClass;
class USceneComponent;


UEquipmentItemInstance::UEquipmentItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StatTags(FGameplayTagStackContainer())
{
}

#if WITH_EDITOR
EDataValidationResult UEquipmentItemInstance::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	return Result;
}
#endif

void UEquipmentItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
	DOREPLIFETIME(ThisClass, Instigator);
	DOREPLIFETIME(ThisClass, SpawnedActors);
}

void UEquipmentItemInstance::OnRep_Instigator()
{
}

#undef LOCTEXT_NAMESPACE
