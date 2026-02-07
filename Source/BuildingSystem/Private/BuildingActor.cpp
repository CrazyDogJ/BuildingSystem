// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingActor.h"

#include "BuildingActorDescription.h"
#include "BuildingDefinition.h"
#include "BuildingManagerSubsystem.h"
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
}

void ABuildingActor::ConstructActor()
{
	if (BuildingDefinition && BuildingDefinition->BuildingActorDescription)
	{
		// Call user def construction event.
		BuildingDefinition->BuildingActorDescription->BP_BuildingActorConstructionEvent(this);
	}
}

void ABuildingActor::BeginPlay()
{
	Super::BeginPlay();

	GetBuildingManagerSubsystem()->RegisterBuildingActor(this);
}

void ABuildingActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	GetBuildingManagerSubsystem()->UnregisterBuildingActor(this);
}

void ABuildingActor::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BuildingDefinition);
}
