#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "EquipmentItemInstance.h"
#include "Character/Components/Ability/ModularAbilitySet.h"

#include "EquipmentItemDefinition.generated.h"

/**
 * UEquipmentItemDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UEquipmentItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Actor class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<AEquipmentItemInstance> InstanceType;

	// Gameplay ability sets to grant when this item is equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TArray<TObjectPtr<const UModularAbilitySet>> AbilitySetsToGrant;

	// Flag that indicates if in order to be given the ability sets of this item, it must be drawn - as opposed to merely equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	bool bIsPassive = false;

	// Socket tag to attach the equipment to when not in active use (holstered)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spawn Info", Meta = (Categories = "Equipment.Socket"))
	FGameplayTag AttachSocket;

	// Transform to apply to the equipment when attached to the socket
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn Info")
	FTransform AttachTransform;

};
