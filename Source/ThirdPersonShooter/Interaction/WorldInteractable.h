// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameFramework/Actor.h"
#include "IInteractableTarget.h"
#include "InteractionOption.h"

#include "WorldInteractable.generated.h"

class UObject;
struct FInteractionQuery;

/**
 * AWorldInteractable
 * 
 * Allows an actor to be interacted with by a user and grant abilities etc.
 */
UCLASS(Abstract, Blueprintable)
class THIRDPERSONSHOOTER_API AWorldInteractable : public AActor, public IInteractableTarget
{
	GENERATED_BODY()

public:

	AWorldInteractable();

	virtual void GatherInteractionOptions(const FInteractionQuery& InteractQuery, FInteractionOptionBuilder& InteractionBuilder) override;

	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category=Interactions, meta=(DisplayName="Activate"))
	void K2_Activate(AActor* InteractionInstigator);

protected:

	UPROPERTY(EditAnywhere, meta=(DisplayName="Options"))
	FInteractionOption Option;
};
