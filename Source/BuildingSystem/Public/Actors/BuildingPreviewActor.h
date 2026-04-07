// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPreviewActor.generated.h"

class UBuildingDefinition;
class UBuildingActorDescription;
class ABuildingActor;

UCLASS(BlueprintType, Blueprintable)
class BUILDINGSYSTEM_API ABuildingPreviewActor : public AActor
{
	GENERATED_BODY()

public:
	ABuildingPreviewActor();

	// Define how to modify materials by bCanBuild.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Settings")
	TArray<FName> CanBuildMaterialScalarNames = {"Alpha"};

	// The main static mesh component.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	UStaticMeshComponent* RootStaticMesh;

	// Current using building definition.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	UBuildingDefinition* BuildingDefinition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	float YawOffset = 0.0f;
	
	// You can add some hit results data here to help you determine.
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	TArray<FHitResult> HitResults;

	// Can build bool, determine can we build right here.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Category = "State")
	bool bCanBuild = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	bool bBuildingMode = true;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	UMaterialInterface* SelectedOverlayMaterial = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "State")
	ABuildingActor* SelectedBuildingActor = nullptr;
	
	UBuildingActorDescription* GetBuildingActorDescription() const;
	void UpdateMaterials() const;
	void UpdateTraceTransform();
	void UpdateBuildingSelectTrace();
	void ClearSelectedBuildingMat();

	UFUNCTION(BlueprintNativeEvent)
	bool AdditionalCanBuild() const;
	
	virtual void Tick(float DeltaSeconds) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
};
