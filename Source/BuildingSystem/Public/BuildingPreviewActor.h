// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPreviewActor.generated.h"

class ABuildingActor;

UCLASS(BlueprintType, Blueprintable)
class BUILDINGSYSTEM_API ABuildingPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ABuildingPreviewActor();

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* RootStaticMesh;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bCanBuild = false;
};
