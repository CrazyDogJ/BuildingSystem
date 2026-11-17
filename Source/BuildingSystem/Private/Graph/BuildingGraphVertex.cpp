// Fill out your copyright notice in the Description page of Project Settings.


#include "Graph/BuildingGraphVertex.h"

#include "BuildingLogicObject.h"
#include "Graph/BuildingGraph.h"
#include "BuildingManagerSubsystem.h"

UBuildingGraph* UBuildingGraphVertex::GetOuterBuildingGraph() const
{
	return Cast<UBuildingGraph>(GetGraph());
}

UBuildingManagerSubsystem* UBuildingGraphVertex::GetOuterBuildingSubsystem() const
{
	if (const auto OuterGraph = GetOuterBuildingGraph())
	{
		return Cast<UBuildingManagerSubsystem>(OuterGraph->GetOuter());
	}
	
	return nullptr;
}

ABuildingActor* UBuildingGraphVertex::GetBuildingActor() const
{
	if (const auto Subsystem = GetOuterBuildingSubsystem())
	{
		return Subsystem->GetBuildingActorByVertexHandle(Handle());
	}

	return nullptr;
}

UBuildingLogicObject* UBuildingGraphVertex::GetBuildingLogicObject() const
{
	return BuildingLogicObject;
}

void UBuildingGraphVertex::SetBuildingLogicObject(TObjectPtr<UBuildingLogicObject> InBuildingLogicObject, bool bRename)
{
	if (IsValid(InBuildingLogicObject) && bRename && InBuildingLogicObject->GetOuter() != this)
	{
		const FName Name = MakeUniqueObjectName(this, InBuildingLogicObject->GetClass(), InBuildingLogicObject->GetFName());
		if (!InBuildingLogicObject->Rename(*Name.ToString(), this, REN_DontCreateRedirectors | REN_DoNotDirty | REN_NonTransactional)) return;
	}
	BuildingLogicObject = InBuildingLogicObject;
}

void UBuildingGraphVertex::InitializeLogicObject(TSubclassOf<UBuildingLogicObject> FallbackClass, UBuildingLogicObject* ExistingLogic)
{
	UClass* LogicClass = PendingLogicClass.IsNull() ? FallbackClass.Get() : PendingLogicClass.LoadSynchronous();
	if (!LogicClass || !LogicClass->IsChildOf(UBuildingLogicObject::StaticClass()) || LogicClass->HasAnyClassFlags(CLASS_Abstract)) return;
	if (!IsValid(BuildingLogicObject))
	{
		SetBuildingLogicObject(IsValid(ExistingLogic) && ExistingLogic->GetClass() == LogicClass
			? ExistingLogic : NewObject<UBuildingLogicObject>(this, LogicClass));
	}
	if (BuildingLogicObject)
	{
		BuildingLogicObject->LoadSaveData(PendingLogicData);
		PendingLogicData.Reset();
		PendingLogicClass.Reset();
	}
}

void UBuildingGraphVertex::HandleOnVertexRemoved()
{
	if (const auto Actor = GetBuildingActor())
	{
		Actor->Destroy(true);
	}
	
	Super::HandleOnVertexRemoved();
}
