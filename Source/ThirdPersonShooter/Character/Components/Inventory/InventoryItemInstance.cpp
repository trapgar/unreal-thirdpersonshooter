// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryItemInstance.h"
#include "InventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryItemInstance)

class FLifetimeProperty;

UInventoryItemInstance::UInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void UInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

void UInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (StatTags.Contains(Tag))
	{
		StatTags.Add(Tag, StatTags[Tag] + StackCount);
	}
	else
	{
		StatTags.Add(Tag, StackCount);
	}
}

void UInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
{
	if (StatTags.Contains(Tag) && StatTags[Tag] - StackCount > 0)
	{
		StatTags.Add(Tag, StatTags[Tag] - StackCount);
	}
	else
	{
		StatTags.Remove(Tag);
	}
}

int32 UInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	if (StatTags.Contains(Tag))
	{
		return StatTags[Tag];
	}
	else
	{
		return 0;
	}
}

bool UInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.Contains(Tag);
}

void UInventoryItemInstance::SetItemDef(UInventoryItemDefinition* InDef)
{
	ItemDef = InDef;
}

