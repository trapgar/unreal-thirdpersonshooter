#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraModifier.h"
#include "CameraModifierWeaponFired.generated.h"

UCLASS(Blueprintable, BlueprintType)
class THIRDPERSONSHOOTER_API UCameraModifierWeaponFired : public UCameraModifier
{
    GENERATED_BODY()

public:

    UCameraModifierWeaponFired();

    /** Rotation applied while active. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Shake")
    FRotator Rotation = FRotator(0.f, 0.f, 0.f);

    /** Total duration in seconds. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Shake", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="s"))
    float Duration = 0.10f;

    /** Blend in time. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Shake", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="s"))
    float BlendInTime = 0.0f;

    /** Blend out time. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Camera Shake", meta=(ClampMin=0.0f, UIMin=0.0f, ForceUnits="s"))
    float BlendOutTime = 0.05f;

    UFUNCTION(BlueprintCallable, Category="Camera Shake")
    void StartShake();

    UFUNCTION(BlueprintCallable, Category="Camera Shake")
    void StopShake();

protected:

    virtual bool ModifyCamera(
        float DeltaTime,
        FMinimalViewInfo& InOutPOV) override;

private:

    bool bPlaying = false;
    float ElapsedTime = 0.f;
};
