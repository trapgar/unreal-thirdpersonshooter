// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatureAction_AddLevelInstance.h"

#include "Containers/UnrealString.h"
#include "CoreTypes.h"
#include "Delegates/Delegate.h"
#include "Engine/Engine.h"
#include "Engine/GameInstance.h"
#include "Engine/LevelStreaming.h"
#include "Engine/LevelStreamingDynamic.h"
#include "Engine/World.h"
#include "HAL/PlatformCrt.h"
#include "Internationalization/Internationalization.h"
#include "Internationalization/Text.h"
#include "Logging/LogCategory.h"
#include "Logging/LogMacros.h"
#include "Misc/AssertionMacros.h"
#include "Trace/Detail/Channel.h"
#include "UObject/ObjectPtr.h"
#include "UObject/UObjectBaseUtility.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

struct FGameFeatureDeactivatingContext;

#define LOCTEXT_NAMESPACE "GameFeatures"

// --------------------------------------------------------
// UGameFeatureAction_AddLevelInstance

void UGameFeatureAction_AddLevelInstance::OnGameFeatureLoading()
{
	Super::OnGameFeatureLoading();

	for (const FGameFeatureLevelInstanceEntry& Entry : LevelInstanceList)
	{
		const FSoftObjectPath& AssetPath = Entry.LevelInstance.ToSoftObjectPath();
		if (AssetPath.IsValid())
		{
			if (!Entry.LevelInstance.IsValid())
			{
				Entry.LevelInstance.LoadSynchronous();
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to load soft object reference `Entry.LevelInstance` '%s'. Level instance will not be added."), *AssetPath.ToString());
		}
		const FSoftObjectPath& AssetPath2 = Entry.TargetWorld.ToSoftObjectPath();
		if (AssetPath2.IsValid())
		{
			if (!Entry.TargetWorld.IsValid())
			{
				Entry.TargetWorld.LoadSynchronous();
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to load soft object reference `Entry.TargetWorld` '%s'. Level instance will not be added."), *AssetPath.ToString());
		}
	}
}

void UGameFeatureAction_AddLevelInstance::OnGameFeatureActivating(FGameFeatureActivatingContext &Context)
{
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddLevelInstance::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddLevelInstance::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);
	int32 EntryIndex = 0;

	for (const FGameFeatureLevelInstanceEntry& Entry : LevelInstanceList)
	{
		if (Entry.LevelInstance.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("LevelInstanceNull", "Null level instance reference at index {0} in LevelInstanceList"), FText::AsNumber(EntryIndex)));
		}
		if (Entry.TargetWorld.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("TargetWorldNull", "Null level reference at index {0} in LevelInstanceList"), FText::AsNumber(EntryIndex)));
		}

		++EntryIndex;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddLevelInstance::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;

	if ((World != nullptr) && World->IsGameWorld())
	{
		for (const FGameFeatureLevelInstanceEntry& Entry : LevelInstanceList)
		{
			if (!Entry.TargetWorld.IsNull())
			{
				UWorld* TargetWorld = Entry.TargetWorld.Get();
				if (TargetWorld == World)
				{
					if (TSubclassOf<AActor> LevelInstance = Entry.LevelInstance.Get())
					{
						World->SpawnActor<AActor>(LevelInstance, Entry.Location, Entry.Rotation);
					}
				}
			}
		}
	}
}

// --------------------------------------------------------

#undef LOCTEXT_NAMESPACE