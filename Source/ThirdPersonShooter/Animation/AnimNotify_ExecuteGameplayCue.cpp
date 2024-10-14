// Copyright Epic Games, Inc. All Rights Reserved.

#include "AnimNotify_ExecuteGameplayCue.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraSystem.h"
#include "GameplayCueFunctionLibrary.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(AnimNotify_ExecuteGameplayCue)

UAnimNotify_ExecuteGameplayCue::UAnimNotify_ExecuteGameplayCue()
{
}

void UAnimNotify_ExecuteGameplayCue::PostLoad()
{
	Super::PostLoad();
}

#if WITH_EDITOR
void UAnimNotify_ExecuteGameplayCue::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

FString UAnimNotify_ExecuteGameplayCue::GetNotifyName_Implementation() const
{
	// If the Effect Tag is valid, pass the string name to the notify name
	if (GameplayCueTag.IsValid())
	{
		return GameplayCueTag.ToString();
	}

	return Super::GetNotifyName_Implementation();
}

void UAnimNotify_ExecuteGameplayCue::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		// Make sure both MeshComp and Owning Actor is valid
		if (AActor* OwningActor = MeshComp->GetOwner())
		{
			// Prepare Trace Data
			bool bHitSuccess = false;
			FHitResult HitResult;
			FCollisionQueryParams QueryParams;

			if (TraceProperties.bIgnoreActor)
			{
				QueryParams.AddIgnoredActor(OwningActor);
			}

			QueryParams.bReturnPhysicalMaterial = true;

			if (bPerformTrace)
			{
				// If trace is needed, set up Start Location to Attached
				FVector TraceStart = MeshComp->GetComponentLocation();

				// Make sure World is valid
				if (UWorld* World = OwningActor->GetWorld())
				{
					// Call Line Trace, Pass in relevant properties
					bHitSuccess = World->LineTraceSingleByChannel(HitResult,
						TraceStart,
						(TraceStart + TraceProperties.EndTraceLocationOffset),
						TraceProperties.TraceChannel,
						QueryParams,
						FCollisionResponseParams::DefaultResponseParam
					);
				}
			}

			FGameplayCueParameters GCN_Params = UGameplayCueFunctionLibrary::MakeGameplayCueParametersFromHitResult(HitResult);
			GCN_Params.Instigator = OwningActor;
			// GCN_Params.EffectCauser = this;
			// GCN_Params.EffectContext = GE_Handle_Damage;

			// Burst to tell anyone listening we hit something
			UGameplayCueFunctionLibrary::ExecuteGameplayCueOnActor(
				OwningActor,
				GameplayCueTag,
				GCN_Params
			);
		}
	}
}

#if WITH_EDITOR
void UAnimNotify_ExecuteGameplayCue::ValidateAssociatedAssets()
{
	Super::ValidateAssociatedAssets();
}
#endif

