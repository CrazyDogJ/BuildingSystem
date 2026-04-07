// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/GraphIsland.h"
#include "BuildingGraphIsland.generated.h"

/**
 * Usually representing an island part of buildings.
 */
UCLASS(BlueprintType)
class BUILDINGSYSTEM_API UBuildingGraphIsland : public UGraphIsland
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure)
	bool IsIslandRooted() const;

protected:
	UPROPERTY()
	TArray<UGraphVertex*> RootedVertices;

	virtual void ChangeVertexHandle(const FGraphVertexHandle& OldVertexHandle, const FGraphVertexHandle& NewVertexHandle) override;
	virtual void HandleOnVertexAdded(const FGraphVertexHandle& Handle) override;
	virtual void HandleOnVertexRemoved(const FGraphVertexHandle& Handle) override;
};
