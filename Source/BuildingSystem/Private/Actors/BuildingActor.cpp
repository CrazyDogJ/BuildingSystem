// Fill out your copyright notice in the Description page of Project Settings.

#include "Actors/BuildingActor.h"

#include "BuildingActorDescription.h"
#include "BuildingBehaviorDefinition.h"
#include "BuildingDefinition.h"
#include "Actors/BuildingGraphData.h"
#include "BuildingManagerSubsystem.h"
#include "SmartObjectComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

ABuildingActor::ABuildingActor()
{
	bReplicates = true;
	SetReplicatingMovement(true);

	PrimaryActorTick.bCanEverTick = true;
	
	RootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootStaticMesh"));
	RootStaticMesh->SetIsReplicated(true);
	SetRootComponent(RootStaticMesh);
}

UBuildingManagerSubsystem* ABuildingActor::GetBuildingManagerSubsystem() const
{
	return GetWorld()->GetSubsystem<UBuildingManagerSubsystem>();
}

void ABuildingActor::OnRep_BuildingDefinition()
{
	InitSmartObject();
}

void ABuildingActor::SetBuildingDefinition(UBuildingDefinition* InBuildingDefinition)
{
	if (!InBuildingDefinition)
	{
		return;
	}
	
	BuildingDefinition = InBuildingDefinition;
	ConstructActor();
}

void ABuildingActor::PostGameLoaded()
{
	ConstructActor();
	RegisterVertexHandle();
}

void ABuildingActor::ConstructActor()
{
	if (BuildingDefinition && BuildingDefinition->BuildingActorDescription)
	{
		RootStaticMesh->SetStaticMesh(BuildingDefinition->BuildingActorDescription->BuildingMesh);
		InitSmartObject();
		
		// Call user def construction event.
		BuildingDefinition->BuildingActorDescription->BP_BuildingActorConstructionEvent(this);
	}
}

void ABuildingActor::InitSmartObject() const
{
	const auto SmartObjectComp = GetComponentByClass<USmartObjectComponent>();
	if (BuildingDefinition && SmartObjectComp)
	{
		if (const auto Subsystem = GetWorld()->GetSubsystem<UBuildingManagerSubsystem>())
		{
			if (const auto Desc = Subsystem->GetBuildingActorDescription(BuildingDefinition))
			{
				SmartObjectComp->SetDefinition(Desc->BuildingSmartObjectDefinition);
			}
		}
	}
}

void ABuildingActor::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

	ConstructActor();
}

void ABuildingActor::BeginPlay()
{
	Super::BeginPlay();

	RegisterVertexHandle();
}

void ABuildingActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	UnregisterVertexHandle();
}

void ABuildingActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BuildingDefinition);
}

const USmartObjectDefinition* ABuildingActor::TryGetSmartObjectDefinition() const
{
	if (const auto SmartObjectComp = GetComponentByClass<USmartObjectComponent>())
	{
		if (auto Definition = SmartObjectComp->GetDefinition())
		{
			return Definition;
		}
	}

	return nullptr;
}

bool ABuildingActor::TryGetSlotDefinitionByIndex(int Index, FSmartObjectSlotDefinition& OutDefinition) const
{
	if (const auto Definition = TryGetSmartObjectDefinition())
	{
		if (Definition->GetSlots().IsValidIndex(Index))
		{
			OutDefinition = Definition->GetSlots()[Index];
			return true;
		}
	}

	OutDefinition = FSmartObjectSlotDefinition();
	return false;
}

bool ABuildingActor::TryGetNearestSlotDefinition(const FVector& Location, FSmartObjectSlotDefinition& OutDefinition) const
{
	float Distance = -1.0f;
	int Index = INDEX_NONE;
	if (const auto Definition = TryGetSmartObjectDefinition())
	{
		for (int i = 0; i < Definition->GetSlots().Num(); ++i)
		{
			const auto SlotTransform = Definition->GetSlotWorldTransform(i, GetTransform());
			const auto CurrentDistance = FVector::Distance(SlotTransform.GetLocation(), Location);
			if (Distance < 0.0f || CurrentDistance < Distance)
			{
				Distance = CurrentDistance;
				Index = i;
			}
		}

		if (Definition->GetSlots().IsValidIndex(Index))
		{
			OutDefinition = Definition->GetSlots()[Index];
			return true;
		}
	}

	OutDefinition = FSmartObjectSlotDefinition();
	return false;
}

bool ABuildingActor::TryGetBuildingBehaviorDefinition(const FSmartObjectSlotDefinition& InSlot,
	UBuildingBehaviorDefinition*& OutBuildingBehaviorDefinition) const
{
	for (const auto Behavior : InSlot.BehaviorDefinitions)
	{
		if (const auto Casted = Cast<UBuildingBehaviorDefinition>(Behavior))
		{
			OutBuildingBehaviorDefinition = Casted;
			return true;
		}
	}

	OutBuildingBehaviorDefinition = nullptr;
	return false;
}

#if WITH_EDITOR
void ABuildingActor::UpdateBuildingGraph() const
{
	const auto Actor = UGameplayStatics::GetActorOfClass(this, ABuildingGraphData::StaticClass());
	if (const auto BuildingData = Cast<ABuildingGraphData>(Actor))
	{
		BuildingData->UpdateBuildingGraph();
	}
}
#endif

void ABuildingActor::SetGraphVertexHandle(const FGraphVertexHandle& InGraphVertexHandle)
{
	GraphVertexHandle = InGraphVertexHandle;
	RegisterVertexHandle();
}

bool ABuildingActor::CanRemoveBuildingActor_Implementation() const
{
	return true;
}

void ABuildingActor::RegisterVertexHandle()
{
	const auto Subsystem = GetBuildingManagerSubsystem();
	if (Subsystem && GraphVertexHandle.IsValid())
	{
		Subsystem->RegisterBuildingActor(this);
	}
}

void ABuildingActor::UnregisterVertexHandle() const
{
	if (const auto Subsystem = GetBuildingManagerSubsystem())
	{
		Subsystem->UnregisterBuildingActor(this);
	}
}
