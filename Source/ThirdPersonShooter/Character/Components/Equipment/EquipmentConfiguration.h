// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"

#include "EquipmentConfiguration.generated.h"

class UInputAction;
class UObject;
struct FFrame;

/**
 * FEquipmentConfigurationEntry
 *
 *	Struct used to map a Gameplay Tag to a Skeletal socket name.
 */
USTRUCT(BlueprintType)
struct FEquipmentConfigurationEntry
{
	GENERATED_BODY()

public:

	// The Gameplay Tag of the socket
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "Equipment.Socket"))
	FGameplayTag SocketTag;

	// The name of the socket on the skeleton.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SocketName;
};

/**
 * UEquipmentConfiguration
 *
 *	Non-mutable data asset that contains input configuration properties.
 *	This is where we define the relationship between input tags and input actions that GameplayAbilities will use.
 */
UCLASS(BlueprintType, Const)
class UEquipmentConfiguration : public UDataAsset
{
	GENERATED_BODY()

public:

	UEquipmentConfiguration(const FObjectInitializer& ObjectInitializer);

public:
	// List of Socket tags and their corresponding Skeletal socket names. This is mostly to limit the number of times a socket name must be spelled out.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FEquipmentConfigurationEntry> Mappings;
};
