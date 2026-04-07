// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SmartObjectDefinition.h"

#include "BuildingBehaviorDefinition.generated.h"

UCLASS()
class BUILDINGSYSTEM_API UBuildingBehaviorDefinition : public USmartObjectBehaviorDefinition
{
	GENERATED_BODY()

public:
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* InteractionStartMontage;

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	UAnimMontage* InteractionEndMontage;
};
