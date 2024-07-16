// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

// #include "GameFramework/PlayerController.h"
#include "Components/ControllerComponent.h"
#include "Inventory/InventoryItemInstance.h"

#include "QuickBarComponent.generated.h"

class AActor;
class AEquipmentItemInstance;
class UEquipmentManagerComponent;
class UObject;
struct FFrame;


USTRUCT(BlueprintType)
struct FQuickBarSlotsChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TArray<TObjectPtr<UInventoryItemInstance>> Slots;
};


USTRUCT(BlueprintType)
struct FQuickBarActiveIndexChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	TObjectPtr<AActor> Owner = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Inventory)
	int32 ActiveIndex = 0;
};


UCLASS(Blueprintable, HideCategories=("Component Tick", "Component Replication"), meta=(BlueprintSpawnableComponent))
class INVENTORYANDEQUIPMENTRUNTIME_API UQuickBarComponent : public UControllerComponent
{
	GENERATED_BODY()

public:
	UQuickBarComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Get the owning player controller
	UFUNCTION(BlueprintCallable, Category="Controller")
	APlayerController* GetPlayerController() const { return GetController<APlayerController>(); }

	// Cycle the active quickbar slot forward
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void CycleActiveSlotForward();

	// Cycle the active quickbar slot backward
	UFUNCTION(BlueprintCallable, Category="Equipment")
	void CycleActiveSlotBackward();

	// Set the active quickbar slot index
	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Equipment")
	void SetActiveSlotIndex(int32 NewIndex);

	// Returns a list of all quickbar slots
	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Equipment|QuickBar")
	TArray<UInventoryItemInstance*> GetSlots() const { return Slots; }

	// Returns the active quickbar slot index
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	// Returns the inventory item in the active quickbar slot
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UInventoryItemInstance* GetActiveSlotItem() const;

	// Returns the inventory item in the specified quickbar slot
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UInventoryItemInstance* GetItemAtSlot(int32 SlotIndex) const { return Slots.IsValidIndex(SlotIndex) ? Slots[SlotIndex] : nullptr; };

	// Returns the next free quickbar slot
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	// Add an item to the specified quickbar slot
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item);

	// Remove an item from the specified quickbar slot
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	UInventoryItemInstance* RemoveItemFromSlot(int32 SlotIndex);

	virtual void BeginPlay() override;

private:
	void UnequipItemInSlot();
	void EquipItemInSlot();

	UEquipmentManagerComponent* FindEquipmentManager() const;

protected:
	UPROPERTY(EditDefaultsOnly, Category="Equipment")
	int32 NumberOfSlots = 5;

	UFUNCTION()
	void OnRep_Slots();

	UFUNCTION()
	void OnRep_ActiveSlotIndex();

private:
	UPROPERTY(ReplicatedUsing=OnRep_Slots)
	TArray<TObjectPtr<UInventoryItemInstance>> Slots;

	UPROPERTY(ReplicatedUsing=OnRep_ActiveSlotIndex)
	int32 ActiveSlotIndex = -1;

	UPROPERTY()
	TObjectPtr<AEquipmentItemInstance> EquippedItem;
};