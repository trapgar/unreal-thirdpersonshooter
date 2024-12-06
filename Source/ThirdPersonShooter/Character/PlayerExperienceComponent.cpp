#include "PlayerExperienceComponent.h"

#include UE_INLINE_GENERATED_CPP_BY_NAME(PlayerExperienceComponent)

UE_DEFINE_GAMEPLAY_TAG(TAG_Gameplay_Message_Experience_StackChanged, "Gameplay.Message.Experience.StackChanged");

void UPlayerExperienceComponent::AddExperience_Implementation(int32 ExperienceAmount, const FText& Reason, FGameplayTagContainer ContextTags)
{
	if (ExperienceAmount > 0)
	{
		CurrentExperience += ExperienceAmount;
	}

	FExperienceStackChangedMessage Message;
	Message.Source = GetController<AController>();
	Message.NewCount = CurrentExperience;
	Message.Delta = ExperienceAmount;
	Message.Reason = Reason;
	Message.ContextTags = ContextTags;

	UGameplayMessageSubsystem& MessageSystem = UGameplayMessageSubsystem::Get(GetOwner()->GetWorld());
	MessageSystem.BroadcastMessage(TAG_Gameplay_Message_Experience_StackChanged, Message);
}
