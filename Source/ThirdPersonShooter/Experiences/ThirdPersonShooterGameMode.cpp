#include "ThirdPersonShooterGameMode.h"
#include "Player/ThirdPersonShooterPlayerController.h"
#include "Character/ThirdPersonShooterCharacter.h"
#include "GameFeaturesSubsystem.h"

#include "CustomLogChannels.h"

AThirdPersonShooterGameMode::AThirdPersonShooterGameMode(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	PlayerControllerClass = AThirdPersonShooterPlayerController::StaticClass();
	DefaultPawnClass = AThirdPersonShooterCharacter::StaticClass();
}

void AThirdPersonShooterGameMode::BeginPlay()
{
	if (Actions.Num() > 0)
	{
		// TODO: properly mock the context so that all the SoftPtr assets are actually loaded
		FGameFeatureActivatingContext Context;

		// Only apply to our specific world context if set
		const FWorldContext* ExistingWorldContext = GEngine->GetWorldContextFromWorld(GetWorld());
		if (ExistingWorldContext)
		{
			Context.SetRequiredWorldContextHandle(ExistingWorldContext->ContextHandle);
		}

		for (UGameFeatureAction* Action : Actions)
		{
			Action->OnGameFeatureRegistering();
			Action->OnGameFeatureLoading();
			Action->OnGameFeatureActivating(Context);
		}
	}

	NumGameFeaturePluginsLoading = GameFeaturesToEnable.Num();

	for (const FString& PluginName : GameFeaturesToEnable)
	{
		FString PluginURL;
		if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
		{
			UGameFeaturesSubsystem::Get().LoadAndActivateGameFeaturePlugin(PluginURL, FGameFeaturePluginLoadComplete::CreateUObject(this, &AThirdPersonShooterGameMode::OnGameFeaturePluginLoadComplete));
		}
		else
		{
			ensureMsgf(false, TEXT("OnExperienceLoadComplete failed to find plugin URL from PluginName %s for game mode %s - fix data, ignoring for this run"), *PluginName, *this->GetPrimaryAssetId().ToString());
		}
	}
}

void AThirdPersonShooterGameMode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	MyEndPlayReason = EndPlayReason;
	NumGameFeaturePluginsLoading = GameFeaturesToEnable.Num();

	for (const FString& PluginName : GameFeaturesToEnable)
	{
		FString PluginURL;
		if (UGameFeaturesSubsystem::Get().GetPluginURLByName(PluginName, /*out*/ PluginURL))
		{
			UGameFeaturesSubsystem::Get().DeactivateGameFeaturePlugin(PluginURL, FGameFeaturePluginDeactivateComplete::CreateUObject(this, &AThirdPersonShooterGameMode::OnGameFeaturePluginUnLoadComplete));
		}
		else
		{
			ensureMsgf(false, TEXT("OnEndPlay failed to find plugin URL from PluginName %s for game mode %s - fix data, ignoring for this run"), *PluginName, *this->GetPrimaryAssetId().ToString());
		}
	}
}

void AThirdPersonShooterGameMode::OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result)
{
	// decrement the number of plugins that are loading
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading <= 0)
	{
		OnExperienceFullLoadCompleted();
	}
}

void AThirdPersonShooterGameMode::OnExperienceFullLoadCompleted()
{
	Super::BeginPlay();
}

void AThirdPersonShooterGameMode::OnGameFeaturePluginUnLoadComplete(const UE::GameFeatures::FResult& Result)
{
	NumGameFeaturePluginsLoading--;

	if (NumGameFeaturePluginsLoading <= 0)
	{
		OnExperienceFullUnLoadCompleted();
	}
}

void AThirdPersonShooterGameMode::OnExperienceFullUnLoadCompleted()
{
	Super::EndPlay(MyEndPlayReason);
}
