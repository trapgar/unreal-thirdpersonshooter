// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Containers/Array.h"
#include "GameFeatureAction_WorldActionBase.h"
#include "Math/MathFwd.h"
#include "Math/Rotator.h"
#include "Math/Vector.h"
#include "UObject/SoftObjectPtr.h"
#include "UObject/UObjectGlobals.h"

#include "GameFeatureAction_AddLevelAsStreamable.generated.h"

class FText;
class ULevelStreamingDynamic;
class UObject;
class UWorld;
struct FFrame;
struct FGameFeatureDeactivatingContext;
struct FWorldContext;

// Description of a level to add to the world when this game feature is enabled
USTRUCT()
struct FGameFeatureLevelAsStreamableEntry
{
	GENERATED_BODY()

	// The level instance to dynamically load at runtime.
	UPROPERTY(EditAnywhere, Category="Instance Info")
	TSoftObjectPtr<UWorld> Level;

	// Specific world to load into. If left null, this level will be loaded for all worlds.
	UPROPERTY(EditAnywhere, Category = "Instance Info")
	TSoftObjectPtr<UWorld> TargetWorld;

	// The translational offset for this level instance.
	UPROPERTY(EditAnywhere, Category="Instance Info")
	FVector Location = FVector(0.f);
	
	// The rotational tranform for this level instance. 
	UPROPERTY(EditAnywhere, Category="Instance Info")
	FRotator Rotation = FRotator(0.f);
};	

// --------------------------------------------------------
// UGameFeatureAction_AddLevelAsStreamable

/**
 * UGameFeatureAction_AddLevelAsStreamable
 * 
 * Loads specified level instances at runtime.
 * 
 * TODO: Pulled from the Unreal sample project ValleyoftheAncient. They called it `AddLevelInstance`, but it actually required a UWorld instead of an ALevelInstance.
 * So I changed it renamed it to `AddLevelAsStreamable` so I could use the same name for a GFA that actually loads level instances.
 * 
 * However, I don't think the AddLevelInstance uses streaming, which I may want to investigate.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Level As Streamable"))
class UGameFeatureAction_AddLevelAsStreamable final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

	/** List of levels to dynamically load when this game feature is enabled */
	UPROPERTY(EditAnywhere, Category="Levels", meta=(TitleProperty="Level", ShowOnlyInnerProperties))
	TArray<FGameFeatureLevelAsStreamableEntry> LevelInstanceList;

private:
	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	void OnWorldCleanup(UWorld* World, bool bSessionEnded, bool bCleanupResources);
	
	ULevelStreamingDynamic* LoadDynamicLevelForEntry(const FGameFeatureLevelAsStreamableEntry& Entry, UWorld* TargetWorld);	

	UFUNCTION() // UFunction so we can bind to a dynamic delegate
	void OnLevelLoaded();

	void DestroyAddedLevels();
	void CleanUpAddedLevel(ULevelStreamingDynamic* Level);

private:
	UPROPERTY(transient)
	TArray<ULevelStreamingDynamic*> AddedLevels;

	bool bIsActivated = false;
	bool bLayerStateReentrantGuard = false;
};
