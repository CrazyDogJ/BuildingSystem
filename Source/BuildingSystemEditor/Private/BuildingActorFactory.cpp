// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingActorFactory.h"

#include "BuildingActorDescription.h"
#include "BuildingDefinition.h"

UBuildingActorFactory::UBuildingActorFactory(const FObjectInitializer& ObjectInitializer)
{
	DisplayName = NSLOCTEXT("ActorFactory", "BuildingActorFactory", "Used to spawn building actor. ");
	NewActorClass = ABuildingActor::StaticClass();
}

bool UBuildingActorFactory::CanCreateActorFrom(const FAssetData& AssetData, FText& OutErrorMsg)
{
	if (!AssetData.IsValid()) return false;
	if (!AssetData.GetClass()) return false;
	
	if (AssetData.GetClass()->IsChildOf(UBuildingDefinition::StaticClass()))
	{
		if (UBuildingDefinition* Asset = Cast<UBuildingDefinition>(AssetData.GetAsset()))
		{
			if (const auto ActorDesc = Asset->BuildingActorDescription)
			{
				if (ActorDesc->BuildingMesh && ActorDesc->BuildingActorClass)
				{
					return true;
				}
			}
		}
	}
	
	return false;
}

AActor* UBuildingActorFactory::SpawnActor(UObject* InAsset, ULevel* InLevel, const FTransform& InTransform,
	const FActorSpawnParameters& InSpawnParams)
{
	const auto BuildingDef = Cast<UBuildingDefinition>(InAsset);
	if (!BuildingDef) return nullptr;
	
	const auto BuildingDesc = BuildingDef->BuildingActorDescription;
	if (!BuildingDesc) return nullptr;

	// 修复：编辑器预览或关卡卸载时可能没有生成世界。
	const auto World = InLevel ? InLevel->GetWorld() : nullptr;
	if (!World) return nullptr;
	const auto NewActor = World->SpawnActor(BuildingDesc->BuildingActorClass, &InTransform);
	
	const auto NewItemActor = Cast<ABuildingActor>(NewActor);
	if (IsValid(NewItemActor)) NewItemActor->SetBuildingDefinition(BuildingDef);
	
	return NewActor;
}

FString UBuildingActorFactory::GetDefaultActorLabel(UObject* Asset) const
{
	const auto BuildingDef = Cast<UBuildingDefinition>(Asset);
	if (!BuildingDef) return Super::GetDefaultActorLabel(Asset);
	
	return "Building_" + BuildingDef->BaseInfo.Id;
}
