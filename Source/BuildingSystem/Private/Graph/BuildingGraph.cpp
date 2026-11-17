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
	TDefaultGraphSerialization::WriteGraphVertex(VertexHandle, Vertex);
	const auto* BuildingVertex = CastChecked<UBuildingGraphVertex>(Vertex);
	auto& Entry = Data.BuildingVertices.FindOrAdd(VertexHandle);
	Entry.bIsRooted = BuildingVertex->bIsRoot;
	Entry.LogicClass = BuildingVertex->PendingLogicClass;
	Entry.LogicObjectData = BuildingVertex->PendingLogicData;
	if (const auto* Logic = BuildingVertex->BuildingLogicObject.Get(); Logic && Entry.LogicObjectData.IsEmpty() && Entry.LogicClass.IsNull())
	{
		Entry.LogicClass = Logic->GetClass();
		BuildingVertex->BuildingLogicObject->GetSaveData(Entry.LogicObjectData);
	}
}

void TBuildingGraphDeserialization::OnDeserializedVertex(const FGraphVertexHandle& VertexHandle) const
{
	const auto* Entry = Data.BuildingVertices.Find(VertexHandle);
	auto* Vertex = Cast<UBuildingGraphVertex>(VertexHandle.GetVertex());
	if (!Entry || !Vertex) return;

	Vertex->bIsRoot = Entry->bIsRooted;
	Vertex->PendingLogicClass = Entry->LogicClass;
	Vertex->PendingLogicData = Entry->LogicObjectData;
	if (bCreateLogicObjects) Vertex->InitializeLogicObject();
}
