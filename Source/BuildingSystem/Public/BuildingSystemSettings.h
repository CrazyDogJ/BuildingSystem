// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "BuildingSystemSettings.generated.h"

/**
 * 
 */
UCLASS(config = BuildingSystem, defaultconfig, notplaceable)
class BUILDINGSYSTEM_API UBuildingSystemSettings : public UObject
{
	GENERATED_BODY()

public:
	UBuildingSystemSettings(const FObjectInitializer& ObjectInitializer);
};
