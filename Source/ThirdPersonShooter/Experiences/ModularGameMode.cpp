#include "ModularGameMode.h"
#include "Character/ModularPlayerController.h"
#include "Character/ModularCharacter.h"
#include "GameFeaturesSubsystem.h"

#include "CustomLogChannels.h"

AModularGameMode::AModularGameMode(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AModularPlayerController::StaticClass();
	DefaultPawnClass = AModularCharacter::StaticClass();
}

void AModularGameMode::BeginPlay()
{
	if (Actions.Num() > 0)
	{
		// TODO: properly mock the context so that all the assets are actually loaded
		FGameFeatureActivatingContext Context = FGameFeatureActivatingContext();
		for (UGameFeatureAction* Action : Actions)
		{
			Action->OnGameFeatureRegistering();
			Action->OnGameFeatureLoading();
			Action->OnGameFeatureActivating(Context);
		}
	}

	NumGameFeaturePluginsLoading = GameFeaturesToEnable.Num();

	for (const FString &PluginName : GameFeaturesToEnable)
	{
		FString PluginURL;
		if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
		{
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &AModularGameMode::OnGameFeaturePluginLoadComplete));
		}
		else
		{
			ensureMsgf(false, TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for game mode %s - fix data, ignoring for this run"), *PluginName, *this->GetPrimaryAssetId().ToString());
		}
	}
}

void AModularGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MyEndPlayReason = EndPlayReason;
	NumGameFeaturePluginsLoading = GameFeaturesToEnable.Num();

	for (const FString &PluginName : GameFeaturesToEnable)
	{
		FString PluginURL;
		if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL, FGameFeaturePluginDeactivateComplete::CreateUObject(this, &AModularGameMode::OnGameFeaturePluginUnLoadComplete));
		}
		else
		{
			ensureMsgf(false, TEXT("OnEndPlay failed to find plugin URL from PluginName %s for game mode %s - fix data, ignoring for this run"), *PluginName, *this->GetPrimaryAssetId().ToString());
		}
	}
}

void AModularGameMode::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult &Result)
{
	// decrement the number of plugins that are loading
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading <= 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void AModularGameMode::OnExperienceFullLoadCompleted()
{
	Super::BeginPlay();
}

void AModularGameMode::OnGameFeaturePluginUnLoadComplete(const UE::GameFeatures::FResult &Result)
{
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading <= 0)
	{
		OnExperienceFullUnLoadCompleted();
	}
}

void AModularGameMode::OnExperienceFullUnLoadCompleted()
{
	Super::EndPlay(MyEndPlayReason);
}
