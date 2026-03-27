// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "StructUtils/InstancedStruct.h"
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

	/** Determine what default properties the building has. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, meta=(ExcludeBaseStruct), Category = "Additional Info")
	TArray<FInstancedStruct> DefaultFragments;
	
	/** Building's actor description, used for spawning building actor. */
	UPROPERTY(EditDefaultsOnly, Category = "Actor Description", Instanced)
	UBuildingActorDescription* BuildingActorDescription;
	
	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	ABuildingActor* SpawnBuildingActor(const UObject* WorldContextObject, const FTransform& InTransform);

	UFUNCTION(BlueprintCallable, meta = (WorldContext = "WorldContextObject"))
	ABuildingPreviewActor* SpawnBuildingPreviewActor(const UObject* WorldContextObject);

	UFUNCTION(BlueprintPure)
	UBuildingActorDescription* GetBuildingActorDescription(const UObject* WorldContextObject) const;
	
public:
	// Return default item fragment.
	template <typename T>
		const T* GetFragmentPtr() const
	{
		const UScriptStruct* StructType = T::StaticStruct();
		if (const auto Found = TypeLookup.Find(StructType))
		{
			return DefaultFragments[*Found].GetPtr<T>();
		}
		
		return nullptr;
	}

	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Building System")
	UPARAM(DisplayName = "Property")
	FInstancedStruct FindFragmentByClass(const UScriptStruct* StructType, bool& bValid) const;
	
#if WITH_EDITOR
	void RebuildLookup(const FPropertyChangedEvent& PropertyChangedEvent);
	void RefreshLocalizationKey(const FPropertyChangedEvent& PropertyChangedEvent);
	
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	// Accelerate look up.
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere, AdvancedDisplay, Category = "Additional Info")
	TMap<const UScriptStruct*, int> TypeLookup;
};
