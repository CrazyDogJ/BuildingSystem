// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingLogicObject.h"

#include "Net/UnrealNetwork.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UBuildingLogicObject::GetSaveData(TArray<uint8>& OutSaveData)
{
	FMemoryWriter WriterArchive(OutSaveData, true);
	FObjectAndNameAsStringProxyArchive Ar(WriterArchive, false);
	Serialize(Ar);
}

void UBuildingLogicObject::LoadSaveData(const TArray<uint8>& InSaveData)
{
	if (!InSaveData.IsEmpty())
	{
		FMemoryReader MemoryReader(InSaveData, true);
		FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
		Serialize(Ar);
	}
}

void UBuildingLogicObject::NativeTickLogic(float DeltaTime)
{
	TickLogic(DeltaTime);
}

void UBuildingLogicObject::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);

	UBlueprintGeneratedClass* BPClass = Cast<UBlueprintGeneratedClass>(GetClass());
	if (BPClass != NULL)
	{
		TArray<class FLifetimeProperty> BP_Props;
		BPClass->GetLifetimeBlueprintReplicationList(BP_Props);
		for (auto& Itr : BP_Props)
		{
			Itr.bIsPushBased = true;
		}
		
		OutLifetimeProps.Append(BP_Props);
	}
	
	FDoRepLifetimeParams Params;
	Params.bIsPushBased = true;
}
