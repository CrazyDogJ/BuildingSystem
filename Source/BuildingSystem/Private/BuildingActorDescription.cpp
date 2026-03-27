// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingActorDescription.h"

#include "BuildingActor.h"
#include "BuildingPreviewActor.h"
#include "BuildingSystem.h"

class UWorld* UBuildingActorDescription::GetWorld() const
{
	if (World)
		return World;
	
	return UObject::GetWorld();
}

void UBuildingActorDescription::SetWorld(const UObject* WorldContextObject)
{
	if (WorldContextObject)
	{
		World = WorldContextObject->GetWorld();
	}
}

void UBuildingActorDescription::BuildingActorConstructionEvent(ABuildingActor* NewBuildingActor)
{
	if (!NewBuildingActor)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("Construction Event : Building actor ptr is not valid, it should not be triggered! "))
		return;
	}
	NewBuildingActor->RootStaticMesh->SetStaticMesh(BuildingMesh);
}

void UBuildingActorDescription::BuildingPreviewActorConstructionEvent(ABuildingPreviewActor* NewBuildingPreviewActor)
{
	if (!NewBuildingPreviewActor)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("Construction Event : Building preview actor ptr is not valid, it should not be triggered! "))
		return;
	}
	NewBuildingPreviewActor->RootStaticMesh->SetStaticMesh(BuildingMesh);
}

bool UBuildingActorDescription::BP_IsBuildingRooted_Implementation(ABuildingActor* NewBuildingActor, const FHitResult& InHitResult)
{
	return IsBuildingRooted(NewBuildingActor, InHitResult);
}

TArray<ABuildingActor*> UBuildingActorDescription::BP_GetNeighbourActors_Implementation(
	ABuildingActor* NewBuildingActor, const FHitResult& InHitResult)
{
	return GetNeighbourActors(NewBuildingActor, InHitResult);
}

void UBuildingActorDescription::BP_BuildingPreviewActorConstructionEvent_Implementation(
	ABuildingPreviewActor* NewBuildingPreviewActor)
{
	BuildingPreviewActorConstructionEvent(NewBuildingPreviewActor);
}

bool UBuildingActorDescription::BP_TraceToPlaceBuilding_Implementation(APlayerController* TracePlayerController,
                                                                       FTransform& Transform, TArray<FHitResult>& OutHitResult)
{
	return TraceToPlaceBuilding(TracePlayerController, Transform, OutHitResult);
}

void UBuildingActorDescription::BP_BuildingActorConstructionEvent_Implementation(ABuildingActor* NewBuildingActor)
{
	BuildingActorConstructionEvent(NewBuildingActor);
}
