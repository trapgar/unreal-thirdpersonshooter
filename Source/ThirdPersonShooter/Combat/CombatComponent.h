// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayAbilities/Attributes/PawnCombatSet.h"

#include "CombatComponent.generated.h"

/**
 * UCombatComponent
 * 
 * A component that handles various combat-related abilities.
 */
UCLASS(Blueprintable, Meta=(BlueprintSpawnableComponent))
class THIRDPERSONSHOOTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCombatComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

public:
	virtual void InitializeComponent() override
	{
		if (IAbilitySystemInterface* WithAbilities = Cast<IAbilitySystemInterface>(GetOwner()))
		{
			if (UAbilitySystemComponent* ASC = WithAbilities->GetAbilitySystemComponent())
			{
				ASC->AddSet<UPawnCombatSet>();
			}
		}
		else if (UAbilitySystemComponent* ASC = GetOwner()->GetComponentByClass<UAbilitySystemComponent>())
		{
			ASC->AddSet<UPawnCombatSet>();
		}

		Super::InitializeComponent();
	};

};
