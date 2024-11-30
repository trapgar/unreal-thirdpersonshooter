// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Animation/AnimNotifies/AnimNotify.h"
#include "Chaos/ChaosEngineInterface.h"
#include "GameplayTagContainer.h"
#include "Engine/EngineTypes.h"
#include "AnimNotify_ExecuteGameplayCue.generated.h"


/**
 *
 */
USTRUCT(BlueprintType)
struct THIRDPERSONSHOOTER_API FAnimNotifyGameplayCueTraceSettings
{
	GENERATED_BODY()

	// Trace Channel
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace)
	TEnumAsByte<ECollisionChannel> TraceChannel = ECollisionChannel::ECC_Visibility;

	// Vector offset from Effect Location
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace)
	FVector EndTraceLocationOffset = FVector::ZeroVector;

	// Ignore this Actor when getting trace result
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Trace)
	bool bIgnoreActor = true;
};


// --------------------------------------------------------


/**
 * 
 */
UCLASS(const, HideCategories=Object, CollapseCategories, Config = Game, meta=(DisplayName="Execute GameplayCue"))
class THIRDPERSONSHOOTER_API UAnimNotify_ExecuteGameplayCue : public UAnimNotify
{
	GENERATED_BODY()

public:
	UAnimNotify_ExecuteGameplayCue();

	// Begin UObject interface
	virtual void PostLoad() override;
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
	// End UObject interface

	// Begin UAnimNotify interface
	virtual FString GetNotifyName_Implementation() const override;
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
#if WITH_EDITOR
	virtual void ValidateAssociatedAssets() override;
#endif
	// End UAnimNotify interface

	// Tag this notify triggers
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (DisplayName = "Gameplay Cue Tag", Categories = "GameplayCue", ExposeOnSpawn = true))
	FGameplayTag GameplayCueTag;

	// Will perform a trace, required for SurfaceType to Context Conversion
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true))
	bool bPerformTrace = true;

	// Scale to spawn the particle system at
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AnimNotify", meta = (ExposeOnSpawn = true, EditCondition = "bPerformTrace"))
	FAnimNotifyGameplayCueTraceSettings TraceProperties;

};
