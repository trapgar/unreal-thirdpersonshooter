// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "GameplayEffectTypes.h"

#include "ModularGameplayEffectContext.generated.h"

class AActor;
class FArchive;
class IModularAbilityAttenuatorInterface;
class UObject;
class UPhysicalMaterial;

USTRUCT()
struct THIRDPERSONSHOOTER_API FModularGameplayEffectContext : public FGameplayEffectContext
{
	GENERATED_BODY()

	FModularGameplayEffectContext()
		: FGameplayEffectContext()
	{
	}

	FModularGameplayEffectContext(AActor* InInstigator, AActor* InEffectCauser)
		: FGameplayEffectContext(InInstigator, InEffectCauser)
	{
	}

	/** Returns the wrapped FModularGameplayEffectContext from the handle, or nullptr if it doesn't exist or is the wrong type */
	static FModularGameplayEffectContext* ExtractEffectContext(struct FGameplayEffectContextHandle Handle);

	/** Sets the object used as the ability source */
	void SetAbilityAttenuator(const IModularAbilityAttenuatorInterface* InObject, float InSourceLevel)
	{
		AbilityAttenuator = MakeWeakObjectPtr(Cast<const UObject>(InObject));
	};

	/** Returns the ability source interface associated with the source object. Only valid on the authority. */
	const IModularAbilityAttenuatorInterface* GetAbilitySource() const { return Cast<IModularAbilityAttenuatorInterface>(AbilityAttenuator.Get());};

	virtual FGameplayEffectContext* Duplicate() const override
	{
		FModularGameplayEffectContext* NewContext = new FModularGameplayEffectContext();
		*NewContext = *this;
		if (GetHitResult())
		{
			// Does a deep copy of the hit result
			NewContext->AddHitResult(*GetHitResult(), true);
		}
		return NewContext;
	}

	virtual UScriptStruct* GetScriptStruct() const override
	{
		return FModularGameplayEffectContext::StaticStruct();
	}

	/** Overridden to serialize new fields */
	virtual bool NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess) override;

	/** Returns the physical material from the hit result if there is one */
	const UPhysicalMaterial* GetPhysicalMaterial() const
	{
		if (const FHitResult* HitResultPtr = GetHitResult())
		{
			return HitResultPtr->PhysMaterial.Get();
		}
		return nullptr;
	};

protected:
	/** Ability Source object (should implement IModularAbilityAttenuatorInterface). NOT replicated currently */
	UPROPERTY()
	TWeakObjectPtr<const UObject> AbilityAttenuator;
};

template<>
struct TStructOpsTypeTraits<FModularGameplayEffectContext> : public TStructOpsTypeTraitsBase2<FModularGameplayEffectContext>
{
	enum
	{
		WithNetSerializer = true,
		WithCopy = true
	};
};
