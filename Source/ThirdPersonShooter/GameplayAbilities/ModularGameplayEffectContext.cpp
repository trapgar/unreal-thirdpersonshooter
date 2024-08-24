// Copyright Epic Games, Inc. All Rights Reserved.

#include "ModularGameplayEffectContext.h"

#include "GameplayAbilities/ModularAbilityAttenuatorInterface.h"
#include "Engine/HitResult.h"
#include "PhysicalMaterials/PhysicalMaterial.h"

#if UE_WITH_IRIS
#include "Iris/ReplicationState/PropertyNetSerializerInfoRegistry.h"
#include "Serialization/GameplayEffectContextNetSerializer.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularGameplayEffectContext)

class FArchive;

FModularGameplayEffectContext* FModularGameplayEffectContext::ExtractEffectContext(struct FGameplayEffectContextHandle Handle)
{
	FGameplayEffectContext* BaseEffectContext = Handle.Get();
	if ((BaseEffectContext != nullptr) && BaseEffectContext->GetScriptStruct()->IsChildOf(FModularGameplayEffectContext::StaticStruct()))
	{
		return (FModularGameplayEffectContext*)BaseEffectContext;
	}

	return nullptr;
}

bool FModularGameplayEffectContext::NetSerialize(FArchive& Ar, class UPackageMap* Map, bool& bOutSuccess)
{
	FGameplayEffectContext::NetSerialize(Ar, Map, bOutSuccess);

	return true;
}

#if UE_WITH_IRIS
namespace UE::Net
{
	// Forward to FGameplayEffectContextNetSerializer
	// Note: If FModularGameplayEffectContext::NetSerialize() is modified, a custom NetSerializesr must be implemented as the current fallback will no longer be sufficient.
	UE_NET_IMPLEMENT_FORWARDING_NETSERIALIZER_AND_REGISTRY_DELEGATES(ModularGameplayEffectContext, FGameplayEffectContextNetSerializer);
}
#endif

