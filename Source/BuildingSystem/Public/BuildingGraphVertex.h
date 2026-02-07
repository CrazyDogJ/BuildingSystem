// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Graph/GraphVertex.h"
#include "BuildingGraphVertex.generated.h"

/**
 * 
 */
UCLASS(BlueprintType)
class BUILDINGSYSTEM_API UBuildingGraphVertex : public UGraphVertex
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintReadOnly, meta=(DeprecatedProperty))
	UPrimitiveComponent* PrimitiveComponent = nullptr;
	
	UPROPERTY(BlueprintReadOnly)
	bool bIsRoot = false;
};
