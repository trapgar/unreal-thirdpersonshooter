// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"
#include "GameplayTagContainer.h"

#include "WeaponReticleWidgetBase.generated.h"

class UInventoryItemInstance;
class UEquipmentItemInstance;
class UObject;
struct FFrame;

UCLASS(Abstract)
class UWeaponReticleWidgetBase : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UWeaponReticleWidgetBase(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UFUNCTION(BlueprintCallable)
	void InitializeFromEquipment(UEquipmentItemInstance* InEquipment);

	UFUNCTION(BlueprintImplementableEvent)
	void OnEquipmentInitialized();

	// Called when the current player either starts or stops aiming down sights
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="OnAimDownSight"))
	void K2_OnAimDownSight(bool bAimDownSight);

	// Called when the current player has damaged another player
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="OnDamage"))
	void K2_OnDamage(float Amount, FGameplayTagContainer AssociatedTags);

	// Called when the current player has eliminated another player
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, meta=(DisplayName="OnEliminate"))
	void K2_OnEliminate();

	/** Returns the current weapon's diametrical spread angle, in degrees */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Equipment|UI")
	float ComputeSpreadAngle() const;

	/** Returns the current weapon's maximum spread radius in screenspace units (pixels) */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Equipment|UI")
	float ComputeMaxScreenspaceSpreadRadius() const;

private:
	TObjectPtr<UEquipmentItemInstance> EquipmentItemInstance;
};
