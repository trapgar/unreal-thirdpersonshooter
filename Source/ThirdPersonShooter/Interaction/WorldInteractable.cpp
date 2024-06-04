// Copyright Epic Games, Inc. All Rights Reserved.

#include "WorldInteractable.h"

#include "Async/TaskGraphInterfaces.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(WorldInteractable)

struct FInteractionQuery;

AWorldInteractable::AWorldInteractable()
{
}

void AWorldInteractable::GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder)
{
	InteractionBuilder.AddInteractionOption(Option);
}
