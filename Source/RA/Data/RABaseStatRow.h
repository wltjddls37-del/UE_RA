// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RABaseStatRow.generated.h"

UENUM(BlueprintType)
enum class ERAStatType : uint8
{
	Health,
	AttackPower,
};

// 기본 능력치 DataTable 의 Row, RowName 을 ID 로 사용한다
USTRUCT(BlueprintType)
struct FRABaseStatRow : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TMap<ERAStatType, float> BaseValueMap;
};
