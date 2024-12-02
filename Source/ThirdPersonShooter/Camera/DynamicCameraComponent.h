// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Camera/CameraComponent.h"
#include "GameFramework/Actor.h"
#include "DynamicCameraMode_ThirdPerson.h"
#include "Abilities/GameplayAbility.h"
#include "GameplayAbilitySpecHandle.h"

#include "DynamicCameraComponent.generated.h"

class UCanvas;
class UDynamicCameraMode;
class UDynamicCameraModeStack;
class UObject;
struct FFrame;
struct FGameplayTag;
struct FMinimalViewInfo;
template <class TClass> class TSubclassOf;

DECLARE_DELEGATE_RetVal(TSubclassOf<UDynamicCameraMode>, FDynamicCameraModeDelegate);


/**
 * UDynamicCameraComponent
 *
 *	The base camera component class used by this project.
 */
UCLASS()
class UDynamicCameraComponent : public UCameraComponent
{
	GENERATED_BODY()

public:
	/** Camera mode set by an ability. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Camera Settings")
	TSubclassOf<UDynamicCameraMode> DefaultCameraMode;

public:

	UDynamicCameraComponent(const FObjectInitializer& ObjectInitializer);

	// Returns the camera component if one exists on the specified actor.
	UFUNCTION(BlueprintPure, Category = "Player|Camera")
	static UDynamicCameraComponent* FindCameraComponent(const AActor* Actor) { return (Actor ? Actor->FindComponentByClass<UDynamicCameraComponent>() : nullptr); }

	// Returns the target actor that the camera is looking at.
	virtual AActor* GetTargetActor() const { return GetOwner(); }

	// Delegate used to query for the best camera mode.
	FDynamicCameraModeDelegate DetermineCameraModeDelegate;

	// Add an offset to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	void AddFieldOfViewOffset(float FovOffset) { FieldOfViewOffset += FovOffset; }

	virtual void DrawDebug(UCanvas* Canvas) const;

	// Gets the tag associated with the top layer and the blend weight of it
	void GetBlendInfo(float& OutWeightOfTopLayer, FGameplayTag& OutTagOfTopLayer) const;

	UFUNCTION(BlueprintCallable, Category = "Player|Camera")
	void SetActiveCameraMode(TSubclassOf<UDynamicCameraMode> NewCameraMode, UGameplayAbility* Source)
	{
		const FGameplayAbilitySpecHandle OwningSpecHandle = Source->GetCurrentAbilitySpecHandle();
		if (NewCameraMode)
		{
			CurrentCameraMode = NewCameraMode;
			Handle_CurrentCameraMode = OwningSpecHandle;
		}
		else if (Handle_CurrentCameraMode == OwningSpecHandle)
		{
			CurrentCameraMode = nullptr;
			Handle_CurrentCameraMode = FGameplayAbilitySpecHandle();
		}
	};

	// @TODO: Load this from a config or save data
	TSubclassOf<UDynamicCameraMode> GetActiveCameraMode() const { return CurrentCameraMode ? CurrentCameraMode : DefaultCameraMode; };

protected:

	virtual void OnRegister() override;
	virtual void GetCameraView(float DeltaTime, FMinimalViewInfo& DesiredView) override;

	virtual void UpdateCameraModes();

protected:

	// Stack used to blend the camera modes.
	UPROPERTY()
	TObjectPtr<UDynamicCameraModeStack> CameraModeStack;

	// Offset applied to the field of view.  The offset is only for one frame, it gets cleared once it is applied.
	float FieldOfViewOffset;

private:
	/** Camera mode set by an ability. */
	UPROPERTY()
	TSubclassOf<UDynamicCameraMode> CurrentCameraMode;
	/** Spec handle for the last ability to set a camera mode. */
	FGameplayAbilitySpecHandle Handle_CurrentCameraMode;
};
