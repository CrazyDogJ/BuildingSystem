// Fill out your copyright notice in the Description page of Project Settings.

#include "BuildingSystemComponent.h"

#include "Net/UnrealNetwork.h"

int32 FBuildingStateList::FindEntryIndex(const UBuildingDefinition* Definition) const
{
	if (const int32* Index = AcceleratorMap.Find(Definition))
	{
		if (Entries.IsValidIndex(*Index) && Entries[*Index].BuildingDefinition == Definition)
		{
			return *Index;
		}
	}
	return Entries.IndexOfByPredicate([Definition](const FBuildingStateEntry& Entry)
	{
		return Entry.BuildingDefinition == Definition;
	});
}

bool FBuildingStateList::IsBuildingUnlocked(const UBuildingDefinition* InBuildingDefinition) const
{
	const int32 Index = FindEntryIndex(InBuildingDefinition);
	if (Index != INDEX_NONE)
	{
		return Entries[Index].bUnlocked;
	}

	return false;
}

int FBuildingStateList::GetBuildingAmount(const UBuildingDefinition* InBuildingDefinition) const
{
	const int32 Index = FindEntryIndex(InBuildingDefinition);
	if (Index != INDEX_NONE)
	{
		return Entries[Index].Amount;
	}

	return 0;
}

FBuildingStateEntry* FBuildingStateList::GetOrAddEntry(const UBuildingDefinition* InBuildingDefinition)
{
	if (!InBuildingDefinition)
	{
		return nullptr;
	}
	
	const int32 ExistingIndex = FindEntryIndex(InBuildingDefinition);
	if (ExistingIndex != INDEX_NONE)
	{
		return &Entries[ExistingIndex];
	}
	
	const auto Index = Entries.Add(InBuildingDefinition);
	AcceleratorMap.Add(InBuildingDefinition, Index);
	if (const auto System = Cast<UBuildingSystemComponent>(Outer))
	{
		System->OnBuildingEntryAdded.Broadcast(Index);
	}
	
	// 修复：蓝图监听器可能修改或扩容 Entries，广播后重新获取条目。
	const int32 CurrentIndex = FindEntryIndex(InBuildingDefinition);
	return CurrentIndex != INDEX_NONE ? &Entries[CurrentIndex] : nullptr;
}

void FBuildingStateList::UnlockBuilding(const UBuildingDefinition* InBuildingDefinition)
{
	if (!InBuildingDefinition)
	{
		return;
	}
	
	if (const auto Ptr = GetOrAddEntry(InBuildingDefinition))
	{
		Ptr->bUnlocked = true;
		MarkItemDirty(*Ptr);
	}
}

void FBuildingStateList::AddBuildingCount(const UBuildingDefinition* InBuildingDefinition, const int& Count)
{
	if (!InBuildingDefinition)
	{
		return;
	}
	
	if (const auto Ptr = GetOrAddEntry(InBuildingDefinition))
	{
		Ptr->Amount += Count;
		MarkItemDirty(*Ptr);
	}
}

TMap<UBuildingDefinition*, FBuildingStateSaveGameEntry> FBuildingStateList::GetSaveGameData() const
{
	TMap<UBuildingDefinition*, FBuildingStateSaveGameEntry> SaveGameData;
	for (const auto& Entry : Entries)
	{
		SaveGameData.Add(const_cast<UBuildingDefinition*>(Entry.BuildingDefinition), FBuildingStateSaveGameEntry(Entry.bUnlocked, Entry.Amount));
	}
	
	return SaveGameData;
}

void FBuildingStateList::LoadSaveGameData(TMap<UBuildingDefinition*, FBuildingStateSaveGameEntry> Data)
{
	for (const auto Itr : Data)
	{
		if (const auto Added = GetOrAddEntry(Itr.Key))
		{
			Added->bUnlocked = Itr.Value.bUnlocked;
			Added->Amount = Itr.Value.Amount;
			MarkItemDirty(*Added);
		}
	}
}

void FBuildingStateList::PreReplicatedRemove(const TArrayView<int32>& RemovedIndices, int32 FinalSize)
{
	for (const auto Itr : RemovedIndices)
	{
		const auto Entry = Entries[Itr];
		AcceleratorMap.Remove(Entry.BuildingDefinition);
		if (const auto System = Cast<UBuildingSystemComponent>(Outer))
		{
			System->OnBuildingEntryRemoved.Broadcast(Itr);
		}
	}
}

void FBuildingStateList::PostReplicatedAdd(const TArrayView<int32>& AddedIndices, int32 FinalSize)
{
	for (const auto Itr : AddedIndices)
	{
		const auto* Entry = &Entries[Itr];
		AcceleratorMap.Add(Entry->BuildingDefinition, Itr);
		if (const auto System = Cast<UBuildingSystemComponent>(Outer))
		{
			System->OnBuildingEntryAdded.Broadcast(Itr);
		}
	}
}

void FBuildingStateList::PostReplicatedChange(const TArrayView<int32>& ChangedIndices, int32 FinalSize)
{
	for (const auto Itr : ChangedIndices)
	{
		const auto* Entry = &Entries[Itr];
		AcceleratorMap.Add(Entry->BuildingDefinition, Itr);
		if (const auto System = Cast<UBuildingSystemComponent>(Outer))
		{
			System->OnBuildingEntryAdded.Broadcast(Itr);
		}
	}
}

bool FBuildingStateList::NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
{
	return FFastArraySerializer::FastArrayDeltaSerialize<FBuildingStateEntry, FBuildingStateList>(Entries, DeltaParams, *this);
}

UBuildingSystemComponent::UBuildingSystemComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
}

void UBuildingSystemComponent::BeginPlay()
{
	Super::BeginPlay();

	BuildingStates.Outer = this;
}

void UBuildingSystemComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, BuildingStates);
}
