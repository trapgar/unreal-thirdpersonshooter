// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/ControllerComponent.h"

#include "RangedWeaponStateComponent.generated.h"

// Tracks weapon state and recent confirmed hit markers to display on screen
UCLASS()
class URangedWeaponStateComponent : public UControllerComponent
{
	GENERATED_BODY()

public:

	URangedWeaponStateComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

};
