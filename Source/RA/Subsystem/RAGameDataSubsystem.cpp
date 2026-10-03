// Fill out your copyright notice in the Description page of Project Settings.


#include "Subsystem/RAGameDataSubsystem.h"
#include "Engine/DataTable.h"
#include "Data/RASpawnRow.h"
#include "Data/RABaseStatRow.h"

void URAGameDataSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	SpawnDataTable = SpawnDataTablePath.LoadSynchronous();
	BaseStatDataTable = BaseStatDataTablePath.LoadSynchronous();
}

template <typename T>
const T* URAGameDataSubsystem::FindRow(const UDataTable* DataTable, FName RowName) const
{
	if (DataTable == nullptr || RowName.IsNone())
	{
		return nullptr;
	}

	return DataTable->FindRow<T>(RowName, TEXT("URAGameDataSubsystem::FindRow"));
}

const FRASpawnRow* URAGameDataSubsystem::GetSpawnData(FName RowName) const
{
	return FindRow<FRASpawnRow>(SpawnDataTable, RowName);
}

const FRABaseStatRow* URAGameDataSubsystem::GetBaseStatData(FName RowName) const
{
	return FindRow<FRABaseStatRow>(BaseStatDataTable, RowName);
}
