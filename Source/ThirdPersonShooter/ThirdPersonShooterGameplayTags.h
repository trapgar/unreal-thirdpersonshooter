// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "NativeGameplayTags.h"

namespace ThirdPersonShooterGameplayTags
{
	THIRDPERSONSHOOTER_API	FGameplayTag FindTagByString(const FString& TagString, bool bMatchPartialString = false);

	// Declare all of the custom native tags that this game will use
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_IsDead);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cooldown);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Cost);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsBlocked);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_TagsMissing);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_Networking);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_ActivateFail_ActivationGroup);

	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Ability_Behavior_SurvivesDeath);

	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Move);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Mouse);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Look_Stick);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Crouch);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InputTag_Sprint);

	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_Spawned);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataAvailable);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_DataInitialized);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(InitState_GameplayReady);

	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Death);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_Reset);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(GameplayEvent_RequestReset);

	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Damage);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(SetByCaller_Healing);

	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Crouching);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Sprinting);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dying);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Status_Death_Dead);

	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Walking);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Falling);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Swimming);
	THIRDPERSONSHOOTER_API	UE_DECLARE_GAMEPLAY_TAG_EXTERN(Movement_Mode_Flying);
};
