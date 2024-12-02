// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/PlayerCameraManager.h"

#include "DynamicPlayerCameraManager.generated.h"

class FDebugDisplayInfo;
class UCanvas;
class UObject;


#define THIRDPERSONSHOOTER_CAMERA_DEFAULT_FOV			(80.0f)
#define THIRDPERSONSHOOTER_CAMERA_DEFAULT_PITCH_MIN	(-89.0f)
#define THIRDPERSONSHOOTER_CAMERA_DEFAULT_PITCH_MAX	(89.0f)

class UDynamicUICameraManagerComponent;

/**
 * ADynamicPlayerCameraManager
 *
 *	The base player camera manager class used by this project.
 */
UCLASS(notplaceable, MinimalAPI)
class ADynamicPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:

	ADynamicPlayerCameraManager(const FObjectInitializer& ObjectInitializer);

	UDynamicUICameraManagerComponent* GetUICameraComponent() const;

protected:

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;

	virtual void DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos) override;

private:
	/** The UI Camera Component, controls the camera when UI is doing something important that gameplay doesn't get priority over. */
	UPROPERTY(Transient)
	TObjectPtr<UDynamicUICameraManagerComponent> UICamera;
};
