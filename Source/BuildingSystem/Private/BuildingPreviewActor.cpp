// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingPreviewActor.h"

ABuildingPreviewActor::ABuildingPreviewActor()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = false;

	RootStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RootStaticMesh"));
	RootStaticMesh->SetMobility(EComponentMobility::Movable);
	RootStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}
