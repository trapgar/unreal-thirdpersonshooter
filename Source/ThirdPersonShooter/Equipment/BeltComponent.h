#pragma once

#include "QuickBarComponent.h"

#include "BeltComponent.generated.h"


/**
 * Wrapper for QuickBarComponent that exposes the next/previous slots.
 * 
 * @see UQuickBarComponent
 * @example NextItem = Slots[(GetActiveSlotIndex() + 1) % Slots.Num()]
 */
UCLASS(Blueprintable, meta=(BlueprintSpawnableComponent))
class THIRDPERSONSHOOTER_API UBeltComponent : public UQuickBarComponent
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UInventoryItemInstance* GetPreviousSlotItem()
	{
		int32 SlotIndex = GetActiveSlotIndex() - 1;

		if (SlotIndex < 0)
		{
			SlotIndex = GetSlots().Num() - 1;
		}

		return GetItemAtSlot(SlotIndex);
	}

	UFUNCTION(BlueprintCallable, BlueprintPure=false)
	UInventoryItemInstance* GetNextSlotItem()
	{
		int32 SlotIndex = GetActiveSlotIndex() + 1;

		if (SlotIndex - 1 < 0)
		{
			return nullptr;
		}

		if (SlotIndex >= GetSlots().Num())
		{
			SlotIndex = 0;
		}

		return GetItemAtSlot(SlotIndex);
	}
};

