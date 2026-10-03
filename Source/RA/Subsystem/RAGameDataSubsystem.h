// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RAGameDataSubsystem.generated.h"

class UDataTable;
struct FRASpawnRow;
struct FRABaseStatRow;

// DataTable 등 게임 데이터 에셋을 로드해서 들고 있고, RowName 으로 데이터를 조회한다
// 에셋 경로는 DefaultGame.ini 의 [/Script/RA.RAGameDataSubsystem] 에서 설정
UCLASS(Config = Game)
class RA_API URAGameDataSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// 없으면 nullptr
	const FRASpawnRow* GetSpawnData(FName RowName) const;

	// 없으면 nullptr
	const FRABaseStatRow* GetBaseStatData(FName RowName) const;

private:
	template <typename T>
	const T* FindRow(const UDataTable* DataTable, FName RowName) const;

	UPROPERTY(Config)
	TSoftObjectPtr<UDataTable> SpawnDataTablePath;

	UPROPERTY(Config)
	TSoftObjectPtr<UDataTable> BaseStatDataTablePath;

	UPROPERTY()
	TObjectPtr<UDataTable> SpawnDataTable;

	UPROPERTY()
	TObjectPtr<UDataTable> BaseStatDataTable;
};
