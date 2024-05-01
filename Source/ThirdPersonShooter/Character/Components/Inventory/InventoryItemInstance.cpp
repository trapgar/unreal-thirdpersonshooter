// Copyright Epic Games, Inc. All Rights Reserved.

#include "InventoryItemInstance.h"
#include "InventoryItemDefinition.h"
#include "Net/UnrealNetwork.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(InventoryItemInstance)

class FLifetimeProperty;

AInventoryItemInstance::AInventoryItemInstance(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
}

void AInventoryItemInstance::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, StatTags);
	DOREPLIFETIME(ThisClass, ItemDef);
}

void AInventoryItemInstance::AddStatTagStack(FGameplayTag Tag, int32 StackCount)
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

void AInventoryItemInstance::RemoveStatTagStack(FGameplayTag Tag, int32 StackCount)
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

int32 AInventoryItemInstance::GetStatTagStackCount(FGameplayTag Tag) const
{
	return StatTags[Tag];
}

bool AInventoryItemInstance::HasStatTag(FGameplayTag Tag) const
{
	return StatTags.Contains(Tag);
}

void AInventoryItemInstance::SetItemDef(TSubclassOf<UInventoryItemDefinition> InDef)
{
	ItemDef = InDef;
}

