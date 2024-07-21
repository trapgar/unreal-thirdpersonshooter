// Copyright Epic Games, Inc. All Rights Reserved.

#include "Inventory/InventoryItemInstance.h"
#include "Inventory/InventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryItemInstance)

class FLifetimeProperty;

UInventoryItemInstance::UInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
	, StatTags(FGameplayTagStackContainer())
{
	// UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(this);
	// Handle_QuickbarSlotsChanged = MessageSystem.RegisterListener(TAG_QuickBar_Message_SlotsChanged, this, &ThisClass::OnQuickbarSlotsChanged);
}

void UInventoryItemInstance::BeginDestroy()
{
	// UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	// MessageSubsystem.UnregisterListener(Handle_QuickbarSlotsChanged);

	Super::BeginDestroy();
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

