// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/Graph.h"
#include "Graph/GraphDefaultSerialization.h"

#include "BuildingGraph.generated.h"

/**
 * Representing a building connection graph.
 */
UCLASS(BlueprintType)
class BUILDINGSYSTEM_API UBuildingGraph : public UGraph
{
	GENERATED_BODY()
	
public:
	virtual TObjectPtr<UGraphIsland> CreateTypedIsland() const override;
	virtual TObjectPtr<UGraphVertex> CreateTypedVertex() const override;
	static bool IsIslandRooted(const FGraphIslandHandle& Island);
};

/**
 * Building graph vertex save game begin.
 */
USTRUCT()
struct FSerializableBuildingVertex
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame)
	bool bIsRooted = false;
};

USTRUCT(BlueprintType)
struct FSerializableBuildingGraph : public FSerializableGraph
{
	GENERATED_BODY()
	
	UPROPERTY(SaveGame)
	TMap<FGraphVertexHandle, FSerializableBuildingVertex> BuildingVertices;
};

class BUILDINGSYSTEM_API TBuildingGraphSerialization  : public TDefaultGraphSerialization<FSerializableBuildingGraph>
{
	virtual void WriteGraphVertex(const FGraphVertexHandle& VertexHandle, const UGraphVertex* Vertex) override;
};

class BUILDINGSYSTEM_API TBuildingGraphDeserialization : public TDefaultGraphDeserialization<FSerializableBuildingGraph>
{
	
public:
	explicit TBuildingGraphDeserialization(const FSerializableBuildingGraph& InData)
		: TDefaultGraphDeserialization(InData)
	{}
	
protected:
	virtual void OnDeserializedVertex(const FGraphVertexHandle& VertexHandle) const override;
};
