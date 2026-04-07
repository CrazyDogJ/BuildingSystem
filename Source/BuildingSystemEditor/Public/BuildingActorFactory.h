// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ActorFactories/ActorFactory.h"
#include "BuildingActorFactory.generated.h"

/**
 * 
 */
UCLASS()
class BUILDINGSYSTEMEDITOR_API UBuildingActorFactory : public UActorFactory
{
	GENERATED_BODY()
	
public:
	UBuildingActorFactory(const FObjectInitializer& ObjectInitializer);
	
	virtual bool CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg) override;
	virtual AActor* SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform, const FActorSpawnParameters& InSpawnParams) override;
	virtual FString GetDefaultActorLabel(UObject* Asset) const override;
};
