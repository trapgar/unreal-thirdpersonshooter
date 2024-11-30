// Copyright Epic Games, Inc. All Rights Reserved.

#include "BoundAnimInstance.h"
#include "AbilitySystemGlobals.h"
#include "Character/ThirdPersonShooterCharacter.h"
#include "Character/ThirdPersonShooterCharacterMovementComponent.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(BoundAnimInstance)


UBoundAnimInstance::UBoundAnimInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UBoundAnimInstance::InitializeWithAbilitySystem(UAbilitySystemComponent* ASC)
{
	check(ASC);

	GameplayTagPropertyMap.Initialize(this, ASC);
}

#if WITH_EDITOR
EDataValidationResult UBoundAnimInstance::IsDataValid(FDataValidationContext& Context) const
{
	Super::IsDataValid(Context);

	GameplayTagPropertyMap.IsDataValid(this, Context);

	return ((Context.GetNumErrors() > 0) ? EDataValidationResult::Invalid : EDataValidationResult::Valid);
}
#endif // WITH_EDITOR

void UBoundAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	if (AActor* OwningActor = GetOwningActor())
	{
		if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningActor))
		{
			InitializeWithAbilitySystem(ASC);
		}
	}
}

void UBoundAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (const AThirdPersonShooterCharacter* Character = Cast<AThirdPersonShooterCharacter>(GetOwningActor()))
	{
		if (UCharacterMovementComponent* CharMoveComp = Character->GetCharacterMovement())
		{
			if (UThirdPersonShooterCharacterMovementComponent* CharMoveCompWithGroundInfo = Cast<UThirdPersonShooterCharacterMovementComponent>(CharMoveComp))
			{
				const FModularCharacterGroundInfo& GroundInfo = CharMoveCompWithGroundInfo->GetGroundInfo();
				GroundDistance = GroundInfo.GroundDistance;
			}
		}
	}
}

