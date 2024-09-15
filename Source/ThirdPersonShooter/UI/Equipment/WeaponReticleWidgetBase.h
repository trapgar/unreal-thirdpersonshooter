// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

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

	UFUNCTION(BlueprintImplementableEvent)
	void OnEquipmentInitialized();

	UFUNCTION(BlueprintCallable)
	void InitializeFromEquipment(UEquipmentItemInstance* InEquipment);

	/** Returns the current weapon's diametrical spread angle, in degrees */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Equipment|UI")
	float ComputeSpreadAngle() const;

	/** Returns the current weapon's maximum spread radius in screenspace units (pixels) */
	UFUNCTION(BlueprintCallable, BlueprintPure)
	float ComputeMaxScreenspaceSpreadRadius() const;

protected:
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UEquipmentItemInstance> EquipmentInstance;

	// UPROPERTY(BlueprintReadOnly)
	// TObjectPtr<ULyraInventoryItemInstance> InventoryInstance;
};
