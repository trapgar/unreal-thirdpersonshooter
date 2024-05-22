#pragma once

#include "GameFramework/GameModeBase.h"
#include "GameFeatureAction.h"

#include "ModularGameMode.generated.h"

namespace UE::GameFeatures { struct FResult; }

/** Please add a class description */
UCLASS(Blueprintable, BlueprintType, Config = Game, HideCategories = ("Actor Tick", "Game", "Game Mode", "Physics", "Events", "Level Instance", "Cooking"), Meta = (ShortTooltip = "The base game mode class used by this project."))
class AModularGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:

	AModularGameMode(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// List of game features to automatically activate on GM startup
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Features Dependencies")
	TArray<FString> GameFeaturesToEnable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Actions, Instanced)
	TArray<TObjectPtr<UGameFeatureAction>> Actions;

private:

	int32 NumGameFeaturePluginsLoading = 0;
	EEndPlayReason::Type MyEndPlayReason;

	void OnGameFeaturePluginLoadComplete(const UE::GameFeatures::FResult& Result);
	void OnExperienceFullLoadCompleted();

	void OnGameFeaturePluginUnLoadComplete(const UE::GameFeatures::FResult &Result);
	void OnExperienceFullUnLoadCompleted();

};
