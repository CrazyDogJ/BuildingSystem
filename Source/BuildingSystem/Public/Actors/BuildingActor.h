// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectDefinition.h"
#include "GameFramework/Actor.h"
#include "Graph/GraphHandle.h"
#include "BuildingActor.generated.h"

class UBuildingLogicObject;
class UBuildingBehaviorDefinition;
class UBuildingManagerSubsystem;
class UBuildingDefinition;

UCLASS(BlueprintType, Blueprintable)
class BUILDINGSYSTEM_API ABuildingActor : public AActor
{
	GENERATED_BODY()

	friend class UBuildingManagerSubsystem;
	
public:
	ABuildingActor();

	/** Set def and construct actor. */
	UFUNCTION(BlueprintCallable)
	void SetBuildingDefinition(UBuildingDefinition* InBuildingDefinition);

	/** Construct actor by loaded def. */
	UFUNCTION(BlueprintCallable)
	void PostGameLoaded();

	void InitVertexGraph();
	
	void ConstructActor();

	UBuildingManagerSubsystem* GetBuildingManagerSubsystem() const;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, ReplicatedUsing=OnRep_BuildingDefinition, SaveGame)
	UBuildingDefinition* BuildingDefinition;

	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly)
	TSubclassOf<UBuildingLogicObject> BuildingLogicObjectClass;
	
	/** Building logic object, do not set this directly, call SetBuildingLogicObject function instead. */
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	TObjectPtr<UBuildingLogicObject> BuildingLogicObject;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void InitLogicObject();
	
	UFUNCTION(BlueprintCallable)
	UBuildingLogicObject* GetBuildingLogicObject() const;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void SetBuildingLogicObject(UBuildingLogicObject* InBuildingLogicObject);
	
	// Buildings that connected to this building.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, Replicated)
	TArray<TObjectPtr<ABuildingActor>> ConnectedBuildingActors;
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddConnectedBuildingActors(ABuildingActor* InBuildingActor);
	
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void RemoveConnectedBuildingActors(ABuildingActor* InBuildingActor);
	
	UFUNCTION()
	void OnRep_BuildingDefinition();
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UStaticMeshComponent* RootStaticMesh;

#if WITH_EDITORONLY_DATA
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bIsRooted = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<ABuildingActor*> NeighbourBuildingActors;
#endif

#if WITH_EDITOR
	UFUNCTION(CallInEditor)
	void UpdateBuildingGraph() const;
#endif
	
	FGraphVertexHandle GetGraphVertexHandle() const
	{
		return GraphVertexHandle;
	}

	void SetGraphVertexHandle(const FGraphVertexHandle& InGraphVertexHandle);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	bool CanRemoveBuildingActor() const;
	
private:
	void RegisterVertexHandle();
	void UnregisterVertexHandle();
	
	UPROPERTY(SaveGame)
	FGraphVertexHandle GraphVertexHandle;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void OnBuildingIslandNotRoot();

	void InitSmartObject() const;

	virtual void OnConstruction(const FTransform& Transform) override;
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool ReplicateSubobjects(class UActorChannel* Channel, class FOutBunch* Bunch, FReplicationFlags* RepFlags) override;
	
public:
	// Smart object functions -----------------------------------------

	// Try get smart object definition
	UFUNCTION(BlueprintPure)
	const USmartObjectDefinition* TryGetSmartObjectDefinition() const;
	
	// Try get slot definition by index.
	UFUNCTION(BlueprintCallable)
	bool TryGetSlotDefinitionByIndex(int Index, FSmartObjectSlotDefinition& OutDefinition) const;
	
	// Try get nearest slot definition flot smart object slot datas.
	UFUNCTION(BlueprintCallable)
	bool TryGetNearestSlotDefinition(const FVector& Location, FSmartObjectSlotDefinition& OutDefinition) const;

	UFUNCTION(BlueprintCallable)
	bool TryGetBuildingBehaviorDefinition(const FSmartObjectSlotDefinition& InSlot, UBuildingBehaviorDefinition*& OutBuildingBehaviorDefinition) const;
};
