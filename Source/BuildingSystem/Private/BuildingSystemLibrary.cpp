// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingSystemLibrary.h"

#include "BuildingGraph.h"
#include "BuildingGraphVertex.h"
#include "BuildingManagerSubsystem.h"

UBuildingGraph* UBuildingSystemLibrary::NewBuildingGraph()
{
	return NewObject<UBuildingGraph>();
}

int UBuildingSystemLibrary::GetIslandCount(UBuildingGraph* BuildingGraph)
{
	if (BuildingGraph)
	{
		return BuildingGraph->GetIslands().Num();
	}
	
	return -1;
}

FGraphVertexHandle_BP UBuildingSystemLibrary::AddComponentToBuildingGraph(UPrimitiveComponent* Comp,
                                                                          UBuildingGraph* BuildingGraph)
{
	if (!Comp || !BuildingGraph)
	{
		return FGraphVertexHandle_BP();
	}

	const auto NewVertexHandle = BuildingGraph->CreateVertex();
	const auto BuildingGraphVtx = Cast<UBuildingGraphVertex>(NewVertexHandle.GetVertex());
	BuildingGraphVtx->PrimitiveComponent = Comp;

	return NewVertexHandle;
}

FGraphVertexHandle_BP UBuildingSystemLibrary::FindVertexHandleByComp(UPrimitiveComponent* Comp,
	UBuildingGraph* BuildingGraph)
{
	if (!BuildingGraph)
	{
		return FGraphVertexHandle_BP();
	}
	
	for (auto Vertex : BuildingGraph->GetVertices())
	{
		auto BuildingVtx = Cast<UBuildingGraphVertex>(Vertex.Value);
		if (BuildingVtx && BuildingVtx->PrimitiveComponent == Comp)
		{
			return Vertex.Key;
		}
	}

	return FGraphVertexHandle_BP();
}

void UBuildingSystemLibrary::RemoveVertexByComp(UPrimitiveComponent* Comp, UBuildingGraph* BuildingGraph)
{
	if (!BuildingGraph) return;

	if (const auto FoundVertex = FindVertexHandleByComp(Comp, BuildingGraph); FoundVertex.IsValid())
	{
		auto CachedEdges = FoundVertex.GetVertex()->GetEdges();
		BuildingGraph->RemoveVertex(FoundVertex);

		for (auto Vertex : CachedEdges)
		{
			BuildingGraph->UpdateIslandState(Vertex.GetVertex()->GetParentIsland());
		}

		Comp->GetOwner()->Destroy();
	}
}

bool UBuildingSystemLibrary::SetCompRooted(bool bIsRoot, UPrimitiveComponent* Comp, UBuildingGraph* BuildingGraph)
{
	auto Handle = FindVertexHandleByComp(Comp, BuildingGraph);
	if (Handle.IsValid())
	{
		auto BuildingVertex = Cast<UBuildingGraphVertex>(Handle.GetVertex());
		BuildingVertex->bIsRoot = bIsRoot;
		return true;
	}

	return false;
}

void UBuildingSystemLibrary::ConnectVertexes(const FGraphVertexHandle& Vtx1, const FGraphVertexHandle& Vtx2,
                                             UBuildingGraph* BuildingGraph)
{
	if (!BuildingGraph) return;

	TArray<FEdgeSpecifier> NodesToConnect;
	NodesToConnect.Add(FEdgeSpecifier(Vtx1, Vtx2));
	BuildingGraph->CreateBulkEdges(MoveTemp(NodesToConnect));
}

void UBuildingSystemLibrary::ConnectVertexesByComp(UPrimitiveComponent* Comp1, UPrimitiveComponent* Comp2,
	UBuildingGraph* BuildingGraph)
{
	if (!BuildingGraph || !Comp1 || !Comp2)
	{
		return;
	}
	
	auto FoundComp1Handle = FindVertexHandleByComp(Comp1, BuildingGraph);
	auto FoundComp2Handle = FindVertexHandleByComp(Comp2, BuildingGraph);

	if (!FoundComp1Handle.IsValid())
	{
		FoundComp1Handle = AddComponentToBuildingGraph(Comp1, BuildingGraph);
	}

	if (!FoundComp2Handle.IsValid())
	{
		FoundComp2Handle = AddComponentToBuildingGraph(Comp2, BuildingGraph);
	}

	ConnectVertexes(FoundComp1Handle, FoundComp2Handle, BuildingGraph);
}

FString UBuildingSystemLibrary::GetDebugVertexString(const UObject* WorldContextObject)
{
	FString Result;
	
	const auto Subsystem = WorldContextObject->GetWorld()->GetSubsystem<UBuildingManagerSubsystem>();
	const auto Graph = Subsystem->GetBuildingGraph();
	
	if (Graph)
	{
		Result = FString::Printf(TEXT("%i"), Graph->NumVertices());
		for (auto Vertex : Graph->GetVertices())
		{
			if (Vertex.Value)
			{
				if (const auto GraphVertex = Cast<UBuildingGraphVertex>(Vertex.Value))
				{
					Result += (GraphVertex->bIsRoot ? "true" : "false");
				}
				else
				{
					Result += FString::Printf(TEXT("%s"), *Vertex.Value.GetClass()->GetName());
				}
			}
		}
	}

	return Result;
}
