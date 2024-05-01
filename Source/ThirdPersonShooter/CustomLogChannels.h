// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Logging/LogMacros.h"
#include "AbilitySystemGlobals.h"

class UObject;

THIRDPERSONSHOOTER_API DECLARE_LOG_CATEGORY_EXTERN(LogExperience, Log, All);
THIRDPERSONSHOOTER_API DECLARE_LOG_CATEGORY_EXTERN(LogAbilities, Log, All);

THIRDPERSONSHOOTER_API FString GetClientServerContextString(UObject* ContextObject = nullptr);
