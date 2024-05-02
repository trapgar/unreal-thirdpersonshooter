#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "EquipmentInstance.h"
#include "Character/Components/Ability/ModularAbilitySet.h"

#include "EquipmentDefinition.generated.h"

/**
 * UEquipmentDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UEquipmentDefinition : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Actor class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<AEquipmentInstance> InstanceType;

	// Gameplay ability sets to grant when this item is equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TArray<TObjectPtr<const UModularAbilitySet>> AbilitySetsToGrant;

	// Socket tag to attach the equipment to when not in active use (holstered)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spawn Info", Meta = (Categories = "Equipment.Socket"))
	FGameplayTag AttachSocket;

	// Transform to apply to the equipment when attached to the socket
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn Info")
	FTransform AttachTransform;

};
