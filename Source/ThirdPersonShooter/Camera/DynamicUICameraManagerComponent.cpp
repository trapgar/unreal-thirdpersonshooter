// Copyright Epic Games, Inc. All Rights Reserved.

#include "DynamicUICameraManagerComponent.h"

#include "GameFramework/HUD.h"
#include "GameFramework/PlayerController.h"
#include "DynamicPlayerCameraManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(DynamicUICameraManagerComponent)

class AActor;
class FDebugDisplayInfo;

UDynamicUICameraManagerComponent* UDynamicUICameraManagerComponent::GetComponent(APlayerController* PC)
{
	if (PC != nullptr)
	{
		if (ADynamicPlayerCameraManager* PCCamera = Cast<ADynamicPlayerCameraManager>(PC->PlayerCameraManager))
		{
			return PCCamera->GetUICameraComponent();
		}
	}

	return nullptr;
}

UDynamicUICameraManagerComponent::UDynamicUICameraManagerComponent()
{
	bWantsInitializeComponent = true;

	if (!HasAnyFlags(RF_ClassDefaultObject))
	{
		// Register "showdebug" hook.
		if (!IsRunningDedicatedServer())
		{
			AHUD::OnShowDebugInfo.AddUObject(this, &ThisClass::OnShowDebugInfo);
		}
	}
}

void UDynamicUICameraManagerComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UDynamicUICameraManagerComponent::SetViewTarget(AActor* InViewTarget, FViewTargetTransitionParams TransitionParams)
{
	TGuardValue<bool> UpdatingViewTargetGuard(bUpdatingViewTarget, true);

	ViewTarget = InViewTarget;
	CastChecked<ADynamicPlayerCameraManager>(GetOwner())->SetViewTarget(ViewTarget, TransitionParams);
}

bool UDynamicUICameraManagerComponent::NeedsToUpdateViewTarget() const
{
	return false;
}

void UDynamicUICameraManagerComponent::UpdateViewTarget(struct FTViewTarget& OutVT, float DeltaTime)
{
}

void UDynamicUICameraManagerComponent::OnShowDebugInfo(AHUD* HUD, UCanvas* Canvas, const FDebugDisplayInfo& DisplayInfo, float& YL, float& YPos)
{
}
