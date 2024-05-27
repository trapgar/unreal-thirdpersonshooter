#include "ModularGameplayAbility.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(ModularGameplayAbility)

UModularGameplayAbility::UModularGameplayAbility(const FObjectInitializer &ObjectInitializer)
	: Super(ObjectInitializer)
{
	ReplicationPolicy = EGameplayAbilityReplicationPolicy::ReplicateNo;
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
	NetSecurityPolicy = EGameplayAbilityNetSecurityPolicy::ClientOrServer;
}
