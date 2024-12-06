#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "NativeGameplayTags.h"

#include "PlayerExperienceComponent.generated.h"


THIRDPERSONSHOOTER_API UE_DECLARE_GAMEPLAY_TAG_EXTERN(TAG_Gameplay_Message_Experience_StackChanged);

// --------------------------------------------------------


USTRUCT(BlueprintType)
struct FExperienceStackChangedMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadOnly, Category=Experience)
	TObjectPtr<AController> Source = nullptr;

	UPROPERTY(BlueprintReadOnly, Category=Experience)
	int32 NewCount = 0;

	UPROPERTY(BlueprintReadOnly, Category=Experience)
	int32 Delta = 0;

	UPROPERTY(BlueprintReadOnly, Category=Experience)
	FGameplayTagContainer ContextTags;

	UPROPERTY(BlueprintReadOnly, Category=Experience)
	FText Reason;
};


// --------------------------------------------------------


UCLASS()
class THIRDPERSONSHOOTER_API UPlayerExperienceComponent : public UControllerComponent
{
	GENERATED_BODY()

public:

	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Experience")
	void AddExperience(int32 ExperienceAmount, const FText& Reason, FGameplayTagContainer ContextTags);

	UFUNCTION(BlueprintCallable)
	int32 GetCurrentExperience() const { return CurrentExperience; }

private:
	UPROPERTY()
	int32 CurrentExperience = 0;
};
