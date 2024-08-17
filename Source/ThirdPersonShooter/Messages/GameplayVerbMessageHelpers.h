// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"

#include "GameplayVerbMessageHelpers.generated.h"

struct FGameplayCueParameters;
struct FGameplayVerbMessage;

class APlayerController;
class APlayerState;
class UObject;
struct FFrame;


UCLASS()
class THIRDPERSONSHOOTER_API UGameplayVerbMessageHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Gameplay|Messages")
	static APlayerState* GetPlayerStateFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Messages")
	static APlayerController* GetPlayerControllerFromObject(UObject* Object);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Messages")
	static FGameplayCueParameters VerbMessageToCueParameters(const FGameplayVerbMessage& Message);

	UFUNCTION(BlueprintCallable, Category = "Gameplay|Messages")
	static FGameplayVerbMessage CueParametersToVerbMessage(const FGameplayCueParameters& Params);
};
