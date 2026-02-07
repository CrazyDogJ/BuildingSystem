// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingManagerSubsystem.h"

#include "BuildingActor.h"
#include "BuildingGraph.h"
#include "BuildingGraphVertex.h"

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
}

void UBuildingManagerSubsystem::InitBuildingGraph()
{
	GlobalBuildingGraph = NewObject<UBuildingGraph>(this);
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

ABuildingActor* UBuildingManagerSubsystem::GetBuildingActorByVertexHandle(FGraphVertexHandle VertexHandle)
{
	const auto FoundCurrentVtx = RegisteredBuildingActors.FindByPredicate([VertexHandle](const ABuildingActor* Actor)
	{
		return Actor->GraphVertexHandle == VertexHandle;
	});

	if (FoundCurrentVtx)
	{
		return *FoundCurrentVtx;
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
}

void UBuildingManagerSubsystem::RegisterBuildingActor(ABuildingActor* BuildingActor)
{
	RegisteredBuildingActors.Add(BuildingActor);
}

void UBuildingManagerSubsystem::UnregisterBuildingActor(ABuildingActor* BuildingActor)
{
	RegisteredBuildingActors.Remove(BuildingActor);
}

TArray<ABuildingActor*> UBuildingManagerSubsystem::GetRegisteredBuildingActors() const
{
	return RegisteredBuildingActors;
}

void UBuildingManagerSubsystem::AddVertex(ABuildingActor* BuildingActor, const bool bIsRooted)
{
	if (!BuildingActor) return;
	
	const auto NewVertexHandle = GlobalBuildingGraph->CreateVertex();
	UBuildingGraphVertex* BuildingGraphVtx = Cast<UBuildingGraphVertex>(NewVertexHandle.GetVertex());
	BuildingGraphVtx->bIsRoot = bIsRooted;
	BuildingActor->GraphVertexHandle = NewVertexHandle;
}

void UBuildingManagerSubsystem::ConnectVertex(ABuildingActor* BuildingActor1, ABuildingActor* BuildingActor2)
{
	if (BuildingActor1 && BuildingActor2)
	{
		TArray<FEdgeSpecifier> NodesToConnect;
		NodesToConnect.Add(FEdgeSpecifier(BuildingActor1->GraphVertexHandle, BuildingActor2->GraphVertexHandle));
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
				NodesToConnect.Add(FEdgeSpecifier(BuildingActor->GraphVertexHandle, Actor->GraphVertexHandle));
			}
		}
		GlobalBuildingGraph->CreateBulkEdges(MoveTemp(NodesToConnect));
	}
}

TArray<ABuildingActor*> UBuildingManagerSubsystem::GetNeighbourActors(ABuildingActor* BuildingActor)
{
	TArray<ABuildingActor*> NeighbourActors;
	
	const auto ActorVertexHandle = BuildingActor->GraphVertexHandle;
	const auto FoundVertexObject = GlobalBuildingGraph->GetVertices().Find(ActorVertexHandle);
	if (FoundVertexObject != nullptr)
	{
		const auto GraphVertex = *FoundVertexObject;
		GraphVertex->ForEachAdjacentVertex([this, &NeighbourActors](const FGraphVertexHandle& Vertex)
		{
			const auto FoundBuildingActor = RegisteredBuildingActors.FindByPredicate([&Vertex](const ABuildingActor* BuildingActor)
			{
				return Vertex == BuildingActor->GraphVertexHandle;
			});

			if (FoundBuildingActor != nullptr)
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
			if (Vector.Equals(Actor->GetActorLocation(), Threshold))
			{
				Result.Add(Actor);
			}
		}
	}

	return Result;
}
