// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/GraphHandle.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "BuildingSystemLibrary.generated.h"

USTRUCT(BlueprintType)
struct FGraphVertexHandle_BP : public FGraphVertexHandle
{
	GENERATED_BODY()

public:
	FGraphVertexHandle_BP() {};
	
	FGraphVertexHandle_BP(const FGraphVertexHandle& Other)
		: FGraphVertexHandle(Other)
	{}
};

class UBuildingGraph;
/**
 * 
 */
UCLASS()
class BUILDINGSYSTEM_API UBuildingSystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

	UFUNCTION(BlueprintCallable)
	static UBuildingGraph* NewBuildingGraph();

	UFUNCTION(BlueprintCallable)
	static int GetIslandCount(UBuildingGraph* BuildingGraph);
	
	UFUNCTION(BlueprintCallable)
	static FGraphVertexHandle_BP AddComponentToBuildingGraph(UPrimitiveComponent* Comp, UBuildingGraph* BuildingGraph);
	
	UFUNCTION(BlueprintCallable)
	static FGraphVertexHandle_BP FindVertexHandleByComp(UPrimitiveComponent* Comp, UBuildingGraph* BuildingGraph);
	
	UFUNCTION(BlueprintCallable)
	static void RemoveVertexByComp(UPrimitiveComponent* Comp, UBuildingGraph* BuildingGraph);
	
	UFUNCTION(BlueprintCallable)
	static bool SetCompRooted(bool bIsRoot, UPrimitiveComponent* Comp, UBuildingGraph* BuildingGraph);
	
	static void ConnectVertexes(const FGraphVertexHandle& Vtx1, const FGraphVertexHandle& Vtx2, UBuildingGraph* BuildingGraph);

	UFUNCTION(BlueprintCallable)
	static void ConnectVertexesByComp(UPrimitiveComponent* Comp1, UPrimitiveComponent* Comp2, UBuildingGraph* BuildingGraph);

	UFUNCTION(BlueprintCallable, meta=( WorldContext = WorldContextObject ))
	static FString GetDebugVertexString(const UObject* WorldContextObject);
};
