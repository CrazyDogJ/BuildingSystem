// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingManagerSubsystem.h"

#include "BuildingDefinition.h"
#include "Actors/BuildingActor.h"
#include "Actors/BuildingGraphData.h"
#include "Graph/BuildingGraph.h"
#include "Graph/BuildingGraphIsland.h"
#include "Graph/BuildingGraphVertex.h"

static TAutoConsoleVariable<bool> CVarDrawDebugBuildingGraph(
	TEXT("BuildingSystem.DrawDebugGraph"),
	false,
	TEXT("Draw debug building graph."));


void UBuildingManagerSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	InitBuildingGraph();
}

void UBuildingManagerSubsystem::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (CVarDrawDebugBuildingGraph.GetValueOnAnyThread())
	{
		DrawGraphDebug();
	}
}

void UBuildingManagerSubsystem::Deinitialize()
{
	Super::Deinitialize();

	DestroyBuildingGraph();
	// Clear bad.
	BuildingActorDescriptions.Empty();
}

void UBuildingManagerSubsystem::InitBuildingGraph()
{
	GlobalBuildingGraph = NewObject<UBuildingGraph>(this);
}

void UBuildingManagerSubsystem::LoadLevelBuildingGraphData(const ABuildingGraphData* BuildingGraphData) const
{
	if (!BuildingGraphData)
	{
		return;
	}
	
	GlobalBuildingGraph->Empty();
	const TBuildingGraphDeserialization Deserializer(BuildingGraphData->BuildingGraphData);
	Deserializer >> *GlobalBuildingGraph;
}

void UBuildingManagerSubsystem::DestroyBuildingGraph()
{
	GlobalBuildingGraph->ConditionalBeginDestroy();
	GlobalBuildingGraph = nullptr;
}

UBuildingGraph* UBuildingManagerSubsystem::GetBuildingGraph() const
{
	return GlobalBuildingGraph;
}

FSerializableBuildingGraph UBuildingManagerSubsystem::GetGraphSaveGameData()
{
	if (GlobalBuildingGraph)
	{
		TBuildingGraphSerialization Serializer;
		Serializer << *GlobalBuildingGraph;
		const FSerializableBuildingGraph& SerializedData = Serializer.GetData();
		
		return SerializedData;
	}
	return FSerializableBuildingGraph();
}

void UBuildingManagerSubsystem::LoadGraphSaveGameData(const FSerializableBuildingGraph& Data)
{
	if (!GlobalBuildingGraph)
	{
		InitBuildingGraph();
	}

	const TBuildingGraphDeserialization Deserializer(Data);
	Deserializer >> *GlobalBuildingGraph;
}

ABuildingActor* UBuildingManagerSubsystem::GetBuildingActorByVertexHandle(const FGraphVertexHandle& VertexHandle)
{
	if (const auto Found = RegisteredBuildingActors.Find(VertexHandle))
	{
		return *Found;
	}

	return nullptr;
}

void UBuildingManagerSubsystem::DrawGraphDebug()
{
	if (!GlobalBuildingGraph) return;
	
	for (auto Vertex : GlobalBuildingGraph->GetVertices())
	{
		if (const auto BuildingVtx = Cast<UBuildingGraphVertex>(Vertex.Value))
		{
			const auto CurrentVtxActor = GetBuildingActorByVertexHandle(Vertex.Key);
			if (!CurrentVtxActor) continue;
			
			auto VertexPos = CurrentVtxActor->GetActorLocation();
			DrawDebugBox(GetWorld(), VertexPos, FVector(10),
				BuildingVtx->bIsRoot ? FColor::Red : FColor::Yellow, false, 0, 255, 4);
			Vertex.Value->ForEachAdjacentVertex([this, &VertexPos](const FGraphVertexHandle& Vertex)
			{
				const auto EdgeVertexActor = GetBuildingActorByVertexHandle(Vertex);
				if (!EdgeVertexActor) return;
				
				const auto EdgeVertexPos = EdgeVertexActor->GetActorLocation();
				DrawDebugLine(GetWorld(), EdgeVertexPos, VertexPos,
					FColor::Green, false, 0, 255, 6);
			});
		}
	}

	for (const auto Island : GlobalBuildingGraph->GetIslands())
	{
		const auto BuildingIsland = Cast<UBuildingGraphIsland>(Island.Value);
		const auto Array = Island.Value->GetVertices().Array();
		if (Array.IsValidIndex(0))
		{
			if (const auto BuildingActor = GetBuildingActorByVertexHandle(Array[0]))
			{
				FString DebugString = BuildingIsland->IsIslandRooted() ? "Is Root Island" : "Island Not Rooted";
				DrawDebugString(GetWorld(), FVector(), DebugString, BuildingActor, FColor::Red, 0);
			}
		}
	}
}

void UBuildingManagerSubsystem::RegisterBuildingActor(ABuildingActor* BuildingActor)
{
	RegisteredBuildingActors.Add(BuildingActor->GetGraphVertexHandle(), BuildingActor);
}

void UBuildingManagerSubsystem::UnregisterBuildingActor(const ABuildingActor* BuildingActor)
{
	if (BuildingActor)
	{
		RegisteredBuildingActors.Remove(BuildingActor->GetGraphVertexHandle());
	}
}

TMap<FGraphVertexHandle, ABuildingActor*> UBuildingManagerSubsystem::GetRegisteredBuildingActors() const
{
	return RegisteredBuildingActors;
}

void UBuildingManagerSubsystem::AddVertex(ABuildingActor* BuildingActor, const bool bIsRooted)
{
	if (!BuildingActor) return;
	
	const auto NewVertexHandle = GlobalBuildingGraph->CreateVertex();
	UBuildingGraphVertex* BuildingGraphVtx = Cast<UBuildingGraphVertex>(NewVertexHandle.GetVertex());
	BuildingGraphVtx->bIsRoot = bIsRooted;
	BuildingActor->SetGraphVertexHandle(NewVertexHandle);
}

void UBuildingManagerSubsystem::RemoveVertexDirectly(ABuildingActor* BuildingActor)
{
	if (!BuildingActor) return;
	if (!BuildingActor->CanRemoveBuildingActor())
	{
		return;
	}
	
	GlobalBuildingGraph->RemoveVertex(BuildingActor->GetGraphVertexHandle());
}

void UBuildingManagerSubsystem::RemoveVertex(ABuildingActor* BuildingActor)
{
	if (!BuildingActor) return;
	if (!BuildingActor->CanRemoveBuildingActor())
	{
		return;
	}
	
	if (const auto VertexPtr = GlobalBuildingGraph->GetSafeVertexFromHandle(BuildingActor->GetGraphVertexHandle()))
	{
		TArray<UGraphVertex*> AdjacentVertexPtr;
		VertexPtr->ForEachAdjacentVertex([&AdjacentVertexPtr](const FGraphVertexHandle& Vertex)
		{
			AdjacentVertexPtr.Add(Vertex.GetVertex());
		});

		RemoveVertexDirectly(BuildingActor);
		
		for (const auto Itr : AdjacentVertexPtr)
		{
			const auto ParentIsland = Itr->GetParentIsland();
			if (const auto IslandPtr = Cast<UBuildingGraphIsland>(ParentIsland.GetIsland()))
			{
				if (IslandPtr && !IslandPtr->IsIslandRooted())
				{
					const auto VertexArray = IslandPtr->GetVertices();
					for (const auto ItrPtr : VertexArray)
					{
						if (const auto FoundBuildingActor = GetBuildingActorByVertexHandle(ItrPtr))
						{
							FoundBuildingActor->OnBuildingIslandNotRoot();
						}
					}
				}
			}
		}
	}
}

bool UBuildingManagerSubsystem::IsIslandRooted(ABuildingActor* BuildingActor)
{
	if (!BuildingActor) return false;

	if (const auto VertexPtr = GlobalBuildingGraph->GetSafeVertexFromHandle(BuildingActor->GetGraphVertexHandle()))
	{
		return IsIslandRooted(VertexPtr);
	}

	return false;
}

bool UBuildingManagerSubsystem::IsIslandRooted(const UGraphVertex* Vertex) const
{
	if (Vertex)
	{
		return GlobalBuildingGraph->IsIslandRooted(Vertex->GetParentIsland());
	}

	return false;
}

void UBuildingManagerSubsystem::ConnectVertex(ABuildingActor* BuildingActor1, ABuildingActor* BuildingActor2)
{
	if (BuildingActor1 && BuildingActor2)
	{
		TArray<FEdgeSpecifier> NodesToConnect;
		NodesToConnect.Add(FEdgeSpecifier(BuildingActor1->GetGraphVertexHandle(), BuildingActor2->GetGraphVertexHandle()));
		GlobalBuildingGraph->CreateBulkEdges(MoveTemp(NodesToConnect));
	}
}

void UBuildingManagerSubsystem::ConnectNeighbourVertices(ABuildingActor* BuildingActor,
	TArray<ABuildingActor*> NeighbourActors)
{
	if (BuildingActor && NeighbourActors.Num() > 0)
	{
		TArray<FEdgeSpecifier> NodesToConnect;
		for (auto Actor : NeighbourActors)
		{
			if (BuildingActor != Actor)
			{
				NodesToConnect.Add(FEdgeSpecifier(BuildingActor->GetGraphVertexHandle(), Actor->GetGraphVertexHandle()));
			}
		}
		GlobalBuildingGraph->CreateBulkEdges(MoveTemp(NodesToConnect));
	}
}

TArray<ABuildingActor*> UBuildingManagerSubsystem::GetNeighbourActors(ABuildingActor* BuildingActor)
{
	TArray<ABuildingActor*> NeighbourActors;
	
	const auto ActorVertexHandle = BuildingActor->GetGraphVertexHandle();
	const auto FoundVertexObject = GlobalBuildingGraph->GetVertices().Find(ActorVertexHandle);
	if (FoundVertexObject != nullptr)
	{
		const auto GraphVertex = *FoundVertexObject;
		GraphVertex->ForEachAdjacentVertex([this, &NeighbourActors](const FGraphVertexHandle& Vertex)
		{
			if (const auto FoundBuildingActor = RegisteredBuildingActors.Find(Vertex); FoundBuildingActor != nullptr)
			{
				NeighbourActors.Add(*FoundBuildingActor);
			}
		});
	}

	return NeighbourActors;
}

TArray<ABuildingActor*> UBuildingManagerSubsystem::QueryNearbyActors(TArray<FVector> Locations, float Threshold)
{
	TArray<ABuildingActor*> Result;
	
	for (auto Actor : RegisteredBuildingActors)
	{
		for (auto Vector : Locations)
		{
			if (Vector.Equals(Actor.Value->GetActorLocation(), Threshold))
			{
				Result.Add(Actor.Value);
			}
		}
	}

	return Result;
}

UBuildingActorDescription* UBuildingManagerSubsystem::GetBuildingActorDescription(
	const UBuildingDefinition* BuildingDefinition)
{
	if (!BuildingDefinition || !BuildingDefinition->BuildingActorDescription)
	{
		return nullptr;
	}

	if (const auto Found = BuildingActorDescriptions.Find(BuildingDefinition))
	{
		return *Found;
	}

	if (const auto NewBAD = DuplicateObject(BuildingDefinition->BuildingActorDescription, this))
	{
		BuildingActorDescriptions.Add(BuildingDefinition, NewBAD);
		return NewBAD;
	}
	
	return nullptr;
}

