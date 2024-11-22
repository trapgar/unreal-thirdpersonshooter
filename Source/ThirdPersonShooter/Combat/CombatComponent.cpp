#include "CombatComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(CombatComponent)


UCombatComponent::UCombatComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// SetIsReplicatedByDefault(true);
	bWantsInitializeComponent = true;
}
