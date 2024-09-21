#pragma once

#include "Components/PawnComponent.h"

#include "PawnCosmeticsComponent.generated.h"

/**
 * A component that handles spawning cosmetic actors attached to the owner pawn on all clients
 * 
 * TODO: Basically this should look very similar to the EquipmentManagerComponent, just doesn't have abilities
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class THIRDPERSONSHOOTER_API UPawnCosmeticsComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPawnCosmeticsComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	virtual void InitializeComponent() override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:

	UFUNCTION(BlueprintCallable, Category="Pawn|Cosmetics")
	void GetSpawnedActors(TArray<AActor*>& Cosmetics);

	UFUNCTION(BlueprintCallable, Category="Pawn|Cosmetics")
	bool SetHidden(bool bHidden)
	{
		TArray<AActor*> SpawnedActors;
		GetSpawnedActors(/*out*/SpawnedActors);

		for (AActor* Actor : SpawnedActors)
		{
			Actor->SetActorHiddenInGame(bHidden);
		}

		return SpawnedActors.Num() > 0;
	};
};
