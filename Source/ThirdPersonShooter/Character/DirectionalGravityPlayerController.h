#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "DirectionalGravityPlayerController.generated.h"

/**
 * A Player Controller class which adds input-handling functionality for
 * CharacterMovementController's custom gravity mechanics.
 * 
 * @cite https://www.youtube.com/@ArghanionsPuzzlebox
 */
UCLASS()
class ADirectionalGravityPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	virtual void UpdateRotation(float DeltaTime) override;

	// Converts a rotation from world space to gravity relative space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityRelativeRotation(FRotator Rotation, FVector GravityDirection);

	// Converts a rotation from gravity relative space to world space.
	UFUNCTION(BlueprintPure)
	static FRotator GetGravityWorldRotation(FRotator Rotation, FVector GravityDirection);

private:
	FVector LastFrameGravity = FVector::ZeroVector;
};
