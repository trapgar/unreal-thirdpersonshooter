// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Components/PawnComponent.h"
#include "Cosmetics/CosmeticAnimationTypes.h"
#include "PawnCosmeticPartTypes.h"
#include "Net/Serialization/FastArraySerializer.h"

#include "PawnCosmeticsComponent.generated.h"

class UPawnCosmeticsComponent;
namespace EEndPlayReason { enum Type : int; }
struct FGameplayTag;
struct FPawnCosmeticList;

class AActor;
class UChildActorComponent;
class UObject;
class USceneComponent;
class USkeletalMeshComponent;
struct FFrame;
struct FNetDeltaSerializeInfo;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSpawnedCosmeticsChanged, UPawnCosmeticsComponent*, ComponentWithChangedParts);

//////////////////////////////////////////////////////////////////////

// A single applied character part
USTRUCT()
struct FAppliedPawnCosmeticEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FAppliedPawnCosmeticEntry()
	{}

	FString GetDebugString() const;

private:
	friend FPawnCosmeticList;
	friend UPawnCosmeticsComponent;

private:
	// The character part being represented
	UPROPERTY()
	FPawnCosmetic Part;

	// Handle index we returned to the user (server only)
	UPROPERTY(NotReplicated)
	int32 PartHandle = INDEX_NONE;

	// The spawned actor instance (client only)
	UPROPERTY(NotReplicated)
	TObjectPtr<UChildActorComponent> SpawnedComponent = nullptr;
};

//////////////////////////////////////////////////////////////////////

// Replicated list of applied character parts
USTRUCT(BlueprintType)
struct FPawnCosmeticList : public FFastArraySerializer
{
	GENERATED_BODY()

	FPawnCosmeticList()
		: OwnerComponent(nullptr)
	{
	}

public:
	//~FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32> ChangedIndices, int32 FinalSize);
	//~End of FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParms)
	{
		return FFastArraySerializer::FastArrayDeltaSerialize<FAppliedPawnCosmeticEntry, FPawnCosmeticList>(Entries, DeltaParms, *this);
	}

	FPawnCosmeticPartHandle AddEntry(FPawnCosmetic NewPart);
	void RemoveEntry(FPawnCosmeticPartHandle Handle);
	void ClearAllEntries(bool bBroadcastChangeDelegate);

	FGameplayTagContainer CollectCombinedTags() const;

	void SetOwnerComponent(UPawnCosmeticsComponent* InOwnerComponent)
	{
		OwnerComponent = InOwnerComponent;
	}
	
private:
	friend UPawnCosmeticsComponent;

	bool SpawnActorForEntry(FAppliedPawnCosmeticEntry& Entry);
	bool DestroyActorForEntry(FAppliedPawnCosmeticEntry& Entry);

private:
	// Replicated list of equipment entries
	UPROPERTY()
	TArray<FAppliedPawnCosmeticEntry> Entries;

	// The component that contains this list
	UPROPERTY(NotReplicated)
	TObjectPtr<UPawnCosmeticsComponent> OwnerComponent;

	// Upcounter for handles
	int32 PartHandleCounter = 0;
};

template<>
struct TStructOpsTypeTraits<FPawnCosmeticList> : public TStructOpsTypeTraitsBase2<FPawnCosmeticList>
{
	enum { WithNetDeltaSerializer = true };
};

//////////////////////////////////////////////////////////////////////

// A component that handles spawning cosmetic actors attached to the owner pawn on all clients
UCLASS(meta=(BlueprintSpawnableComponent))
class UPawnCosmeticsComponent : public UPawnComponent
{
	GENERATED_BODY()

public:
	UPawnCosmeticsComponent(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UActorComponent interface
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void OnRegister() override;
	//~End of UActorComponent interface

	// Adds a character part to the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	FPawnCosmeticPartHandle AddCharacterPart(const FPawnCosmetic& NewPart);

	// Removes a previously added character part from the actor that owns this customization component, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveCharacterPart(FPawnCosmeticPartHandle Handle);

	// Removes all added character parts, should be called on the authority only
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category=Cosmetics)
	void RemoveAllCharacterParts();

	// Gets the list of all spawned character parts from this component
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	TArray<AActor*> GetCharacterPartActors() const;

	// If the parent actor is derived from ACharacter, returns the Mesh component, otherwise nullptr
	UFUNCTION(BlueprintCallable)
	USkeletalMeshComponent* GetParentMeshComponent() const;

	// Returns the scene component to attach the spawned actors to
	// If the parent actor is derived from ACharacter, we'll use the Mesh component, otherwise the root component
	UFUNCTION(BlueprintCallable)
	USceneComponent* GetSceneComponentToAttachTo() const;

	// Returns the set of combined gameplay tags from attached character parts, optionally filtered to only tags that start with the specified root
	UFUNCTION(BlueprintCallable, BlueprintPure=false, BlueprintCosmetic, Category=Cosmetics)
	FGameplayTagContainer GetCombinedTags(FGameplayTag RequiredPrefix) const;

	void BroadcastChanged();

public:
	// Delegate that will be called when the list of spawned character parts has changed
	UPROPERTY(BlueprintAssignable, Category=Cosmetics, BlueprintCallable)
	FSpawnedCosmeticsChanged OnCharacterPartsChanged;

private:
	// List of character parts
	UPROPERTY(Replicated, Transient)
	FPawnCosmeticList CharacterPartList;

	// Rules for how to pick a body style mesh for animation to play on, based on character part cosmetics tags
	UPROPERTY(EditAnywhere, Category=Cosmetics)
	FAnimBodyStyleSelectionSet BodyMeshes;
};
