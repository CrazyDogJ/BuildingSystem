// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Graph/GraphHandle.h"
#include "BuildingActor.generated.h"

class UBuildingManagerSubsystem;
class UBuildingDefinition;

UCLASS(BlueprintType, Blueprintable)
class BUILDINGSYSTEM_API ABuildingActor : public AActor
{
	GENERATED_BODY()

public:
	ABuildingActor();

	/** Set def and construct actor. */
	UFUNCTION(BlueprintCallable)
	void SetBuildingDefinition(UBuildingDefinition* InBuildingDefinition);

	/** Construct actor by loaded def. */
	UFUNCTION(BlueprintCallable)
	void PostGameLoaded();

	void ConstructActor();

	UBuildingManagerSubsystem* GetBuildingManagerSubsystem() const;
	
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated, SaveGame)
	UBuildingDefinition* BuildingDefinition;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* RootStaticMesh;

	UPROPERTY(SaveGame)
	FGraphVertexHandle GraphVertexHandle;
};
