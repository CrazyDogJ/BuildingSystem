// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Actors/BuildingActor.h"
#include "Graph/BuildingGraph.h"
#include "Subsystems/WorldSubsystem.h"
#include "BuildingManagerSubsystem.generated.h"

class UBuildingActorDescription;
class ABuildingGraphData;
class ABuildingActor;
class UBuildingGraph;

/**
 * 
 */
UCLASS()
class BUILDINGSYSTEM_API UBuildingManagerSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual bool IsTickable() const override { return true; }
	virtual void Tick(float DeltaTime) override;
	virtual TStatId GetStatId() const override { RETURN_QUICK_DECLARE_CYCLE_STAT(UBuildingManagerSubsystem, STATGROUP_Tickables); }
	virtual void Deinitialize() override;

	// Graph
	void InitBuildingGraph();
	void LoadLevelBuildingGraphData(const ABuildingGraphData* BuildingGraphData) const;
	void DestroyBuildingGraph();
	UBuildingGraph* GetBuildingGraph() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	FSerializableBuildingGraph GetGraphSaveGameData();

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void LoadGraphSaveGameData(const FSerializableBuildingGraph& Data);

	ABuildingActor* GetBuildingActorByVertexHandle(const FGraphVertexHandle& VertexHandle);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void DrawGraphDebug();
	
	// Building Actors
	void RegisterBuildingActor(ABuildingActor* BuildingActor);
	void UnregisterBuildingActor(const ABuildingActor* BuildingActor);
	TMap<FGraphVertexHandle, ABuildingActor*> GetRegisteredBuildingActors() const;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddVertex(ABuildingActor* BuildingActor, bool bIsRooted = false);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveVertexDirectly(ABuildingActor* BuildingActor);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveVertex(ABuildingActor* BuildingActor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	bool IsIslandRooted(ABuildingActor* BuildingActor);

	bool IsIslandRooted(const UGraphVertex* Vertex) const;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ConnectVertex(ABuildingActor* BuildingActor1, ABuildingActor* BuildingActor2);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void ConnectNeighbourVertices(ABuildingActor* BuildingActor, TArray<ABuildingActor*> NeighbourActors);

	// TODO : Cant get neighbour actors in client side. This is a server side function!
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	TArray<ABuildingActor*> GetNeighbourActors(ABuildingActor* BuildingActor);

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	TArray<ABuildingActor*> QueryNearbyActors(TArray<FVector> Locations, float Threshold = 0.01);

	UPROPERTY()
	TMap<const UBuildingDefinition*, UBuildingActorDescription*> BuildingActorDescriptions;
	
	UFUNCTION(BlueprintCallable)
	UBuildingActorDescription* GetBuildingActorDescription(const UBuildingDefinition* BuildingDefinition);

protected:
	/** Only managed by authority side. */
	UPROPERTY()
	UBuildingGraph* GlobalBuildingGraph;

	UPROPERTY()
	TMap<FGraphVertexHandle, ABuildingActor*> RegisteredBuildingActors;
};
