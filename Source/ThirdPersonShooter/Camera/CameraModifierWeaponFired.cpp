#include "CameraModifierWeaponFired.h"

UCameraModifierWeaponFired::UCameraModifierWeaponFired()
{
    Priority = 127;
}

void UCameraModifierWeaponFired::StartShake()
{
    ElapsedTime = 0.f;
    bPlaying = true;
}

void UCameraModifierWeaponFired::StopShake()
{
    bPlaying = false;
}

bool UCameraModifierWeaponFired::ModifyCamera(
    float DeltaTime,
    FMinimalViewInfo& InOutPOV)
{
    if (!bPlaying)
    {
        return false;
    }

    ElapsedTime += DeltaTime;

    if (ElapsedTime >= Duration)
    {
        bPlaying = false;
        return false;
    }

    float AlphaInternal = 1.f;

    if (BlendInTime > 0.f && ElapsedTime < BlendInTime)
    {
        AlphaInternal = ElapsedTime / BlendInTime;
    }

    const float Remaining = Duration - ElapsedTime;

    if (BlendOutTime > 0.f && Remaining < BlendOutTime)
    {
        AlphaInternal *= Remaining / BlendOutTime;
    }

    InOutPOV.Rotation += Rotation * AlphaInternal;

    return false;
}
