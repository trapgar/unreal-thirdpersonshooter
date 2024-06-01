// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Engine/World.h"
#include "Common/TaggedActor.h"
#include "Common/GameplayTagStack.h"
#include "GameplayTagContainer.h"

#include "EquipmentItemInstance.generated.h"

class AActor;
class APawn;
struct FFrame;

/**
 * UEquipmentItemInstance
 *
 * A piece of equipment spawned and applied to a pawn
 */
UCLASS(BlueprintType, Blueprintable)
class INVENTORYANDEQUIPMENTRUNTIME_API AEquipmentItemInstance : public ATaggedActor
{
    GENERATED_BODY()

public:
	AEquipmentItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetPawn() const;

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedPawn(TSubclassOf<APawn> PawnType) const;

	virtual void OnReady();
	virtual void OnEquipped();
	virtual void OnUnequipped();

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount);

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Equipment)
	int32 GetStatTagStackCount(FGameplayTag Tag) const;

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Equipment)
	bool HasStatTag(FGameplayTag Tag) const;

	UFUNCTION(BlueprintCallable, BlueprintPure=true, Category = Equipment)
	TSubclassOf<UEquipmentItemDefinition> GetItemDef() const
	{
		return ItemDef;
	}

	void SetItemDef(TSubclassOf<UEquipmentItemDefinition> InDef);

protected:
	virtual void PreInitializeComponents() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnReady"))
	void K2_OnReady();

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();

private:

	FGameplayTagStackContainer StatTags;

	// The item definition
	TSubclassOf<UEquipmentItemDefinition> ItemDef;
};
