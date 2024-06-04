// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "Templates/SubclassOf.h"
#include "UObject/Interface.h"

#include "UObject/ObjectPtr.h"
#include "IPickupable.generated.h"

template <typename InterfaceType> class TScriptInterface;

class AActor;
class UInventoryItemDefinition;
class UInventoryItemInstance;
class UInventoryManagerComponent;
class UObject;
struct FFrame;

USTRUCT(BlueprintType)
struct INVENTORYANDEQUIPMENTRUNTIME_API FPickupTemplate
{
	GENERATED_BODY()

public:
	// Number of item instances to give to the player
	UPROPERTY(EditAnywhere)
	int32 StackCount = 1;

	// The definition of the item to give
	UPROPERTY(EditAnywhere)
	TSubclassOf<UInventoryItemDefinition> ItemDef;
};

USTRUCT(BlueprintType)
struct INVENTORYANDEQUIPMENTRUNTIME_API FPickupInstance
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UInventoryItemInstance> Item = nullptr;
};

USTRUCT(BlueprintType)
struct INVENTORYANDEQUIPMENTRUNTIME_API FInventoryPickup
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPickupInstance> Instances;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FPickupTemplate> Templates;
};

/**  */
UINTERFACE(MinimalAPI, BlueprintType, meta = (CannotImplementInterfaceInBlueprint))
class UPickupable : public UInterface
{
	GENERATED_BODY()
};

/**  */
class IPickupable
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	virtual FInventoryPickup GetPickupInventory() const = 0;
};

/**  */
UCLASS()
class UPickupableStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UPickupableStatics();

public:
	UFUNCTION(BlueprintPure)
	static TScriptInterface<IPickupable> GetFirstPickupableFromActor(AActor* Actor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, meta = (WorldContext = "Ability"))
	static void AddPickupToInventory(UInventoryManagerComponent* InventoryComponent, TScriptInterface<IPickupable> Pickup);
};
