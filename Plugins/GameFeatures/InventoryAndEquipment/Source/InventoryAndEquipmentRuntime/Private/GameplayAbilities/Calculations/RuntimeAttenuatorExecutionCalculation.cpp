// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayAbilities/Calculations/RuntimeAttenuatorExecutionCalculation.h"
#include "GameplayAbilities/Attributes/PawnHealthSet.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"
#include "GameplayAbilities/ModularGameplayEffectContext.h"
#include "GameplayAbilities/ModularAbilityAttenuatorInterface.h"
#include "Engine/World.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(RuntimeAttenuatorExecutionCalculation)

struct FDamageStatics
{
	FGameplayEffectAttributeCaptureDefinition DamageDef;

	FDamageStatics()
	{
		DamageDef = FGameplayEffectAttributeCaptureDefinition(UPawnCombatSet::GetDamageAttribute(), EGameplayEffectAttributeCaptureSource::Source, true);
	}
};

static FDamageStatics& DamageStatics()
{
	static FDamageStatics Statics;
	return Statics;
}


URuntimeAttenuatorExecutionCalculation::URuntimeAttenuatorExecutionCalculation()
{
	RelevantAttributesToCapture.Add(DamageStatics().DamageDef);
}

void URuntimeAttenuatorExecutionCalculation::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams, FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
#if WITH_SERVER_CODE
	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	FModularGameplayEffectContext* TypedContext = FModularGameplayEffectContext::ExtractEffectContext(Spec.GetContext());
	check(TypedContext);

	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;

	float BaseDamage = Spec.GetLevel();
	const AActor* EffectCauser = TypedContext->GetEffectCauser();
	const FHitResult* HitActorResult = TypedContext->GetHitResult();

	AActor* HitActor = nullptr;
	FVector ImpactLocation = FVector::ZeroVector;
	FVector ImpactNormal = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;
	FVector EndTrace = FVector::ZeroVector;

	// Calculation of hit actor, surface, zone, and distance all rely on whether the calculation has a hit result or not.
	// Effects just being added directly w/o having been targeted will always come in without a hit result, which must default
	// to some fallback information.
	if (HitActorResult)
	{
		const FHitResult& CurHitResult = *HitActorResult;
		HitActor = CurHitResult.HitObjectHandle.FetchActor();
		if (HitActor)
		{
			ImpactLocation = CurHitResult.ImpactPoint;
			ImpactNormal = CurHitResult.ImpactNormal;
			StartTrace = CurHitResult.TraceStart;
			EndTrace = CurHitResult.TraceEnd;
		}
	}

	// Handle case of no hit result or hit result not actually returning an actor
	UAbilitySystemComponent* TargetAbilitySystemComponent = ExecutionParams.GetTargetAbilitySystemComponent();
	if (!HitActor)
	{
		HitActor = TargetAbilitySystemComponent ? TargetAbilitySystemComponent->GetAvatarActor_Direct() : nullptr;
		if (HitActor)
		{
			ImpactLocation = HitActor->GetActorLocation();
		}
	}

	// Apply rules for team damage/self damage/etc...
	float DamageInteractionAllowedMultiplier = 1.0f;

	// Determine distance
	double Distance = WORLD_MAX;

	if (TypedContext->HasOrigin())
	{
		Distance = FVector::Dist(TypedContext->GetOrigin(), ImpactLocation);
	}
	else if (EffectCauser)
	{
		Distance = FVector::Dist(EffectCauser->GetActorLocation(), ImpactLocation);
	}
	else
	{
		ensureMsgf(false, TEXT("Damage Calculation cannot deduce a source location for damage coming from %s; Falling back to WORLD_MAX dist!"), *GetPathNameSafe(Spec.Def));
	}

	// Apply ability source modifiers
	float PhysicalMaterialAttenuation = 1.0f;
	float DistanceAttenuation = 1.0f;
	if (const IModularAbilityAttenuatorInterface* AbilitySource = TypedContext->GetAbilitySource())
	{
		if (const UPhysicalMaterial* PhysMat = TypedContext->GetPhysicalMaterial())
		{
			PhysicalMaterialAttenuation = AbilitySource->GetPhysicalMaterialAttenuation(PhysMat, SourceTags, TargetTags);
		}

		DistanceAttenuation = AbilitySource->GetDistanceAttenuation(Distance, SourceTags, TargetTags);
	}
	DistanceAttenuation = FMath::Max(DistanceAttenuation, 0.0f);

	// Clamping is done when damage is converted to -health
	const float DamageDone = FMath::Max(BaseDamage * DistanceAttenuation * PhysicalMaterialAttenuation * DamageInteractionAllowedMultiplier, 0.0f);

	if (DamageDone > 0.0f)
	{
		// Apply a damage modifier, this gets turned into - health on the target
		OutExecutionOutput.AddOutputModifier(FGameplayModifierEvaluatedData(UPawnHealthSet::GetDamageAttribute(), EGameplayModOp::Additive, DamageDone));
	}
#endif // #if WITH_SERVER_CODE
}
