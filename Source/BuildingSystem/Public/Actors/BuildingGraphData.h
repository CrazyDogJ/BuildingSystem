// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Graph/BuildingGraph.h"
#include "BuildingGraphData.generated.h"

class UBuildingGraph;

UCLASS()
class BUILDINGSYSTEM_API ABuildingGraphData : public AActor
{
	GENERATED_BODY()

public:
	ABuildingGraphData();

#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void UpdateBuildingGraph();
#endif
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	FSerializableBuildingGraph BuildingGraphData;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShowGraph = false;
	
	UPROPERTY(BlueprintReadOnly, Transient)
	UBuildingGraph* BuildingGraph = nullptr;
#endif
	
protected:
	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual bool ShouldTickIfViewportsOnly() const override { return bShowGraph; }
};
