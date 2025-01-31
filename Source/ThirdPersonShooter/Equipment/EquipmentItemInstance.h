// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Common/TaggedActor.h"
#include "Common/GameplayTagStack.h"
#include "Equipment/EquipmentItemDefinition.h"
#include "Inventory/InventoryItemInstance.h"

#include "EquipmentItemInstance.generated.h"

class AActor;
class APawn;
class UInventoryItemInstance;
struct FFrame;


/**
 * UEquipmentItemInstance
 *
 * A piece of equipment spawned and applied to a pawn
 */
UCLASS(BlueprintType, Blueprintable)
class THIRDPERSONSHOOTER_API UEquipmentItemInstance : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentItemInstance(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UObject interface
	virtual bool IsSupportedForNetworking() const override { return true; }
	virtual UWorld* GetWorld() const override final { return GetInstigator() ? GetInstigator()->GetWorld() : nullptr; };
	//~End of UObject interface

	UFUNCTION(BlueprintPure, Category=Equipment)
	APawn* GetInstigator() const { return Instigator; }

	void SetInstigator(APawn* InInstigator) { Instigator = InInstigator; }

	UFUNCTION(BlueprintPure, Category=Equipment, meta=(DeterminesOutputType=PawnType))
	APawn* GetTypedInstigator(TSubclassOf<APawn> PawnType) const
	{
		APawn* Result = nullptr;
		if (UClass* ActualPawnType = PawnType)
		{
			if (GetInstigator()->IsA(ActualPawnType))
			{
				Result = Cast<APawn>(GetInstigator());
			}
		}
		return Result;
	}

	virtual void OnEquipped() { K2_OnEquipped(); };
	virtual void OnUnequipped() { K2_OnUnequipped(); };

	// Adds a specified number of stacks to the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	void AddStatTagStack(FGameplayTag Tag, int32 StackCount) { StatTags.AddStack(Tag, StackCount); };

	// Removes a specified number of stacks from the tag (does nothing if StackCount is below 1)
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Equipment)
	void RemoveStatTagStack(FGameplayTag Tag, int32 StackCount) { StatTags.RemoveStack(Tag, StackCount); };

	// Returns the stack count of the specified tag (or 0 if the tag is not present)
	UFUNCTION(BlueprintCallable, Category=Equipment)
	int32 GetStatTagStackCount(FGameplayTag Tag) const { return StatTags.GetStackCount(Tag); };

	// Returns true if there is at least one stack of the specified tag
	UFUNCTION(BlueprintCallable, Category=Equipment)
	bool HasStatTag(FGameplayTag Tag) const { return StatTags.ContainsTag(Tag); };

	UFUNCTION(BlueprintCallable, BlueprintPure=true, Category=Equipment)
	UEquipmentItemDefinition* GetItemDef() const { return ItemDef; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false, meta=(DeterminesOutputType=FragmentClass))
	const UEquipmentItemFragment* FindFragmentByClass(TSubclassOf<UEquipmentItemFragment> FragmentClass) const
	{
		return ItemDef ? ItemDef->FindFragmentByClass(FragmentClass) : nullptr;
	}

	template <typename TResultClass>
	const TResultClass* FindFragmentByClass() const { return (TResultClass*)FindFragmentByClass(TResultClass::StaticClass()); }

	// @TODO: InventoryItemInstance already has this function, should remove?
	UFUNCTION(BlueprintCallable, BlueprintPure=true, Category=Equipment)
	TArray<AActor*> GetSpawnedActors() { return SpawnedActors; }

	template <typename TResultClass>
	TResultClass* FindSpawnedActorByClass() { return (TResultClass*)FindSpawnedActorByClass(TResultClass::StaticClass()); }

	UFUNCTION(BlueprintCallable, BlueprintPure=true, meta=(DeterminesOutputType=ActorClass))
	AActor* FindSpawnedActorByClass(TSubclassOf<AActor> ActorClass)
	{
		for (AActor* Actor : SpawnedActors)
		{
			if (Actor && Actor->GetClass()->IsChildOf(ActorClass))
			{
				return Actor;
			}
		}

		return nullptr;
	}

	UFUNCTION(BlueprintPure, Category=Equipment)
	UInventoryItemInstance* GetAssociatedItem() const { return Source; }

	void SetItemDef(UEquipmentItemDefinition* InDef) { ItemDef = InDef; };

	void AddSpawnedActor(AActor* Actor) { SpawnedActors.Emplace(Actor); }

	void DestroySpawnedActors()
	{
		for (AActor* Actor : SpawnedActors)
		{
			if (Actor)
			{
				Actor->Destroy();
			}
		}
	}

	void SetAssociatedItem(UInventoryItemInstance* InSource) { Source = InSource; }

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnEquipped"))
	void K2_OnEquipped();

	UFUNCTION(BlueprintImplementableEvent, Category=Equipment, meta=(DisplayName="OnUnequipped"))
	void K2_OnUnequipped();

#if WITH_EDITOR
	virtual EDataValidationResult IsDataValid(class FDataValidationContext& Context) const override;
#endif

private:
	UFUNCTION()
	void OnRep_Instigator();

private:

	UPROPERTY(Replicated)
	FGameplayTagStackContainer StatTags;

	// The item definition
	UPROPERTY(Replicated)
	UEquipmentItemDefinition* ItemDef;

	UPROPERTY(Replicated)
	TArray<TObjectPtr<AActor>> SpawnedActors;

	UPROPERTY(ReplicatedUsing=OnRep_Instigator)
	TObjectPtr<APawn> Instigator;

	TObjectPtr<UInventoryItemInstance> Source;
};
