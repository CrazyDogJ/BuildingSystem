// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingDefinition.h"

#include "BuildingActor.h"
#include "BuildingActorDescription.h"
#include "BuildingPreviewActor.h"
#include "BuildingSystem.h"

/**
TArray<UBoxComponent*> UBuildingDefinition::AddSnapBoxes(AActor* ActorToAdd)
{
	TArray<UBoxComponent*> Results;
	if (!ActorToAdd) return Results;
	for (auto BoxSetting : SnapBoxes)
	{
		const auto Comp = ActorToAdd->AddComponentByClass(
			UBoxComponent::StaticClass(), false, BoxSetting.BoxTransform, false);
		const auto BoxComp = Cast<UBoxComponent>(Comp);
		BoxComp->SetBoxExtent(BoxSetting.BoxExtent);
		BoxComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		BoxComp->SetCollisionResponseToAllChannels(ECR_Block);
		Results.Add(BoxComp);
	}

	return Results;
}
*/

ABuildingActor* UBuildingDefinition::SpawnBuildingActor(const UObject* WorldContextObject,
	const FTransform& InTransform)
{
	if (!BuildingActorDescription)
	{
		UE_LOG(LogBuildingSystem, Error, TEXT("SpawnBuildingActor : BuildingActorDescription is not valid, please check building def : %s"), *BuildingId)
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
		UE_LOG(LogBuildingSystem, Error, TEXT("SpawnBuildingPreviewActor : BuildingActorDescription is not valid, please check building def : %s"), *BuildingId)
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
	BuildingActorDescription->BP_BuildingPreviewActorConstructionEvent(NewBuildingPreviewActor);

	return NewBuildingPreviewActor;
}

UBuildingActorDescription* UBuildingDefinition::GetBuildingActorDescription(const UObject* WorldContextObject) const
{
	BuildingActorDescription->SetWorld(WorldContextObject);
	return BuildingActorDescription;
}

#if WITH_EDITOR
void UBuildingDefinition::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	// Localization.
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, BuildingId) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, BuildingDisplayName) ||
		PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, BuildingDescription))
	{
		if (!BuildingId.IsEmpty())
		{
			BuildingDisplayName = FText::ChangeKey(TEXT("BuildingSystem"), BuildingId + "DisplayName", BuildingDisplayName);
			BuildingDescription = FText::ChangeKey(TEXT("BuildingSystem"), BuildingId + "Description", BuildingDescription);
		}
	}
}
#endif
