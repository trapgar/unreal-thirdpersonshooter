// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicPlayerCameraManager.h"

#include "Async/TaskGraphInterfaces.h"
#include "Engine/Canvas.h"
#include "Engine/Engine.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "DynamicCameraComponent.h"
#include "DynamicUICameraManagerComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DynamicPlayerCameraManager)

class FDebugDisplayInfo;

static FName UICameraComponentName(TEXT("UICamera"));

ADynamicPlayerCameraManager::ADynamicPlayerCameraManager(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	DefaultFOV = THIRDPERSONSHOOTER_CAMERA_DEFAULT_FOV;
	ViewPitchMin = THIRDPERSONSHOOTER_CAMERA_DEFAULT_PITCH_MIN;
	ViewPitchMax = THIRDPERSONSHOOTER_CAMERA_DEFAULT_PITCH_MAX;

	UICamera = CreateDefaultSubobject<UDynamicUICameraManagerComponent>(UICameraComponentName);
}

UDynamicUICameraManagerComponent* ADynamicPlayerCameraManager::GetUICameraComponent() const
{
	return UICamera;
}

void ADynamicPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	// If the UI Camera is looking at something, let it have priority.
	if (UICamera->NeedsToUpdateViewTarget())
	{
		Super::UpdateViewTarget(OutVT, DeltaTime);
		UICamera->UpdateViewTarget(OutVT, DeltaTime);
		return;
	}

	Super::UpdateViewTarget(OutVT, DeltaTime);
}

void ADynamicPlayerCameraManager::DisplayDebug(UCanvas* Canvas, const FDebugDisplayInfo& DebugDisplay, float& YL, float& YPos)
{
	check(Canvas);

	FDisplayDebugManager& DisplayDebugManager = Canvas->DisplayDebugManager;

	DisplayDebugManager.SetFont(GEngine->GetSmallFont());
	DisplayDebugManager.SetDrawColor(FColor::Yellow);
	DisplayDebugManager.DrawString(FString::Printf(TEXT("DynamicPlayerCameraManager: %s"), *GetNameSafe(this)));

	Super::DisplayDebug(Canvas, DebugDisplay, YL, YPos);

	const APawn* Pawn = (PCOwner ? PCOwner->GetPawn() : nullptr);

	if (const UDynamicCameraComponent* CameraComponent = UDynamicCameraComponent::FindCameraComponent(Pawn))
	{
		CameraComponent->DrawDebug(Canvas);
	}
}

