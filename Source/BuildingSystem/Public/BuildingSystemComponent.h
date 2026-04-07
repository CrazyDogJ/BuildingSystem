// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BuildingDefinition.h"
#include "Components/ActorComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "BuildingSystemComponent.generated.h"

class UBuildingDefinition;

USTRUCT(BlueprintType)
struct FBuildingStateSaveGameEntry
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUnlocked = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Amount = 0;
};

USTRUCT(BlueprintType)
struct FBuildingStateEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FBuildingStateEntry() {}
	FBuildingStateEntry(const UBuildingDefinition* InBuildingDefinition)
		: BuildingDefinition(InBuildingDefinition) {}
	
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	const UBuildingDefinition* BuildingDefinition = nullptr;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bUnlocked = false;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	int Amount = 0;
};

USTRUCT(BlueprintType)
struct FBuildingStateList : public FFastArraySerializer
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	TArray<FBuildingStateEntry> Entries;

	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UObject* Outer = nullptr;

	UPROPERTY(NotReplicated)
	TMap<const UBuildingDefinition*, int> AcceleratorMap;

	bool IsBuildingUnlocked(const UBuildingDefinition* InBuildingDefinition) const;
	int GetBuildingAmount(const UBuildingDefinition* InBuildingDefinition) const;
	
	FBuildingStateEntry* GetOrAddEntry(const UBuildingDefinition* InBuildingDefinition);
	void UnlockBuilding(const UBuildingDefinition* InBuildingDefinition);
	void AddBuildingCount(const UBuildingDefinition* InBuildingDefinition, const int& Count);
	TMap<UBuildingDefinition*, FBuildingStateSaveGameEntry> GetSaveGameData() const;
	void LoadSaveGameData(TMap<UBuildingDefinition*, FBuildingStateSaveGameEntry> Data);
	
	void PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize);
	void PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize);
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FBuildingStateListEvent, int, Index);

UCLASS(Blueprintable, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class BUILDINGSYSTEM_API UBuildingSystemComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UBuildingSystemComponent();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Replicated)
	FBuildingStateList BuildingStates;

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void UnlockBuilding(const UBuildingDefinition* InBuildingDefinition)
	{
		BuildingStates.UnlockBuilding(InBuildingDefinition);
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void AddBuildingCount(const UBuildingDefinition* InBuildingDefinition, int Amount)
	{
		BuildingStates.AddBuildingCount(InBuildingDefinition, Amount);
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	TMap<UBuildingDefinition*, FBuildingStateSaveGameEntry> GetSaveGameData() const
	{
		return BuildingStates.GetSaveGameData();
	}

	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly)
	void LoadSaveGameData(const TMap<UBuildingDefinition*, FBuildingStateSaveGameEntry> Data)
	{
		BuildingStates.LoadSaveGameData(Data);
	}

	UFUNCTION(BlueprintPure)
	bool IsBuildingUnlocked(const UBuildingDefinition* InBuildingDefinition) const
	{
		return BuildingStates.IsBuildingUnlocked(InBuildingDefinition);
	}

	UFUNCTION(BlueprintPure)
	int GetBuildingAmount(const UBuildingDefinition* InBuildingDefinition) const
	{
		return BuildingStates.GetBuildingAmount(InBuildingDefinition);
	}

	UPROPERTY(BlueprintAssignable)
	FBuildingStateListEvent OnBuildingEntryAdded;

	UPROPERTY(BlueprintAssignable)
	FBuildingStateListEvent OnBuildingEntryRemoved;
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
};
