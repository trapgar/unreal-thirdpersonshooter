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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Spawn Info", Meta = (Categories = "Equipment.Socket"))
	FGameplayTag SocketName;

	// Transform to apply to the equipment when attached to the socket
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn Info")
	FTransform Transform;
};
