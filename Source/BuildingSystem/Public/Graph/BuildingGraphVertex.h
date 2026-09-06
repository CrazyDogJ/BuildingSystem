// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/GraphVertex.h"
#include "BuildingGraphVertex.generated.h"

class UBuildingLogicObject;
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

	/** Soft class ptr for building logic to run off state. */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UBuildingLogicObject> BuildingLogicObject;
	
	/** Get outer building graph. */
	UBuildingGraph* GetOuterBuildingGraph() const;

	/** Get outer subsystem. */
	UBuildingManagerSubsystem* GetOuterBuildingSubsystem() const;
	
	/** Building actor may not exist if out of loaded streaming level. */
	ABuildingActor* GetBuildingActor() const;
	
	/** Building actor updated logic. To make it update when unloaded from world. */
	UBuildingLogicObject* GetBuildingLogicObject() const;
	
	/** Set building logic from building actor. */
	void SetBuildingLogicObject(TObjectPtr<UBuildingLogicObject> InBuildingLogicObject, bool bRename = true);

protected:
	virtual void HandleOnVertexRemoved() override;
};
