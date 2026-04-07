// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/BuildingGraphVertex.h"

#include "Graph/BuildingGraph.h"
#include "BuildingManagerSubsystem.h"

UBuildingGraph* UBuildingGraphVertex::GetOuterBuildingGraph() const
{
	return Cast<UBuildingGraph>(GetGraph());
}

UBuildingManagerSubsystem* UBuildingGraphVertex::GetOuterBuildingSubsystem() const
{
	if (const auto OuterGraph = GetOuterBuildingGraph())
	{
		return Cast<UBuildingManagerSubsystem>(OuterGraph->GetOuter());
	}
	
	return nullptr;
}

ABuildingActor* UBuildingGraphVertex::GetBuildingActor() const
{
	if (const auto Subsystem = GetOuterBuildingSubsystem())
	{
		const auto Actors = Subsystem->GetRegisteredBuildingActors();
		if (const auto Found = Actors.Find(Handle()))
		{
			return *Found;
		}
	}

	return nullptr;
}

void UBuildingGraphVertex::HandleOnVertexRemoved()
{
	if (const auto Actor = GetBuildingActor())
	{
		Actor->Destroy(true);
	}
	
	Super::HandleOnVertexRemoved();
}
