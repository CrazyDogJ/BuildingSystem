// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Net/Core/PushModel/PushModelMacros.h"
#include "UObject/Object.h"
#include "BuildingLogicObject.generated.h"

UCLASS(Blueprintable, BlueprintType)
class BUILDINGSYSTEM_API UBuildingLogicObject : public UObject
{
	GENERATED_BODY()
	REPLICATED_BASE_CLASS(UBuildingLogicObject)
	
public:
	void GetSaveData(TArray<uint8>& OutSaveData);
	void LoadSaveData(const TArray<uint8>& InSaveData);
	
	virtual void NativeTickLogic(float DeltaTime);
	
	UFUNCTION(BlueprintImplementableEvent)
	void TickLogic(float DeltaTime);
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual bool IsSupportedForNetworking() const override { return true; }
	
#if WITH_EDITOR
	virtual bool ImplementsGetWorld() const override
	{
		return true;
	}
#endif
	
};
