// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatureAction_AddInputContextMapping.h"
#include "Components/GameFrameworkComponentManager.h"
#include "Engine/GameInstance.h"
#include "GameFramework/PlayerController.h"
#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "EnhancedInputSubsystems.h"
#include "GameFeatures/GameFeatureAction_WorldActionBase.h"
#include "InputMappingContext.h"
#include "UserSettings/EnhancedInputUserSettings.h"
#include "Engine/AssetManager.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddInputContextMapping)

#define LOCTEXT_NAMESPACE "GameFeatures"

// --------------------------------------------------------
// UGameFeatureAction_AddInputContextMapping

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureRegistering()
{
	Super::OnGameFeatureRegistering();

	RegisterInputMappingContexts();
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureLoading()
{
	Super::OnGameFeatureLoading();

	// TODO: fix this
	// Assets referenced by the ModularGameMode need to be loaded synchronously;
	// otherwise they will be null on AddInputMappingForPlayer(...)
	// Probably because ModularGameMode isn't a normal GameFeature.
	for (const FInputMappingContextAndPriority& Entry : InputMappings)
	{
		const FSoftObjectPath& AssetPath = Entry.InputMapping.ToSoftObjectPath();
		if (AssetPath.IsValid())
		{
			if (!Entry.InputMapping.IsValid())
			{
				Entry.InputMapping.LoadSynchronous();
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to load soft object reference `UInputMappingContext` for IMC. Input mappings will not be added."));
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	FPerContextData& ActiveData = ContextData.FindOrAdd(Context);
	if (!ensure(ActiveData.ExtensionRequestHandles.IsEmpty()) ||
		!ensure(ActiveData.ControllersAddedTo.IsEmpty()))
	{
		Reset(ActiveData);
	}
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);

	FPerContextData* ActiveData = ContextData.Find(Context);
	if (ensure(ActiveData))
	{
		Reset(*ActiveData);
	}
}

void UGameFeatureAction_AddInputContextMapping::OnGameFeatureUnregistering()
{
	Super::OnGameFeatureUnregistering();	
	
	UnregisterInputMappingContexts();
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContexts()
{
	RegisterInputContextMappingsForGameInstanceHandle = FWorldDelegates::OnStartGameInstance.AddUObject(this,
		&UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance
	);

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();

	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator(); WorldContextIterator; ++WorldContextIterator)
	{
		RegisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance != nullptr && !GameInstance->OnLocalPlayerAddedEvent.IsBoundToObject(this))
	{
		GameInstance->OnLocalPlayerAddedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer);
		GameInstance->OnLocalPlayerRemovedEvent.AddUObject(this, &UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer);
		
		for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator(); LocalPlayerIterator; ++LocalPlayerIterator)
		{
			RegisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::RegisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		UAssetManager& AssetManager = UAssetManager::Get();
		
		if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			// TODO: This evaluates to false when enabled through a GM, but not at runtime
			if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
			{
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					if (!Entry.bRegisterWithSettings)
					{
						continue;
					}

					const FSoftObjectPath& AssetPath = Entry.InputMapping.ToSoftObjectPath();

					// Register this IMC with the settings!
					if (AssetPath.IsValid())
					{
						if (UInputMappingContext* IMC = Entry.InputMapping.Get())
						{
							Settings->RegisterInputMappingContext(IMC);
						}
					}
				}
			}
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContexts()
{
	FWorldDelegates::OnStartGameInstance.Remove(RegisterInputContextMappingsForGameInstanceHandle);
	RegisterInputContextMappingsForGameInstanceHandle.Reset();

	const TIndirectArray<FWorldContext>& WorldContexts = GEngine->GetWorldContexts();
	for (TIndirectArray<FWorldContext>::TConstIterator WorldContextIterator = WorldContexts.CreateConstIterator(); WorldContextIterator; ++WorldContextIterator)
	{
		UnregisterInputContextMappingsForGameInstance(WorldContextIterator->OwningGameInstance);
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputContextMappingsForGameInstance(UGameInstance* GameInstance)
{
	if (GameInstance != nullptr)
	{
		GameInstance->OnLocalPlayerAddedEvent.RemoveAll(this);
		GameInstance->OnLocalPlayerRemovedEvent.RemoveAll(this);

		for (TArray<ULocalPlayer*>::TConstIterator LocalPlayerIterator = GameInstance->GetLocalPlayerIterator(); LocalPlayerIterator; ++LocalPlayerIterator)
		{
			UnregisterInputMappingContextsForLocalPlayer(*LocalPlayerIterator);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::UnregisterInputMappingContextsForLocalPlayer(ULocalPlayer* LocalPlayer)
{
	if (ensure(LocalPlayer))
	{
		if (UEnhancedInputLocalPlayerSubsystem* EISubsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
		{
			if (UEnhancedInputUserSettings* Settings = EISubsystem->GetUserSettings())
			{
				for (const FInputMappingContextAndPriority& Entry : InputMappings)
				{
					// Skip entries that don't want to be registered
					if (!Entry.bRegisterWithSettings)
					{
						continue;
					}

					// Register this IMC with the settings!
					if (UInputMappingContext* IMC = Entry.InputMapping.Get())
					{
						Settings->UnregisterInputMappingContext(IMC);
					}
				}
			}
		}
	}
}


#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddInputContextMapping::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 Index = 0;

	for (const FInputMappingContextAndPriority& Entry : InputMappings)
	{
		if (Entry.InputMapping.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("NullInputMapping", "Null InputMapping at index {0}."), Index));
		}
		++Index;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddInputContextMapping::AddToWorld(const FWorldContext& WorldContext, const FGameFeatureStateChangeContext& ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentManager = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate =
				UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(this, &ThisClass::HandleControllerExtension, ChangeContext);
			TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle =
				ComponentManager->AddExtensionHandler(APlayerController::StaticClass(), AddAbilitiesDelegate);

			ActiveData.ExtensionRequestHandles.Add(ExtensionRequestHandle);
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::Reset(FPerContextData& ActiveData)
{
	ActiveData.ExtensionRequestHandles.Empty();

	while (!ActiveData.ControllersAddedTo.IsEmpty())
	{
		TWeakObjectPtr<APlayerController> ControllerPtr = ActiveData.ControllersAddedTo.Top();
		if (ControllerPtr.IsValid())
		{
			RemoveInputMapping(ControllerPtr.Get(), ActiveData);
		}
		else
		{
			ActiveData.ControllersAddedTo.Pop();
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::HandleControllerExtension(AActor* Actor, FName EventName, FGameFeatureStateChangeContext ChangeContext)
{
	APlayerController* AsController = CastChecked<APlayerController>(Actor);
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	// TODO Why does this code mix and match controllers and local players? ControllersAddedTo is never modified
	if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
	{
		RemoveInputMapping(AsController, ActiveData);
	}
	else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverAdded))
	{
		AddInputMappingForPlayer(AsController, ActiveData);
	}
}

void UGameFeatureAction_AddInputContextMapping::AddInputMappingForPlayer(APlayerController* PlayerController, FPerContextData& ActiveData)
{
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			FModifyContextOptions Options = {};
			Options.bIgnoreAllPressedKeysUntilRelease = false;
			bool bAdded = false;

			for (const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				// IMC_Character::Get() will return null if it's not loaded
				if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					InputSystem->AddMappingContext(IMC, Entry.Priority, Options);
					bAdded = true;
				}
				else
				{
					UE_LOG(LogGameFeatures, Error, TEXT("Failed to load soft object reference `UInputMappingContext` for IMC. Input mappings will not be added."));
				}
			}

			if (bAdded)
			{
				ActiveData.ControllersAddedTo.Add(PlayerController);
			}
		}
		else
		{
			UE_LOG(LogGameFeatures, Error, TEXT("Failed to find `UEnhancedInputLocalPlayerSubsystem` for local player. Input mappings will not be added. Make sure you're set to use the EnhancedInput system via config file."));
		}
	}
}

void UGameFeatureAction_AddInputContextMapping::RemoveInputMapping(APlayerController* PlayerController, FPerContextData& ActiveData)
{
	if (ULocalPlayer* LocalPlayer = PlayerController->GetLocalPlayer())
	{
		if (UEnhancedInputLocalPlayerSubsystem* InputSystem = LocalPlayer->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>())
		{
			for (const FInputMappingContextAndPriority& Entry : InputMappings)
			{
				if (const UInputMappingContext* IMC = Entry.InputMapping.Get())
				{
					InputSystem->RemoveMappingContext(IMC);
				}
			}
		}
	}

	ActiveData.ControllersAddedTo.Remove(PlayerController);
}

#undef LOCTEXT_NAMESPACE
