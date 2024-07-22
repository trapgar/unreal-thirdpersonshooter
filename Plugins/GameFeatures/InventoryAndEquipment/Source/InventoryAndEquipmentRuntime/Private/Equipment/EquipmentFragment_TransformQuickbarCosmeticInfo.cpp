#include "Equipment/EquipmentFragment_TransformQuickbarCosmeticInfo.h"

#include "Equipment/EquipmentItemInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(EquipmentFragment_TransformQuickbarCosmeticInfo)

#define LOCTEXT_NAMESPACE "Equipment"

#if WITH_EDITOR
EDataValidationResult UEquipmentFragment_TransformQuickbarCosmeticInfo::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (ActorToFind == nullptr)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(LOCTEXT("EntryHasNullActor", "Null ActorToFind"));
	}

	return Result;
}
#endif

void UEquipmentFragment_TransformQuickbarCosmeticInfo::OnInstanceCreated(UEquipmentItemInstance* Instance) const
{
	if (UInventoryItemInstance* Item = Instance->GetAssociatedItem())
	{
		// If the inventory item DOESN'T have an actor of the specified type, spawn it
		if (AActor* Actor = Item->FindSpawnedActorByClass(ActorToFind)) {}
		else
		{
			if (APawn* OwningPawn = Instance->GetInstigator())
			{
				USceneComponent* AttachTarget = OwningPawn->GetRootComponent();
				if (ACharacter* Char = Cast<ACharacter>(OwningPawn))
				{
					AttachTarget = Char->GetMesh();
				}

				AActor* NewActor = OwningPawn->GetWorld()->SpawnActorDeferred<AActor>(ActorToFind, FTransform::Identity, OwningPawn, OwningPawn);
				NewActor->FinishSpawning(FTransform::Identity, /*bIsDefaultTransform=*/ true);
				NewActor->SetActorRelativeTransform(AttachTransform);
				NewActor->AttachToComponent(AttachTarget, FAttachmentTransformRules::KeepRelativeTransform, AttachSocketName);

				Instance->AddSpawnedActor(NewActor);
			}
		}
	}
}

// void UEquipmentFragment_TransformQuickbarCosmeticInfo::OnInstanceDestroyed(UEquipmentItemInstance* Instance) const
// {
// }
