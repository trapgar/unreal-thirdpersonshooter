// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatureAction_AddInputBinding.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"


#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputBinding)

#define LOCTEXT_NAMESPACE "GameFeatures"

void UGameFeatureAction_AddInputBinding::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputBinding::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputBinding::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 Index = 0;

	for (const TSoftObjectPtr<const UGameplayInputConfiguration>& Entry : InputConfigs)
	{
		if (Entry.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputConfig", "Null InputConfig at index {0}."), Index));
		}
		++Index;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddInputBinding::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandlePawnExtension, ChangeContext);
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				ComponentManager->AddExtensionHandler(APawn::StaticClass(), AddAbilitiesDelegate);

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputBinding::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.PawnsAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APawn> PawnPtr = ActiveData.PawnsAddedTo.Top();
		if (PawnPtr.IsValid())
		{
			RemoveInputMapping(PawnPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.PawnsAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputBinding::HandlePawnExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	APawn* AsPawn = CastChecked<APawn>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(AsPawn, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverAdded))
	{
		AddInputMappingForPlayer(AsPawn, ActiveData);
	}
}

void UGameFeatureAction_AddInputBinding::AddInputMappingForPlayer(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());

	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const TSoftObjectPtr<const UGameplayInputConfiguration>& Entry : InputConfigs)
			{
				if (const UGameplayInputConfiguration* BindSet = Entry.Get())
				{
					// TODO: implement
					// HeroComponent->AddAdditionalInputConfig(BindSet);
				}
			}
			ActiveData.PawnsAddedTo.AddUnique(Pawn);
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}
}

void UGameFeatureAction_AddInputBinding::RemoveInputMapping(APawn* Pawn, FPerContextData& ActiveData)
{
	APlayerController* PlayerController = Cast<APlayerController>(Pawn->GetController());

	if (ULocalPlayer* LocalPlayer = PlayerController ? PlayerController->GetLocalPlayer() : nullptr)
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const TSoftObjectPtr<const UGameplayInputConfiguration>& Entry : InputConfigs)
			{
				if (const UGameplayInputConfiguration* InputConfig = Entry.Get())
				{
					// TODO: implement
					// HeroComponent->RemoveAdditionalInputConfig(InputConfig);
				}
			}
		}
	}

	ActiveData.PawnsAddedTo.Remove(Pawn);
}

#undef LOCTEXT_NAMESPACE

