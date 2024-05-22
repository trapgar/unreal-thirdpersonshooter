#pragma once

#include "EquipmentItemDefinition.h"
#include "Templates/SubclassOf.h"

#include "EquipmentFragment_AttachmentInfo.generated.h"

class UEquipmentItemDefinition;
class UObject;

UCLASS(BlueprintType)
class UEquipmentFragment_AttachmentInfo : public UEquipmentItemFragment
{
	GENERATED_BODY()

public:

	// Socket tag to attach the equipment to when not in active use (holstered)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attach Info")
	FName HolsteredSocketName;

	// Transform to apply to the equipment when attached to the holstered socket
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attach Info")
	FTransform HolsteredTransform;

	// Socket tag to attach the equipment to when in active use (drawn)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Attach Info")
	FName DrawnSocketName;

	// Transform to apply to the equipment when attached to the drawn socket
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Attach Info")
	FTransform DrawnTransform;
};
