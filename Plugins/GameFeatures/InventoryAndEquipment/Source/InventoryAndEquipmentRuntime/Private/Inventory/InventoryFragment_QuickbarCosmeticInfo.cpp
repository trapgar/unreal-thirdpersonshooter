#include "Inventory/InventoryFragment_QuickbarCosmeticInfo.h"

#include "Inventory/InventoryItemInstance.h"
#include "Components/SkeletalMeshComponent.h"
#include "GameFramework/Character.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryFragment_QuickbarCosmeticInfo)

#define LOCTEXT_NAMESPACE "Inventory"

#if WITH_EDITOR
EDataValidationResult UInventoryFragment_QuickbarCosmeticInfo::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	if (ActorToSpawn == nullptr)
	{
		Result = EDataValidationResult::Invalid;
		Context.AddError(FText::Format(LOCTEXT("EntryHasNullActor", "Null ActorToSpawn")));
	}

	return Result;
}
#endif

void UInventoryFragment_QuickbarCosmeticInfo::OnInstanceAdded(UInventoryItemInstance* Instance)
{
	if (APawn* OwningPawn = Instance->GetInstigator())
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
