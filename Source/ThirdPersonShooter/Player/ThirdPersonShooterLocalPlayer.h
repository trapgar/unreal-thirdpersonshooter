// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CommonLocalPlayer.h"

#include "ThirdPersonShooterLocalPlayer.generated.h"

/**
 * UThirdPersonShooterLocalPlayer
 */
UCLASS()
class THIRDPERSONSHOOTER_API UThirdPersonShooterLocalPlayer : public UCommonLocalPlayer
{
	GENERATED_BODY()

public:

	UThirdPersonShooterLocalPlayer();

	/** Starts an async request to load the shared settings, this will call OnSharedSettingsLoaded after loading or creating new ones */
	void LoadSharedSettingsFromDisk(bool bForceLoad = false) {};
};
