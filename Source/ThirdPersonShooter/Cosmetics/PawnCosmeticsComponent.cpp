#include "PawnCosmeticsComponent.h"
#include "Inventory/InventoryManagerComponent.h"
#include "Equipment/EquipmentManagerComponent.h"

UPawnCosmeticsComponent::UPawnCosmeticsComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	bWantsInitializeComponent = true;
}

void UPawnCosmeticsComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UPawnCosmeticsComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);
}

void UPawnCosmeticsComponent::InitializeComponent()
{
	Super::InitializeComponent();
}

void UPawnCosmeticsComponent::GetSpawnedActors(TArray<AActor*>& Cosmetics)
{
	if (UInventoryManagerComponent* IMC = GetOwner()->GetComponentByClass<UInventoryManagerComponent>())
	{
		for (FReadOnlyInventoryEntry Entry : IMC->GetAllItems())
		{
			for (AActor* Actor : Entry.Instance->GetSpawnedActors())
			{
				Cosmetics.Emplace(Actor);
			}
		}
	}

	if (UEquipmentManagerComponent* EMC = GetOwner()->GetComponentByClass<UEquipmentManagerComponent>())
	{
		for (UEquipmentItemInstance* Item : EMC->GetAllItems())
		{
			for (AActor* Actor : Item->GetSpawnedActors())
			{
				Cosmetics.Emplace(Actor);
			}
		}
	}
}
