// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingDefinition.h"

#include "Actors/BuildingActor.h"
#include "BuildingActorDescription.h"
#include "Actors/BuildingPreviewActor.h"
#include "BuildingSystem.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/World.h"

ABuildingActor* UBuildingDefinition::SpawnBuildingActor(const UObject* WorldContextObject,
	const FTransform& InTransform)
{
	if (!BuildingActorDescription)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("SpawnBuildingActor : BuildingActorDescription is not valid, please check building def : %s"), *BaseInfo.Id)
		return nullptr;
	}
	
	const auto World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("SpawnBuildingActor : World not valid"))
		return nullptr;
	}

	const FTransform* Transform = &InTransform;
	const auto NewActor = World->SpawnActor(BuildingActorDescription->BuildingActorClass, Transform);
	const auto NewBuildingActor = Cast<ABuildingActor>(NewActor);
	if (NewBuildingActor)
	{
		NewBuildingActor->SetBuildingDefinition(this);
	}
	
	return NewBuildingActor;
}

ABuildingPreviewActor* UBuildingDefinition::SpawnBuildingPreviewActor(const UObject* WorldContextObject)
{
	if (!BuildingActorDescription)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("SpawnBuildingPreviewActor : BuildingActorDescription is not valid, please check building def : %s"), *BaseInfo.Id)
		return nullptr;
	}
	
	const auto World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("SpawnBuildingPreviewActor : World not valid"))
		return nullptr;
	}
	
	const auto NewActor = World->SpawnActor(BuildingActorDescription->BuildingPreviewActorClass);
	const auto NewBuildingPreviewActor = Cast<ABuildingPreviewActor>(NewActor);
	NewBuildingPreviewActor->BuildingDefinition = this;
	NewBuildingPreviewActor->RootStaticMesh->SetStaticMesh(BuildingActorDescription->BuildingMesh);
	BuildingActorDescription->BP_BuildingPreviewActorConstructionEvent(NewBuildingPreviewActor);

	return NewBuildingPreviewActor;
}

#if WITH_EDITOR
void UBuildingDefinition::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
	
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, BaseInfo))
	{
		BaseInfo.RefreshLocalizationKey();
	}
	
	if (PropertyChangedEvent.GetMemberPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, DefaultFragmentsContainer))
	{
		DefaultFragmentsContainer.RebuildTypeMapping();
	}
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	MarkPackageDirty();
}
#endif
