// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/GraphVertex.h"
#include "BuildingGraphVertex.generated.h"

class UBuildingManagerSubsystem;
class UBuildingGraph;
class ABuildingActor;

/**
 * Usually representing a building actor.
 */
UCLASS(BlueprintType)
class BUILDINGSYSTEM_API UBuildingGraphVertex : public UGraphVertex
{
	GENERATED_BODY()

public:
	/** If this vertex is rooted. */
	UPROPERTY(BlueprintReadOnly)
	bool bIsRoot = false;

	/** Get outer building graph. */
	UBuildingGraph* GetOuterBuildingGraph() const;

	/** Get outer subsystem. */
	UBuildingManagerSubsystem* GetOuterBuildingSubsystem() const;
	
	/** Building actor may not exist if out of loaded streaming level. */
	ABuildingActor* GetBuildingActor() const;

protected:
	virtual void HandleOnVertexRemoved() override;
};
