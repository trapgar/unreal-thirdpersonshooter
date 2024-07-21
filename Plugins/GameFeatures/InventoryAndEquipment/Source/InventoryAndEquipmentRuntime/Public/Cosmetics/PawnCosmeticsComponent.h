#pragma once

#include "Components/PawnComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Equipment/QuickBarComponent.h"

#include "PawnCosmeticsComponent.generated.h"

// A component that handles spawning cosmetic actors attached to the owner pawn on all clients
UCLASS(meta=(BlueprintSpawnableComponent))
class INVENTORYANDEQUIPMENTRUNTIME_API UPawnCosmeticsComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPawnCosmeticsComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

private:
	FGameplayMessageListenerHandle Handle_QuickbarSlotsChanged;

	void OnQuickbarSlotsChanged(const FGameplayTag Channel, const FQuickBarSlotsChangedMessage& Payload);
};
