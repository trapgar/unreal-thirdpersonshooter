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

	UFUNCTION(BlueprintCallable, Category="Controller")
	APlayerController* GetPlayerController() const { return GetController<APlayerController>(); }

	UFUNCTION(BlueprintCallable, Category="Equipment")
	void CycleActiveSlotForward();

	UFUNCTION(BlueprintCallable, Category="Equipment")
	void CycleActiveSlotBackward();

	UFUNCTION(Server, Reliable, BlueprintCallable, Category="Equipment")
	void SetActiveSlotIndex(int32 NewIndex);

	UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="Equipment|QuickBar")
	TArray<UInventoryItemInstance*> GetSlots() const { return Slots; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetActiveSlotIndex() const { return ActiveSlotIndex; }

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UInventoryItemInstance* GetActiveSlotItem() const;

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UInventoryItemInstance* GetItemAtSlot(int32 SlotIndex) const { return Slots.IsValidIndex(SlotIndex) ? Slots[SlotIndex] : nullptr; };

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	int32 GetNextFreeItemSlot() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddItemToSlot(int32 SlotIndex, UInventoryItemInstance* Item);

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
