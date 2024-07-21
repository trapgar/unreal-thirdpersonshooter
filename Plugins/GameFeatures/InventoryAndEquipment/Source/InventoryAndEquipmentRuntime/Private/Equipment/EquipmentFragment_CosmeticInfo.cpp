#include "Equipment/EquipmentFragment_CosmeticInfo.h"

#include "Equipment/EquipmentItemInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_CosmeticInfo)

#define LOCTEXT_NAMESPACE "Equipment"

#if WITH_EDITOR
EDataValidationResult UEquipmentFragment_CosmeticInfo::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	return Result;
}
#endif

void UEquipmentFragment_CosmeticInfo::OnInstanceCreated(UEquipmentItemInstance* Instance) const
{
	if (APawn* OwningPawn = Instance->GetInstigator())
	{
		if (LifeTime == EEquipmentCosmeticInfoLifeTime::WhileInUse)
		{
			USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
			if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
			{
				AttachTarget = Char->GetMesh();
			}

			AActor* NewActor = OwningPawn->GetWorld()->SpawnActorDeferred<AActor>(ActorToSpawn, FTransform::Identity, OwningPawn, OwningPawn);
			NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
			NewActor->SetActorRelativeTransform(AttachTransform);
			NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, AttachSocketName);

			Instance->AddSpawnedActor(NewActor);
		}
	}
}

void UEquipmentFragment_CosmeticInfo::OnInstanceDestroyed(UEquipmentItemInstance* Instance) const
{
	// TODO: BAD - need to figure out who should track & destroy the spawned actors
	for (AActor* Actor : Instance->GetSpawnedActors())
	{
		Actor->Destroy();
	}
}
