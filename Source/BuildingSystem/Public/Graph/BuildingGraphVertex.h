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

	/** The one runtime logic instance, retained here even while its Outer is the loaded actor. */
	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UBuildingLogicObject> BuildingLogicObject;
	
	// Legacy saves need the building actor to supply the original logic class.
	TArray<uint8> PendingLogicData;
	TSoftClassPtr<UBuildingLogicObject> PendingLogicClass;

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

	/** Restore pending state, reusing an existing instance when its class matches. */
	void InitializeLogicObject(TSubclassOf<UBuildingLogicObject> FallbackClass = nullptr, UBuildingLogicObject* ExistingLogic = nullptr);

protected:
	virtual void HandleOnVertexRemoved() override;
};
