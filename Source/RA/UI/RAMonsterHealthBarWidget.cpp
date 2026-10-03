// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAMonsterHealthBarWidget.h"

void URAMonsterHealthBarWidget::UpdateStat(ERAStatType StatType, float CurrentValue, float BaseValue)
{
	if (StatType == ERAStatType::Health)
	{
		SetHealthPercent(HealthBar, CurrentValue, BaseValue);
	}
}
