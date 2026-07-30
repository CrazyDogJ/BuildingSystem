// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingActorDescription.h"
#include "InstancedFragmentContainer.h"
#include "NameTextStruct.h"
#include "Engine/DataAsset.h"
#include "BuildingDefinition.generated.h"

class ABuildingPreviewActor;
class ABuildingActor;
class UBoxComponent;

UCLASS(BlueprintType)
class BUILDINGSYSTEM_API UBuildingDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FNameTextStruct BaseInfo {"", FText(), FText(), "BuildingSystem"};

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	FInstancedFragmentContainer DefaultFragmentsContainer;
	
	/** Building's actor description, used for spawning building actor. */
	UPROPERTY(EditDefaultsOnly, Instanced)
	UBuildingActorDescription* BuildingActorDescription;
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	ABuildingActor* SpawnBuildingActor(const UObject* WorldContextObject, const FTransform& InTransform);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	ABuildingPreviewActor* SpawnBuildingPreviewActor(const UObject* WorldContextObject);
	
public:
	// Return default item fragment.
	template <typename T>
		const T* GetFragmentPtr() const
	{
		return DefaultFragmentsContainer.GetFragmentPtr<T>();
	}
	
#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
