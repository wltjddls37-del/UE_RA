// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/RAUserWidget.h"
#include "Data/RABaseStatRow.h"
#include "RAMonsterHealthBarWidget.generated.h"

class UProgressBar;

// 몬스터 머리 위 체력바, 몬스터의 WidgetComponent 에 세팅
UCLASS()
class RA_API URAMonsterHealthBarWidget : public URAUserWidget
{
	GENERATED_BODY()

public:
	// 몬스터가 StatComponent 의 OnUpdateStat 에 바인딩하는 콜백
	UFUNCTION()
	void UpdateStat(ERAStatType StatType, float CurrentValue, float BaseValue);

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> HealthBar;
};
