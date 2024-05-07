// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameplayInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/GameFrameworkComponentManager.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameplayInputComponent)

class UGameplayInputConfiguration;

const FName UGameplayInputComponent::NAME_BindInputsNow("BindInputsNow");
const FName UGameplayInputComponent::NAME_ActorFeatureName("Hero");

UGameplayInputComponent::UGameplayInputComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UGameplayInputComponent::InitializePlayerInput(UInputComponent *PlayerInputComponent)
{
	check(PlayerInputComponent);

	AActor* Owner = GetOwner();
	APawn* Pawn = Cast<APawn>(Owner);

	if (!Pawn)
	{
		UE_LOG(LogInput, Error, TEXT("Owner is NOT a Pawn - unable to bind input ([%s])."), *GetNameSafe(Owner));
		return;
	}

	const APlayerController* PC = Pawn->GetController<APlayerController>();
	check(PC);

	const ULocalPlayer* LP = Cast<ULocalPlayer>(PC->GetLocalPlayer());
	check(LP);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = LP->GetSubsystem<UEnhancedInputLocalPlayerSubsystem>();
	check(Subsystem);

	// Subsystem->ClearAllMappings();


	// if (const ULyraPawnExtensionComponent* PawnExtComp = ULyraPawnExtensionComponent::FindPawnExtensionComponent(Pawn))
	// {
	// 	if (const ULyraPawnData* PawnData = PawnExtComp->GetPawnData<ULyraPawnData>())
	// 	{
	// 		if (const ULyraInputConfig* InputConfig = PawnData->InputConfig)
	// 		{
	// 			for (const FInputMappingContextAndPriority& Mapping : DefaultInputMappings)
	// 			{
	// 				if (UInputMappingContext* IMC = Mapping.InputMapping.Get())
	// 				{
	// 					if (Mapping.bRegisterWithSettings)
	// 					{
	// 						if (UEnhancedInputUserSettings* Settings = Subsystem->GetUserSettings())
	// 						{
	// 							Settings->RegisterInputMappingContext(IMC);
	// 						}
							
	// 						FModifyContextOptions Options = {};
	// 						Options.bIgnoreAllPressedKeysUntilRelease = false;
	// 						// Actually add the config to the local player
	// 						Subsystem->AddMappingContext(IMC, Mapping.Priority, Options);
	// 					}
	// 				}
	// 			}

	// 			// The Lyra Input Component has some additional functions to map Gameplay Tags to an Input Action.
	// 			// If you want this functionality but still want to change your input component class, make it a subclass
	// 			// of the ULyraInputComponent or modify this component accordingly.
	// 			// ULyraInputComponent* GIC = Cast<ULyraInputComponent>(PlayerInputComponent);
	// 			// if (ensureMsgf(GIC, TEXT("Unexpected Input Component class! The Gameplay Abilities will not be bound to their inputs. Change the input component to ULyraInputComponent or a subclass of it.")))
	// 			// {
	// 			// 	// Add the key mappings that may have been set by the player
	// 			// 	GIC->AddInputMappings(InputConfig, Subsystem);

	// 			// 	// This is where we actually bind and input action to a gameplay tag, which means that Gameplay Ability Blueprints will
	// 			// 	// be triggered directly by these input actions Triggered events. 
	// 			// 	TArray<uint32> BindHandles;
	// 			// 	GIC->BindAbilityActions(InputConfig, this, &ThisClass::Input_AbilityInputTagPressed, &ThisClass::Input_AbilityInputTagReleased, /*out*/ BindHandles);

	// 			// 	GIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Move, ETriggerEvent::Triggered, this, &ThisClass::Input_Move, /*bLogIfNotFound=*/ false);
	// 			// 	GIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Mouse, ETriggerEvent::Triggered, this, &ThisClass::Input_LookMouse, /*bLogIfNotFound=*/ false);
	// 			// 	GIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Look_Stick, ETriggerEvent::Triggered, this, &ThisClass::Input_LookStick, /*bLogIfNotFound=*/ false);
	// 			// 	GIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_Crouch, ETriggerEvent::Triggered, this, &ThisClass::Input_Crouch, /*bLogIfNotFound=*/ false);
	// 			// 	GIC->BindNativeAction(InputConfig, LyraGameplayTags::InputTag_AutoRun, ETriggerEvent::Triggered, this, &ThisClass::Input_AutoRun, /*bLogIfNotFound=*/ false);
	// 			// }
	// 		}
	// 	}
	// }

	// // if (ensure(!bReadyToBindInputs))
	// // {
	// 	bReadyToBindInputs = true;
	// // }
 
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APlayerController*>(PC), NAME_BindInputsNow);
	// UGameFrameworkComponentManager::SendGameFrameworkComponentExtensionEvent(const_cast<APawn*>(Pawn), NAME_BindInputsNow);
}

void UGameplayInputComponent::AddInputMappings(const UGameplayInputConfiguration* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to add something from your input config if required
}

void UGameplayInputComponent::RemoveInputMappings(const UGameplayInputConfiguration* InputConfig, UEnhancedInputLocalPlayerSubsystem* InputSubsystem) const
{
	check(InputConfig);
	check(InputSubsystem);

	// Here you can handle any custom logic to remove input mappings that you may have added above
}

void UGameplayInputComponent::RemoveBinds(TArray<uint32>& BindHandles)
{
	for (uint32 Handle : BindHandles)
	{
		RemoveBindingByHandle(Handle);
	}
	BindHandles.Reset();
}
