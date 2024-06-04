// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFeatureAction_WorldActionBase.h"
#include "UObject/SoftObjectPtr.h"
#include "Input/InputActionToGameplayTagBindings.h"

#include "GameFeatureAction_AddInputBinding.generated.h"

class AActor;
class UInputMappingContext;
class UPlayer;
class APlayerController;
struct FComponentRequestHandle;
class UInputActionToGameplayTagBindings;

/**
 * UGameFeatureAction_AddInputBinding
 * 
 * Adds the relationship between an input action and Input Gameplay Tag to the `UGameplayInputComponent`.
 */
UCLASS(MinimalAPI, meta = (DisplayName = "Add Input Binding"))
class UGameFeatureAction_AddInputBinding final : public UGameFeatureAction_WorldActionBase
{
	GENERATED_BODY()

public:
	//~ Begin UGameFeatureAction interface
	virtual void OnGameFeatureLoading() override;
	virtual void OnGameFeatureActivating(FGameFeatureActivatingContext& Context) override;
	virtual void OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context) override;
	//~ End UGameFeatureAction interface

	//~ Begin UObject interface
#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif
	//~ End UObject interface

	// List of Gameplay input actions to bind
	UPROPERTY(EditAnywhere, Category="Input", meta=(AssetBundles="Client,Server"), meta=(DisplayName="Input Bindings"))
	TArray<TSoftObjectPtr<const UInputActionToGameplayTagBindings>> InputConfigs;

private:
	struct FPerContextData
	{
		TArray<TSharedPtr<FComponentRequestHandle>> ExtensionRequestHandles;
		TArray<TWeakObjectPtr<APawn>> PawnsAddedTo;
	};

	TMap<FGameFeatureStateChangeContext, FPerContextData> ContextData;

	//~ Begin UGameFeatureAction_WorldActionBase interface
	virtual void AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext) override;
	//~ End UGameFeatureAction_WorldActionBase interface

	void Reset(FPerContextData& ActiveData);
	void HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext);
	void AddInputBindingsForPlayer(APawn* Pawn, FPerContextData& ActiveData);
	void RemoveInputBindingsFromPlayer(APawn* Pawn, FPerContextData& ActiveData);

};
