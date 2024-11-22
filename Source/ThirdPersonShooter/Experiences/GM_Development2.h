#pragma once

#include "ThirdPersonShooterGameMode.h"
#include "GameFeatureAction_AddComponents.h"
#include "GameFeatures/GameFeatureAction_AddInputBinding.h"
#include "GameFeatures/GameFeatureAction_AddInputContextMapping.h"
#include "GameFeatures/GameFeatureAction_AddAbilities.h"

#include "Components/GameFrameworkComponentManager.h"

#include "Player/ThirdPersonShooterPlayerController.h"
#include "Character/ThirdPersonShooterCharacter.h"
#include "Equipment/QuickBarComponent.h"

#include "GM_Development2.generated.h"

namespace UE::GameFeatures { struct FResult; }

/**
 * AGM_Development2
 * 
 * Game mode that dynamically loads game features on startup.
 */
UCLASS(Blueprintable, BlueprintType, Config = Game)
class AGM_Development2 : public AThirdPersonShooterGameMode
{
	GENERATED_BODY()
public:

	AGM_Development2(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get())
		: Super(ObjectInitializer)
	{
		GameFeaturesToEnable.Emplace("VehiclesAndNavigation");

		UGameFeatureAction_AddComponents* Action_AddComponents = NewObject<UGameFeatureAction_AddComponents>();
		FGameFeatureComponentEntry PC_QuickBarComponent;
		PC_QuickBarComponent.ActorClass = TSoftClassPtr<AActor>(AThirdPersonShooterPlayerController::StaticClass());
		PC_QuickBarComponent.ComponentClass = TSoftClassPtr<UActorComponent>(UQuickBarComponent::StaticClass());
		PC_QuickBarComponent.bClientComponent = 1;
		PC_QuickBarComponent.bServerComponent = 0;
		PC_QuickBarComponent.AdditionFlags = static_cast<uint8>(EGameFrameworkAddComponentFlags::AddUnique);
		Action_AddComponents->ComponentList.Emplace(PC_QuickBarComponent);

		UGameFeatureAction_AddInputBinding* Action_AddInputBinding = NewObject<UGameFeatureAction_AddInputBinding>();
		Action_AddInputBinding->InputConfigs.Emplace(
			TSoftObjectPtr<UInputActionToGameplayTagBindings>(FSoftObjectPath("/Game/Input/InputBindings_Character"))
		);
		Action_AddInputBinding->InputConfigs.Emplace(
			TSoftObjectPtr<UInputActionToGameplayTagBindings>(FSoftObjectPath("/Game/Input/InputBindings_InventoryAndEquipment"))
		);

		UGameFeatureAction_AddInputContextMapping* Action_AddInputContextMapping = NewObject<UGameFeatureAction_AddInputContextMapping>();
		FInputMappingContextAndPriority IMC_Character;
		IMC_Character.InputMapping = TSoftObjectPtr<UInputMappingContext>(FSoftObjectPath("/Game/Input/IMC_Character"));
		IMC_Character.Priority = 0;
		IMC_Character.bRegisterWithSettings = true;
		Action_AddInputContextMapping->InputMappings.Emplace(IMC_Character);

		FInputMappingContextAndPriority IMC_InventoryAndEquipment;
		IMC_InventoryAndEquipment.InputMapping = TSoftObjectPtr<UInputMappingContext>(FSoftObjectPath("/Game/Input/IMC_InventoryAndEquipment"));
		IMC_InventoryAndEquipment.Priority = 0;
		IMC_InventoryAndEquipment.bRegisterWithSettings = true;
		Action_AddInputContextMapping->InputMappings.Emplace(IMC_InventoryAndEquipment);

		UGameFeatureAction_AddAbilities* Action_AddAbilities = NewObject<UGameFeatureAction_AddAbilities>();
		FGameFeatureAbilitiesEntry AbilitySet_Hero;
		AbilitySet_Hero.ActorClass = TSoftClassPtr<AActor>(AThirdPersonShooterCharacter::StaticClass());
		AbilitySet_Hero.GrantedAbilitySets.Add(TSoftObjectPtr<UModularAbilitySet>(FSoftObjectPath("/Game/Characters/Abilities/AbilitySet_Hero")));
		AbilitySet_Hero.GrantedAbilitySets.Add(TSoftObjectPtr<UModularAbilitySet>(FSoftObjectPath("/Game/Items/Weapons/AbilitySet_InventoryAndEquipment")));
		Action_AddAbilities->AbilitiesList.Emplace(AbilitySet_Hero);

		Actions.Emplace(Action_AddComponents);
		Actions.Emplace(Action_AddInputBinding);
		Actions.Emplace(Action_AddInputContextMapping);
		Actions.Emplace(Action_AddAbilities);
	};
};
