// Fill out your copyright notice in the Description page of Project Settings.


#include "BuildingLogicObject.h"

#include "Net/UnrealNetwork.h"
#include "Serialization/ObjectAndNameAsStringProxyArchive.h"

void UBuildingLogicObject::GetSaveData(TArray<uint8>& OutSaveData)
{
	OutSaveData.Reset();
	FMemoryWriter WriterArchive(OutSaveData, true);
	uint32 Signature = 0x53474F31;
	WriterArchive << Signature;
	FObjectAndNameAsStringProxyArchive Ar(WriterArchive, false);
	Ar.ArIsSaveGame = true;
	Ar.ArNoDelta = true;
	Serialize(Ar);
}

void UBuildingLogicObject::LoadSaveData(const TArray<uint8>& InSaveData)
{
	if (!InSaveData.IsEmpty())
	{
		FMemoryReader MemoryReader(InSaveData, true);
		uint32 Signature = 0;
		if (MemoryReader.TotalSize() < sizeof(Signature)) return;
		MemoryReader << Signature;
		const bool bFilteredArchive = Signature == 0x53474F31;
		if (!bFilteredArchive) MemoryReader.Seek(0);
		FObjectAndNameAsStringProxyArchive Ar(MemoryReader, true);
		Ar.ArIsSaveGame = bFilteredArchive;
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
