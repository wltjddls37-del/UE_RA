// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RASpawnRow.generated.h"

class ARAMonster;

USTRUCT(BlueprintType)
struct FRASpawnWaveInfo
{
	GENERATED_BODY()

	// 이번 웨이브에 생성할 몬스터 클래스와 마리 수 (예: 근접 3, 원거리 2)
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<TSubclassOf<ARAMonster>, int32> MonsterCountMap;
};

// 스포너 DataTable 의 Row, RowName 을 ID 로 사용한다
USTRUCT(BlueprintType)
struct FRASpawnRow : public FTableRowBase
{
	GENERATED_BODY()

	// 웨이브 순서대로 생성할 몬스터 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FRASpawnWaveInfo> WaveInfoArray;
};
