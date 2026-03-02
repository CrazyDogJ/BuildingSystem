// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingActor.h"
#include "BuildingPreviewActor.h"
#include "UObject/Object.h"
#include "BuildingActorDescription.generated.h"

class ABuildingPreviewActor;
class ABuildingActor;
class UStaticMesh;

/**
 * You can set class of building actor and add property you want to construct building actor.
 */
UCLASS(EditInlineNew, DefaultToInstanced, Blueprintable)
class BUILDINGSYSTEM_API UBuildingActorDescription : public UObject
{
	GENERATED_BODY()
	
public:
#if WITH_EDITOR
	/** Enable world context functions calling for blueprint users. */
	virtual bool ImplementsGetWorld() const override { return true; }
#endif
	virtual class UWorld* GetWorld() const override;

	UPROPERTY()
	UWorld* World;

	UFUNCTION(BlueprintCallable)
	void SetWorld(const UObject* WorldContextObject);
	
	/** Building actor class to spawn. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Default")
	TSubclassOf<ABuildingActor> BuildingActorClass = ABuildingActor::StaticClass();

	/** Building preview actor class to spawn. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Default")
	TSubclassOf<ABuildingPreviewActor> BuildingPreviewActorClass = ABuildingPreviewActor::StaticClass();

	/** Building actor root static mesh asset. */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Default")
	UStaticMesh* BuildingMesh = nullptr;
	
	/** Building actor construction event. Override it you want. */
	virtual void BuildingActorConstructionEvent(ABuildingActor* NewBuildingActor);

	/** Building preview actor construction event. Override it you want. */
	virtual void BuildingPreviewActorConstructionEvent(ABuildingPreviewActor* NewBuildingPreviewActor);

	/** Trace method. */
	virtual bool TraceToPlaceBuilding(APlayerController* TracePlayerController, FTransform& Transform) { return false; }

	/** Get neighbour actors( if snap )*/
	virtual TArray<ABuildingActor*> GetNeighbourActors(ABuildingActor* NewBuildingActor) { return {}; }

	/** Should building actor rooted. */
	virtual bool IsBuildingRooted(ABuildingActor* NewBuildingActor) { return false; }

	// BP override events ---------------------------------------------------------------------------------------
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Building Events", DisplayName = "Building Actor Construction Event")
	void BP_BuildingActorConstructionEvent(ABuildingActor* NewBuildingActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Building Events", DisplayName = "Building Preview Actor Construction Event")
	void BP_BuildingPreviewActorConstructionEvent(ABuildingPreviewActor* NewBuildingPreviewActor);
	
	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Building Events", DisplayName = "Trace To Place Building")
	bool BP_TraceToPlaceBuilding(APlayerController* TracePlayerController, FTransform& Transform);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Building Events", DisplayName = "Get Neighbour Actors")
	TArray<ABuildingActor*> BP_GetNeighbourActors(ABuildingActor* NewBuildingActor);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent, Category = "Building Events", DisplayName = "Is Building Rooted")
	bool BP_IsBuildingRooted(ABuildingActor* NewBuildingActor);
};
