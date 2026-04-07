// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuildingGraphData.h"

#include "Actors/BuildingActor.h"
#include "BuildingManagerSubsystem.h"
#include "Graph/BuildingGraph.h"
#include "Graph/BuildingGraphVertex.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABuildingGraphData::ABuildingGraphData()
{
	PrimaryActorTick.bCanEverTick = false;
	bNetLoadOnClient = true;
	SetCanBeDamaged(false);

#if WITH_EDITORONLY_DATA
	bIsSpatiallyLoaded = false;
#endif
}

#if WITH_EDITOR
void ABuildingGraphData::UpdateBuildingGraph()
{
	if (BuildingGraph)
	{
		BuildingGraph->Empty();

		TArray<AActor*> BuildingActors;
		UGameplayStatics::GetAllActorsOfClass(this, ABuildingActor::StaticClass(), BuildingActors);

		for (const auto Itr : BuildingActors)
		{
			const auto BuildingActor = Cast<ABuildingActor>(Itr);
			const auto NewVertex = BuildingGraph->CreateVertex();
			if (const auto BuildingVertex = Cast<UBuildingGraphVertex>(NewVertex.GetVertex()))
			{
				BuildingVertex->bIsRoot = BuildingActor->bIsRooted;
			}
			BuildingActor->Modify();
			BuildingActor->SetGraphVertexHandle(NewVertex);
			// ReSharper disable once CppExpressionWithoutSideEffects
			BuildingActor->MarkPackageDirty();
		}

		TArray<FEdgeSpecifier> Edges;
		for (const auto Itr : BuildingActors)
		{
			const auto BuildingActor = Cast<ABuildingActor>(Itr);
			if (!BuildingActor->GetGraphVertexHandle().IsValid())
			{
				continue;
			}
			
			for (const auto Connection : BuildingActor->NeighbourBuildingActors)
			{
				if (Connection && Connection->GetGraphVertexHandle().IsValid())
				{
					Edges.Add(FEdgeSpecifier(BuildingActor->GetGraphVertexHandle(), Connection->GetGraphVertexHandle()));
				}
			}
		}

		BuildingGraph->CreateBulkEdges(MoveTemp(Edges));
		
		TBuildingGraphSerialization Serializer;
		Serializer << *BuildingGraph;
		const FSerializableBuildingGraph& SerializedData = Serializer.GetData();

		Modify();
		BuildingGraphData = SerializedData;
		// ReSharper disable once CppExpressionWithoutSideEffects
		MarkPackageDirty();
	}
}
#endif

void ABuildingGraphData::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);

#if WITH_EDITOR
	if (BuildingGraph == nullptr && !GWorld->HasBegunPlay())
	{
		BuildingGraph = NewObject<UBuildingGraph>(this);
	}

	const TBuildingGraphDeserialization Deserializer(BuildingGraphData);
	Deserializer >> *BuildingGraph;
#endif
}

void ABuildingGraphData::BeginPlay()
{
	Super::BeginPlay();

	if (const auto Subsystem = GetWorld()->GetSubsystem<UBuildingManagerSubsystem>())
	{
		Subsystem->LoadLevelBuildingGraphData(this);
	}
}

void ABuildingGraphData::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	
#if WITH_EDITOR
	// TODO : Draw debug connection graph here.
#endif
}
