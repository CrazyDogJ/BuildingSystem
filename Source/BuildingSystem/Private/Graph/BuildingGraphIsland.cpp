// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/BuildingGraphIsland.h"

#include "Graph/BuildingGraphVertex.h"

bool UBuildingGraphIsland::IsIslandRooted() const
{
	return RootedVertices.Num() > 0;
}

void UBuildingGraphIsland::ChangeVertexHandle(const FGraphVertexHandle& OldVertexHandle,
	const FGraphVertexHandle& NewVertexHandle)
{
	Super::ChangeVertexHandle(OldVertexHandle, NewVertexHandle);

	RootedVertices.Remove(OldVertexHandle.GetVertex());

	if (const auto BuildingVertex = Cast<UBuildingGraphVertex>(NewVertexHandle.GetVertex()))
	{
		if (BuildingVertex->bIsRoot)
		{
			RootedVertices.Add(NewVertexHandle.GetVertex());
		}
	}
}

void UBuildingGraphIsland::HandleOnVertexAdded(const FGraphVertexHandle& Handle)
{
	Super::HandleOnVertexAdded(Handle);

	if (const auto BuildingVertex = Cast<UBuildingGraphVertex>(Handle.GetVertex()))
	{
		if (BuildingVertex->bIsRoot)
		{
			RootedVertices.Add(Handle.GetVertex());
		}
	}
}

void UBuildingGraphIsland::HandleOnVertexRemoved(const FGraphVertexHandle& Handle)
{
	RootedVertices.Remove(Handle.GetVertex());
	
	Super::HandleOnVertexRemoved(Handle);
}
