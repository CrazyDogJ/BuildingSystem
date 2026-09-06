// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/BuildingGraph.h"

#include "BuildingLogicObject.h"
#include "Graph/BuildingGraphVertex.h"
#include "Graph/BuildingGraphIsland.h"
#include "BuildingSystem.h"

TObjectPtr<UGraphIsland> UBuildingGraph::CreateTypedIsland() const
{
	return NewObject<UBuildingGraphIsland>(const_cast<UGraph*>(Cast<UGraph>(this)), UBuildingGraphIsland::StaticClass());
}

TObjectPtr<UGraphVertex> UBuildingGraph::CreateTypedVertex() const
{
	return NewObject<UBuildingGraphVertex>(const_cast<UGraph*>(Cast<UGraph>(this)), UBuildingGraphVertex::StaticClass());
}

bool UBuildingGraph::IsIslandRooted(const FGraphIslandHandle& Island)
{
	if (const auto BuildingIsland = Cast<UBuildingGraphIsland>(Island.GetIsland()))
	{
		return BuildingIsland->IsIslandRooted();
	}

	return false;
}

void TBuildingGraphSerialization::WriteGraphVertex(const FGraphVertexHandle& VertexHandle, const UGraphVertex* Vertex)
{
	//UE_LOG(LogBuildingSystem, Warning, TEXT("Test BuildingGraphSerialization Called!"));

	TDefaultGraphSerialization::WriteGraphVertex(VertexHandle, Vertex);

	const auto VertexObject = Cast<UBuildingGraphVertex>(Vertex);
	TArray<uint8> SavedData;
	if (VertexObject->BuildingLogicObject)
	{
		VertexObject->BuildingLogicObject->GetSaveData(SavedData);
	}
	const auto VertexData = FSerializableBuildingVertex(VertexObject->bIsRoot, SavedData);
	Data.BuildingVertices.Add(VertexHandle, VertexData);
}

void TBuildingGraphDeserialization::OnDeserializedVertex(const FGraphVertexHandle& VertexHandle) const
{
	//UE_LOG(LogBuildingSystem, Warning, TEXT("Test BuildingGraphDeserialization Called!"));

	if (const auto Found = Data.BuildingVertices.Find(VertexHandle))
	{
		if (const auto VertexObject = Cast<UBuildingGraphVertex>(VertexHandle.GetVertex()))
		{
			//const FString LogString = Found->bIsRooted ? "Is Rooted" : "Is Not Rooted";
			//UE_LOG(LogBuildingSystem, Warning, TEXT("%s"), *LogString);
			
			VertexObject->bIsRoot = Found->bIsRooted;
			if (!Found->LogicObjectData.IsEmpty())
			{
				VertexObject->BuildingLogicObject = NewObject<UBuildingLogicObject>(VertexObject->GetOuterBuildingGraph());
				if (VertexObject->BuildingLogicObject)
				{
					VertexObject->BuildingLogicObject->LoadSaveData(Found->LogicObjectData);
				}
			}
		}
	}
}
