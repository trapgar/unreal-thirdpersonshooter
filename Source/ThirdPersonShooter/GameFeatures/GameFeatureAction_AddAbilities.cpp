// Copyright Epic Games, Inc. All Rights Reserved.

#include "GameFeatureAction_AddAbilities.h"
#include "Engine/GameInstance.h"
#include "Components/GameFrameworkComponentManager.h"
#include "AbilitySystemComponent.h"
#include "Engine/World.h"
#include "GameFeatureAction_WorldActionBase.h"

#if WITH_EDITOR
#include "Misc/DataValidation.h"
#endif

#include UE_INLINE_GENERATED_CPP_BY_NAME(GameFeatureAction_AddAbilities)

#define LOCTEXT_NAMESPACE "GameFeatures"

// --------------------------------------------------------
// UGameFeatureAction_AddAbilities

void UGameFeatureAction_AddAbilities::OnGameFeatureLoading()
{
	Super::OnGameFeatureLoading();

	// TODO: fix this
	// Assets referenced by the ModularGameMode need to be loaded synchronously;
	// otherwise they will be null on load
	// Probably because ModularGameMode isn't a normal GameFeature.
	for (const FGameFeatureAbilitiesEntry& Entry : AbilitiesList)
	{
		for (auto Set : Entry.GrantedAbilities)
		{
			const FSoftObjectPath& AssetPath = Set.AbilityType.ToSoftObjectPath();
			if (AssetPath.IsValid())
			{
				if (!Set.AbilityType.IsValid())
				{
					Set.AbilityType.LoadSynchronous();
				}
			}
			else
			{
				UE_LOG(LogGameFeatures, Error, TEXT("Failed to load soft object reference `Entry.GrantedAbilities` for AddAbilities. Abilities will not be added."));
			}
		}
		for (auto Set : Entry.GrantedAttributes)
		{
			const FSoftObjectPath& AssetPath = Set.AttributeSetType.ToSoftObjectPath();
			if (AssetPath.IsValid())
			{
				if (!Set.AttributeSetType.IsValid())
				{
					Set.AttributeSetType.LoadSynchronous();
				}
			}
			else
			{
				UE_LOG(LogGameFeatures, Error, TEXT("Failed to load soft object reference `Entry.GrantedAttributes` for AddAbilities. Abilities will not be added."));
			}
		}
		for (auto Set : Entry.GrantedAbilitySets)
		{
			const FSoftObjectPath& AssetPath = Set.ToSoftObjectPath();
			if (AssetPath.IsValid())
			{
				if (!Set.IsValid())
				{
					Set.LoadSynchronous();
				}
			}
			else
			{
				UE_LOG(LogGameFeatures, Error, TEXT("Failed to load soft object reference `Entry.GrantedAbilitySets` for AddAbilities. Abilities will not be added."));
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::OnGameFeatureActivating(FGameFeatureActivatingContext& Context)
{
	Super::OnGameFeatureActivating(Context);
}

void UGameFeatureAction_AddAbilities::OnGameFeatureDeactivating(FGameFeatureDeactivatingContext& Context)
{
	Super::OnGameFeatureDeactivating(Context);
}

#if WITH_EDITOR
EDataValidationResult UGameFeatureAction_AddAbilities::IsDataValid(FDataValidationContext& Context) const
{
	EDataValidationResult Result = CombineDataValidationResults(Super::IsDataValid(Context), EDataValidationResult::Valid);

	int32 EntryIndex = 0;
	for (const FGameFeatureAbilitiesEntry& Entry : AbilitiesList)
	{
		if (Entry.ActorClass.IsNull())
		{
			Result = EDataValidationResult::Invalid;
			Context.AddError(FText::Format(LOCTEXT("EntryHasNullActor", "Null ActorClass at index {0} in AbilitiesList"), FText::AsNumber(EntryIndex)));
		}
		++EntryIndex;
	}

	return Result;
}
#endif

void UGameFeatureAction_AddAbilities::AddToWorld(const FWorldContext &WorldContext, const FGameFeatureStateChangeContext &ChangeContext)
{
	UWorld* World = WorldContext.World();
	UGameInstance* GameInstance = WorldContext.OwningGameInstance;
	FPerContextData& ActiveData = ContextData.FindOrAdd(ChangeContext);

	if ((GameInstance != nullptr) && (World != nullptr) && World->IsGameWorld())
	{
		if (UGameFrameworkComponentManager* ComponentMan = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{			
			int32 EntryIndex = 0;
			for (const FGameFeatureAbilitiesEntry& Entry : AbilitiesList)
			{
				if (!Entry.ActorClass.IsNull())
				{
					UGameFrameworkComponentManager::FExtensionHandlerDelegate AddAbilitiesDelegate = UGameFrameworkComponentManager::FExtensionHandlerDelegate::CreateUObject(
						this, &UGameFeatureAction_AddAbilities::HandleActorExtension, EntryIndex, ChangeContext);
					TSharedPtr<FComponentRequestHandle> ExtensionRequestHandle = ComponentMan->AddExtensionHandler(Entry.ActorClass, AddAbilitiesDelegate);

					ActiveData.ComponentRequests.Add(ExtensionRequestHandle);
					EntryIndex++;
				}
			}
		}
	}
}

void UGameFeatureAction_AddAbilities::Reset(FPerContextData& ActiveData)
{
	while (!ActiveData.ActiveExtensions.IsEmpty())
	{
		auto ExtensionIt = ActiveData.ActiveExtensions.CreateIterator();
		RemoveActorAbilities(ExtensionIt->Key, ActiveData);
	}

	ActiveData.ComponentRequests.Empty();
}

void UGameFeatureAction_AddAbilities::HandleActorExtension(AActor* Actor, FName EventName, int32 EntryIndex, FGameFeatureStateChangeContext ChangeContext)
{
	FPerContextData* ActiveData = ContextData.Find(ChangeContext);
	if (AbilitiesList.IsValidIndex(EntryIndex) && ActiveData)
	{
		const FGameFeatureAbilitiesEntry& Entry = AbilitiesList[EntryIndex];
		if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionRemoved) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverRemoved))
		{
			RemoveActorAbilities(Actor, *ActiveData);
		}
		else if ((EventName == UGameFrameworkComponentManager::NAME_ExtensionAdded) || (EventName == UGameFrameworkComponentManager::NAME_ReceiverAdded))
		{
			AddActorAbilities(Actor, Entry, *ActiveData);
		}
	}
}

void UGameFeatureAction_AddAbilities::AddActorAbilities(AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
{
	check(Actor);
	if (!Actor->HasAuthority())
	{
		return;
	}

	// early out if Actor already has ability extensions applied
	if (ActiveData.ActiveExtensions.Find(Actor) != nullptr)
	{
		return;	
	}

	if (UAbilitySystemComponent* AbilitySystemComponent = FindOrAddComponentForActor<UAbilitySystemComponent>(Actor, AbilitiesEntry, ActiveData))
	{
		FActorExtensions AddedExtensions;
		AddedExtensions.Abilities.Reserve(AbilitiesEntry.GrantedAbilities.Num());
		AddedExtensions.Attributes.Reserve(AbilitiesEntry.GrantedAttributes.Num());
		AddedExtensions.AbilitySetHandles.Reserve(AbilitiesEntry.GrantedAbilitySets.Num());

		for (const FModularAbilityGrant& Ability : AbilitiesEntry.GrantedAbilities)
		{
			if (!Ability.AbilityType.IsNull())
			{
				FGameplayAbilitySpec NewAbilitySpec(Ability.AbilityType.LoadSynchronous());
				FGameplayAbilitySpecHandle AbilityHandle = AbilitySystemComponent->GiveAbility(NewAbilitySpec);

				AddedExtensions.Abilities.Add(AbilityHandle);
			}
		}

		for (const FModularAttributeSetGrant& Attributes : AbilitiesEntry.GrantedAttributes)
		{
			if (!Attributes.AttributeSetType.IsNull())
			{
				TSubclassOf<UAttributeSet> SetType = Attributes.AttributeSetType.LoadSynchronous();
				if (SetType)
				{
					UAttributeSet* NewSet = NewObject<UAttributeSet>(AbilitySystemComponent->GetOwner(), SetType);
					if (!Attributes.InitializationData.IsNull())
					{
						UDataTable* InitData = Attributes.InitializationData.LoadSynchronous();
						if (InitData)
						{
							NewSet->InitFromMetaDataTable(InitData);
						}
					}

					AddedExtensions.Attributes.Add(NewSet);
					AbilitySystemComponent->AddAttributeSetSubobject(NewSet);
				}
			}
		}

		UAbilitySystemComponent* ASC = CastChecked<UAbilitySystemComponent>(AbilitySystemComponent);
		for (const TSoftObjectPtr<const UModularAbilitySet>& SetPtr : AbilitiesEntry.GrantedAbilitySets)
		{
			if (const UModularAbilitySet* Set = SetPtr.Get())
			{
				Set->GiveToAbilitySystem(ASC, &AddedExtensions.AbilitySetHandles.AddDefaulted_GetRef());
			}
		}

		ActiveData.ActiveExtensions.Add(Actor, AddedExtensions);
	}
	else
	{
		UE_LOG(LogGameFeatures, Error, TEXT("Failed to find/add an ability component to '%s'. Abilities will not be granted."), *Actor->GetPathName());
	}
}

void UGameFeatureAction_AddAbilities::RemoveActorAbilities(AActor* Actor, FPerContextData& ActiveData)
{
	if (FActorExtensions* ActorExtensions = ActiveData.ActiveExtensions.Find(Actor))
	{
		if (UAbilitySystemComponent* AbilitySystemComponent = Actor->FindComponentByClass<UAbilitySystemComponent>())
		{
			for (UAttributeSet* AttribSetInstance : ActorExtensions->Attributes)
			{
				AbilitySystemComponent->RemoveSpawnedAttribute(AttribSetInstance);
			}

			for (FGameplayAbilitySpecHandle AbilityHandle : ActorExtensions->Abilities)
			{
				AbilitySystemComponent->SetRemoveAbilityOnEnd(AbilityHandle);
			}

			UAbilitySystemComponent* ASC = CastChecked<UAbilitySystemComponent>(AbilitySystemComponent);
			for (FModularAbilitySet_GrantedHandles& SetHandle : ActorExtensions->AbilitySetHandles)
			{
				SetHandle.TakeFromAbilitySystem(ASC);
			}
		}

		ActiveData.ActiveExtensions.Remove(Actor);
	}
}

UActorComponent* UGameFeatureAction_AddAbilities::FindOrAddComponentForActor(UClass* ComponentType, AActor* Actor, const FGameFeatureAbilitiesEntry& AbilitiesEntry, FPerContextData& ActiveData)
{
	UActorComponent* Component = Actor->FindComponentByClass(ComponentType);
	
	bool bMakeComponentRequest = (Component == nullptr);
	if (Component)
	{
		// Check to see if this component was created from a different `UGameFrameworkComponentManager` request.
		// `Native` is what `CreationMethod` defaults to for dynamically added components.
		if (Component->CreationMethod == EComponentCreationMethod::Native)
		{
			// Attempt to tell the difference between a true native component and one created by the GameFrameworkComponent system.
			// If it is from the UGameFrameworkComponentManager, then we need to make another request (requests are ref counted).
			UObject* ComponentArchetype = Component->GetArchetype();
			bMakeComponentRequest = ComponentArchetype->HasAnyFlags(RF_ClassDefaultObject);
		}
	}

	if (bMakeComponentRequest)
	{
		UWorld* World = Actor->GetWorld();
		UGameInstance* GameInstance = World->GetGameInstance();

		if (UGameFrameworkComponentManager* ComponentMan = UGameInstance::GetSubsystem<UGameFrameworkComponentManager>(GameInstance))
		{
			TSharedPtr<FComponentRequestHandle> RequestHandle = ComponentMan->AddComponentRequest(AbilitiesEntry.ActorClass, ComponentType);
			ActiveData.ComponentRequests.Add(RequestHandle);
		}

		if (!Component)
		{
			Component = Actor->FindComponentByClass(ComponentType);
			ensureAlways(Component);
		}
	}

	return Component;
}

#undef LOCTEXT_NAMESPACE
