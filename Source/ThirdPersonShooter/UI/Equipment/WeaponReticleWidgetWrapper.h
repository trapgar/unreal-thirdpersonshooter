// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonUserWidget.h"

#include "WeaponReticleWidgetWrapper.generated.h"

class UInventoryItemInstance;
class UEquipmentItemInstance;
class UObject;
struct FFrame;

UCLASS(Abstract)
class UWeaponReticleWidgetWrapper : public UCommonUserWidget
{
	GENERATED_BODY()

public:
	UWeaponReticleWidgetWrapper(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

};
