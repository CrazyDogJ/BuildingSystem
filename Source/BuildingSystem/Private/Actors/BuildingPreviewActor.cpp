// Fill out your copyright notice in the Description page of Project Settings.


#include "Actors/BuildingPreviewActor.h"

#include "BuildingActorDescription.h"
#include "BuildingDefinition.h"
#include "BuildingManagerSubsystem.h"

ABuildingPreviewActor::ABuildingPreviewActor()
{
	// True for ticking to trace and get transform!!!
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = false;

	RootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootStaticMesh"));
	RootStaticMesh->SetMobility(EComponentMobility::Movable);
	
	SetActorEnableCollision(false);
	RootStaticMesh->SetCollisionResponseToChannels(ECR_Ignore);
}

UBuildingActorDescription* ABuildingPreviewActor::GetBuildingActorDescription() const
{
	if (BuildingDefinition)
	{
		if (const auto Subsystem = GetWorld()->GetSubsystem<UBuildingManagerSubsystem>())
		{
			return Subsystem->GetBuildingActorDescription(BuildingDefinition);
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

			// Enough space to build detect.
			TArray<FHitResult> OverlappingActors;
			FCollisionQueryParams Params = FCollisionQueryParams();
			Params.AddIgnoredActor(this);

			if (RootStaticMesh->GetStaticMesh())
			{
				const auto Extend = RootStaticMesh->GetStaticMesh()->GetBoundingBox().GetExtent();
				const auto Center = RootStaticMesh->GetStaticMesh()->GetBoundingBox().GetCenter();
				GetWorld()->SweepMultiByChannel(OverlappingActors, GetActorLocation() + Center, GetActorLocation() + Center, GetActorRotation().Quaternion(),
					ECC_Pawn, FCollisionShape::MakeBox(Extend - FVector(Desc->BuildingValidThreshold)), Params);
			}
			
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

void ABuildingPreviewActor::UpdateBuildingSelectTrace()
{
	if (const auto Desc = GetBuildingActorDescription())
	{
		if (GetWorld()->GetFirstLocalPlayerFromController())
		{
			FHitResult HitResult;
			const auto BuildingActor = Desc->BP_TraceToGetBuildingActor(GetWorld()->GetFirstLocalPlayerFromController()->PlayerController, HitResult);
			if (SelectedBuildingActor != BuildingActor)
			{
				ClearSelectedBuildingMat();
				SelectedBuildingActor = BuildingActor;

				if (SelectedBuildingActor)
				{
					SelectedBuildingActor->RootStaticMesh->SetOverlayMaterial(SelectedOverlayMaterial);
				}
			}
		}
	}
}

void ABuildingPreviewActor::ClearSelectedBuildingMat()
{
	if (SelectedBuildingActor)
	{
		SelectedBuildingActor->RootStaticMesh->SetOverlayMaterial(nullptr);
		SelectedBuildingActor = nullptr;
	}
}

bool ABuildingPreviewActor::AdditionalCanBuild_Implementation() const
{
	return true;
}

void ABuildingPreviewActor::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Update actor visibility and collision.
	RootStaticMesh->SetVisibility(bBuildingMode, true);

	if (bBuildingMode)
	{
		UpdateTraceTransform();
		UpdateMaterials();

		ClearSelectedBuildingMat();
	}
	else
	{
		UpdateBuildingSelectTrace();
	}
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

	ClearSelectedBuildingMat();
}
