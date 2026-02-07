// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "BuildingDefinition.generated.h"

class ABuildingPreviewActor;
class UBuildingActorDescription;
class ABuildingActor;
class UBoxComponent;

/**
USTRUCT(BlueprintType)
struct FBuildingSnapBox
{
	GENERATED_BODY()
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform BoxTransform;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FVector BoxExtent;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FTransform SnapRelativeTransform;
};
*/

UCLASS(BlueprintType)
class BUILDINGSYSTEM_API UBuildingDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	/** Building id string, used for identity or debug. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic Info")
	FString BuildingId;

	/** Building's display name. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic Info")
	FText BuildingDisplayName;

	/** Building's display description. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Basic Info")
	FText BuildingDescription;

	/** Building's actor description, used for spawning building actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Actor Description", Instanced)
	UBuildingActorDescription* BuildingActorDescription;

	//UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Snap Settings")
	//TArray<FBuildingSnapBox> SnapBoxes;

	//UFUNCTION(BlueprintCallable)
	//TArray<UBoxComponent*> AddSnapBoxes(AActor* ActorToAdd);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	ABuildingActor* SpawnBuildingActor(const UObject* WorldContextObject, const FTransform& InTransform);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	ABuildingPreviewActor* SpawnBuildingPreviewActor(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	UBuildingActorDescription* GetBuildingActorDescription(const UObject* WorldContextObject) const;

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
