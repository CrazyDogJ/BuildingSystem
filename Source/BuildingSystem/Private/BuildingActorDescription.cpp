// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingActorDescription.h"

#include "BuildingManagerSubsystem.h"
#include "Actors/BuildingActor.h"
#include "Actors/BuildingPreviewActor.h"
#include "BuildingSystem.h"

class UWorld* UBuildingActorDescription::GetWorld() const
{
	if (const auto OuterSubsystem = Cast<UBuildingManagerSubsystem>(GetOuter()))
	{
		return OuterSubsystem->GetWorld();
	}
	
	return UObject::GetWorld();
}

void UBuildingActorDescription::BuildingActorConstructionEvent(ABuildingActor* NewBuildingActor)
{
	if (!NewBuildingActor)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("Construction Event : Building actor ptr is not valid, it should not be triggered! "))
		return;
	}
}

void UBuildingActorDescription::BuildingPreviewActorConstructionEvent(ABuildingPreviewActor* NewBuildingPreviewActor)
{
	if (!NewBuildingPreviewActor)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("Construction Event : Building preview actor ptr is not valid, it should not be triggered! "))
		return;
	}
}

ABuildingActor* UBuildingActorDescription::BP_TraceToGetBuildingActor_Implementation(APlayerController* TracePlayerController, FHitResult& OutHitResult) const
{
	return TraceToGetBuildingActor(TracePlayerController, OutHitResult);
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
