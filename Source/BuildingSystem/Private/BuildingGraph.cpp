// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingGraph.h"

#include "BuildingGraphVertex.h"
#include "BuildingSystem.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

TObjectPtr<UGraphVertex> UBuildingGraph::CreateTypedVertex() const
{
	return NewObject<UBuildingGraphVertex>(const_cast<UGraph*>(Cast<UGraph>(this)), UBuildingGraphVertex::StaticClass());
}

bool UBuildingGraph::IsIslandHasRoot(const FGraphIslandHandle& Island) const
{
	for (auto Vertex : Island.GetIsland()->GetVertices())
	{
		auto BuildingGraphVertex = Cast<UBuildingGraphVertex>(Vertex.GetVertex());
		if (BuildingGraphVertex->bIsRoot)
		{
			return true;
		}
	}

	return false;
}

void UBuildingGraph::UpdateIslandState(const FGraphIslandHandle& Island)
{
	if (!IsIslandHasRoot(Island))
	{
		UE_LOG(LogTemp, Warning, TEXT("Sim physics"))
		auto FirstVertex = Island.GetIsland()->GetVertices().Array()[0];
		const auto FirstBuildingVtx = Cast<UBuildingGraphVertex>(FirstVertex.GetVertex());
		
		//Island.GetIsland()->ForEachVertex([FirstBuildingVtx](const FGraphVertexHandle& Vh)
		//{
		//	const auto BuildingVtx = Cast<UBuildingGraphVertex>(Vh.GetVertex());
		//	BuildingVtx->PrimitiveComponent->UnWeldFromParent();
		//	
		//	if (FirstBuildingVtx != BuildingVtx)
		//	{
		//		BuildingVtx->PrimitiveComponent->WeldTo(FirstBuildingVtx->PrimitiveComponent);
		//	}
		//});
		//FirstBuildingVtx->PrimitiveComponent->SetSimulatePhysics(true);
	}
}

void TBuildingGraphSerialization ::WriteGraphVertex(const FGraphVertexHandle& VertexHandle, const UGraphVertex* Vertex)
{
	UE_LOG(LogBuildingSystem, Warning, TEXT("Test BuildingGraphSerialization Called!"));

	TDefaultGraphSerialization::WriteGraphVertex(VertexHandle, Vertex);

	const auto VertexData = FSerializableBuildingVertex(VertexHandle, Cast<UBuildingGraphVertex>(Vertex)->bIsRoot);
	Data.BuildingVertices.Add(VertexData);
}

void TBuildingGraphDeserialization::OnDeserializedVertex(const FGraphVertexHandle& VertexHandle) const
{
	UE_LOG(LogBuildingSystem, Warning, TEXT("Test BuildingGraphDeserialization Called!"));

	const auto Found = Data.BuildingVertices.FindByPredicate([VertexHandle](const FSerializableBuildingVertex& Vertex)
	{
		return Vertex.Handle == VertexHandle;
	});

	if (Found)
	{
		if (const auto VertexObject = Cast<UBuildingGraphVertex>(VertexHandle.GetVertex()))
		{
			if (Found->bIsRooted)
			{
				UE_LOG(LogBuildingSystem, Warning, TEXT("Is Rooted"))
			}
			else
			{
				UE_LOG(LogBuildingSystem, Warning, TEXT("Is Not Rooted"))
			}
			VertexObject->bIsRoot = Found->bIsRooted;
		}
	}
}
