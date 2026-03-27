// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPreviewActor.h"

#include "BuildingActorDescription.h"
#include "BuildingDefinition.h"

ABuildingPreviewActor::ABuildingPreviewActor()
{
	// True for ticking to trace and get transform!!!
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	RootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootStaticMesh"));
	RootStaticMesh->SetMobility(EComponentMobility::Movable);
	// Overlap check collision.
	RootStaticMesh->SetCollisionEnabled(ECollisionEnabled::Type::QueryOnly);
	RootStaticMesh->SetCollisionResponseToAllChannels(ECR_Overlap);
}

UBuildingActorDescription* ABuildingPreviewActor::GetBuildingActorDescription() const
{
	if (BuildingDefinition)
	{
		if (const auto BuildingActorDesc = BuildingDefinition->GetBuildingActorDescription(this))
		{
			return BuildingActorDesc;
		}
	}
	
	return nullptr;
}

void ABuildingPreviewActor::UpdateMaterials() const
{
	ForEachComponent(false, [this](UActorComponent* Component)
	{
		if (const auto MeshComp = Cast<UMeshComponent>(Component))
		{
			for (const auto Name : CanBuildMaterialScalarNames)
			{
				MeshComp->SetScalarParameterValueOnMaterials(Name, bCanBuild ? 1 : 0);
			}
		}
	});
}

void ABuildingPreviewActor::UpdateTraceTransform()
{
	if (const auto Desc = GetBuildingActorDescription())
	{
		if (GetWorld()->GetFirstLocalPlayerFromController())
		{
			// Update transform.
			FTransform LocalTransform;
			const auto bLocalCanBuild = Desc->BP_TraceToPlaceBuilding(GetWorld()->GetFirstLocalPlayerFromController()->PlayerController, LocalTransform, HitResults);
			const auto FinalRotation = LocalTransform.TransformRotation(FQuat(FRotator(0, YawOffset, 0)));
			LocalTransform.SetRotation(FinalRotation);
			SetActorTransform(LocalTransform);

			// Overlapping local pawn.
			TArray<AActor*> OverlappingActors;
			GetOverlappingActors(OverlappingActors, APawn::StaticClass());
			if (OverlappingActors.Num() > 0)
			{
				bCanBuild = false;
			}
			else
			{
				if (AdditionalCanBuild())
				{
					bCanBuild = bLocalCanBuild;
				}
				else
				{
					bCanBuild = false;
				}
			}
		}
	}
}

bool ABuildingPreviewActor::AdditionalCanBuild_Implementation() const
{
	return true;
}

void ABuildingPreviewActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	UpdateTraceTransform();
	UpdateMaterials();
}

void ABuildingPreviewActor::BeginPlay()
{
	Super::BeginPlay();

	if (const auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		EnableInput(LocalPlayer->PlayerController);
	}
}

void ABuildingPreviewActor::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (const auto LocalPlayer = GetWorld()->GetFirstLocalPlayerFromController())
	{
		DisableInput(LocalPlayer->PlayerController);
	}
}
