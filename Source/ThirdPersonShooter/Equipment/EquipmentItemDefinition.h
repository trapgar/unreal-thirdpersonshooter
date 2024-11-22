#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "GameplayAbilities/ModularAbilitySet.h"

#include "EquipmentItemDefinition.generated.h"

template <typename T> class TSubclassOf;

class AActor;
class UModularAbilitySet;
class UEquipmentItemInstance;


// --------------------------------------------------------


// Represents a fragment of an item definition
UCLASS(DefaultToInstanced, EditInlineNew, Abstract)
class THIRDPERSONSHOOTER_API UEquipmentItemFragment : public UObject
{
	GENERATED_BODY()

public:
	virtual void OnInstanceCreated(UEquipmentItemInstance* Instance) const {}
	virtual void OnInstanceDestroyed(UEquipmentItemInstance* Instance) const {}
};


// --------------------------------------------------------


/**
 * Definition of a piece of equipment that can be applied to a pawn
 */
UCLASS(Blueprintable, Const, Abstract, BlueprintType)
class UEquipmentItemDefinition : public UObject
{
	GENERATED_BODY()

public:
	UEquipmentItemDefinition(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	// Equipment type to spawn
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Equipment)
	TSubclassOf<UEquipmentItemInstance> InstanceType;

	// Gameplay ability sets to grant when this item is equipped
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects)
	TArray<TObjectPtr<const UModularAbilitySet>> AbilitySetsToGrant;

	// Flag indicating if the AbilitySets granted by this item should be applied on add to inventory (passively), or only while equipped (activately in use)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Effects, meta=(DisplayName="Is Passive?"))
	bool bIsPassive = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Attributes, Instanced)
	TArray<TObjectPtr<UEquipmentItemFragment>> Fragments;

	// Gameplay-related tags associated with this equipment (e.g.: "Primary", "Secondary", "Belt", etc.)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category=Equipment, meta=(Categories="Equipment.Classification"))
	FGameplayTagContainer StaticGameplayTags;

public:
	const UEquipmentItemFragment* FindFragmentByClass(TSubclassOf<UEquipmentItemFragment> FragmentClass) const;

	template<class T>
	const T* FindFragmentByClass() const
	{
		static_assert(std::is_base_of<UEquipmentItemFragment, T>::value, "T must be a subclass of UEquipmentItemFragment");
		return (T*)FindFragmentByClass(T::StaticClass());
	}
};


// --------------------------------------------------------


UCLASS()
class UEquipmentFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable, meta=(DeterminesOutputType=FragmentClass))
	static const UEquipmentItemFragment* FindItemDefinitionFragment(UEquipmentItemDefinition* ItemDef, TSubclassOf<UEquipmentItemFragment> FragmentClass);
};
