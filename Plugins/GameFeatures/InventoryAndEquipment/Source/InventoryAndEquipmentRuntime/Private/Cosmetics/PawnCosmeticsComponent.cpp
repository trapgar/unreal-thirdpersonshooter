#include "Cosmetics/PawnCosmeticsComponent.h"
#include "Inventory/InventoryFragment_QuickbarCosmeticInfo.h"

UPawnCosmeticsComponent::UPawnCosmeticsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UPawnCosmeticsComponent::BeginPlay()
{
	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	Handle_QuickbarSlotsChanged = MessageSystem.RegisterListener(TAG_QuickBar_Message_SlotsChanged, this, &ThisClass::OnQuickbarSlotsChanged);
}

void UPawnCosmeticsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(Handle_QuickbarSlotsChanged);
}

void UPawnCosmeticsComponent::OnQuickbarSlotsChanged(const FGameplayTag Channel, const FQuickBarSlotsChangedMessage& Payload)
{
	if (Payload.Owner == GetOwner())
	{
		for (UInventoryItemInstance* Item : Payload.Slots)
		{
			if (Item)
			{
				for (UInventoryItemFragment* Fragment : Item->GetItemDef()->Fragments)
				{
					if (UInventoryFragment_QuickbarCosmeticInfo* Cosmetic = Cast<UInventoryFragment_QuickbarCosmeticInfo>(Fragment))
					{
						Cosmetic->OnInstanceAdded(Item);
					}
				}
			}
		}
	}
}
