// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingDefinition.h"

#include "Actors/BuildingActor.h"
#include "BuildingActorDescription.h"
#include "Actors/BuildingPreviewActor.h"
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
	NewBuildingPreviewActor->BuildingDefinition = this;
	NewBuildingPreviewActor->RootStaticMesh->SetStaticMesh(BuildingActorDescription->BuildingMesh);
	BuildingActorDescription->BP_BuildingPreviewActorConstructionEvent(NewBuildingPreviewActor);

	return NewBuildingPreviewActor;
}

FInstancedStruct UBuildingDefinition::FindFragmentByClass(const UScriptStruct* StructType, bool& bValid) const
{
	FInstancedStruct EmptyStruct;
	EmptyStruct.InitializeAs(StructType);

	if (const auto Found = TypeLookup.Find(StructType))
	{
		FInstancedStruct Dummy;
		Dummy.InitializeAs(DefaultFragments[*Found].GetScriptStruct());

		if (Dummy.Identical(&EmptyStruct, 0))
		{
			bValid = true;
			return DefaultFragments[*Found];
		}
	}
	
	bValid = false;
	return FInstancedStruct();
}

#if WITH_EDITOR
void UBuildingDefinition::RebuildLookup(const FPropertyChangedEvent& PropertyChangedEvent)
{
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, DefaultFragments))
	{
		// Update lookup pre save.
		TypeLookup.Empty();
	
		for (int i = 0; i < DefaultFragments.Num(); i++)
		{
			if (DefaultFragments[i].IsValid())
			{
				auto Type = DefaultFragments[i].GetScriptStruct();
				TypeLookup.Add(Type, i);
			}
		}
	}
}

void UBuildingDefinition::RefreshLocalizationKey(const FPropertyChangedEvent& PropertyChangedEvent)
{
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

void UBuildingDefinition::PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	Modify();
	RebuildLookup(PropertyChangedEvent);
	RefreshLocalizationKey(PropertyChangedEvent);
	
	// ReSharper disable once CppExpressionWithoutSideEffects
	MarkPackageDirty();
}
#endif
