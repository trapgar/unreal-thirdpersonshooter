#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Templates/SubclassOf.h"
#include "EquipmentItemInstance.h"
#include "Character/Components/Ability/ModularAbilitySet.h"

#include "EquipmentItemDefinition.generated.h"

// --------------------------------------------------------

// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class THIRDPERSONSHOOTER_API UEquipmentItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(AEquipmentItemInstance* Instance) const {}
};


// --------------------------------------------------------


/**
 * UEquipmentItemDefinition
 *
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UEquipmentItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Actor class to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Default")
	TSubclassOf<AEquipmentItemInstance> InstanceType;

	// Gameplay ability sets to grant when this item is equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects")
	TArray<TObjectPtr<const UModularAbilitySet>> AbilitySetsToGrant;

	// Flag indicating if the ability sets of this item are applied on equip, or on draw
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effects", meta=(DisplayName="Is Passive?"))
	bool bIsPassive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Attributes, Instanced)
	TArray<TObjectPtr<UEquipmentItemFragment>> Fragments;

public:
	const UEquipmentItemFragment* FindFragmentByClass(TSubclassOf<UEquipmentItemFragment> FragmentClass) const;

	template<class T>
	T* FindFragmentByClass() const
	{
		for (UEquipmentItemFragment* Fragment : Fragments)
		{
			if (T* Result = Cast<T>(Fragment))
			{
				return Result;
			}
		}
		return nullptr;
	}
};


// --------------------------------------------------------


UCLASS()
class UEquipmentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UEquipmentItemFragment* FindItemDefinitionFragment(TSubclassOf<UEquipmentItemDefinition> ItemDef, TSubclassOf<UEquipmentItemFragment> FragmentClass);
};
