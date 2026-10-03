// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAPlayerHUDWidget.h"
#include "Character/RAPlayer.h"
#include "Component/RAStatComponent.h"
#include "Nexus/RANexus.h"

void URAPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// NativeConstruct 는 뷰포트에 다시 추가될 때도 호출되므로 중복 바인딩 방지
	if (URAStatComponent* PlayerStatComponent = GetPlayerStatComponent())
	{
		PlayerStatComponent->OnUpdateStat.AddUniqueDynamic(this, &URAPlayerHUDWidget::HandlePlayerStatChanged);
		SetHealthPercent(PlayerHealthBar, PlayerStatComponent);
	}

	// 넥서스는 BeginPlay 이전에 WaveSubSystem 에 등록되므로 HUD 생성 시점에 항상 존재
	if (URAStatComponent* NexusStatComponent = GetNexusStatComponent())
	{
		NexusStatComponent->OnUpdateStat.AddUniqueDynamic(this, &URAPlayerHUDWidget::HandleNexusStatChanged);
		SetHealthPercent(NexusHealthBar, NexusStatComponent);
	}
}

void URAPlayerHUDWidget::NativeDestruct()
{
	if (URAStatComponent* PlayerStatComponent = GetPlayerStatComponent())
	{
		PlayerStatComponent->OnUpdateStat.RemoveDynamic(this, &URAPlayerHUDWidget::HandlePlayerStatChanged);
	}

	if (URAStatComponent* NexusStatComponent = GetNexusStatComponent())
	{
		NexusStatComponent->OnUpdateStat.RemoveDynamic(this, &URAPlayerHUDWidget::HandleNexusStatChanged);
	}

	Super::NativeDestruct();
}

URAStatComponent* URAPlayerHUDWidget::GetPlayerStatComponent() const
{
	const ARAPlayer* Player = GetRAPlayer();
	return Player ? Player->GetStatComponent() : nullptr;
}

URAStatComponent* URAPlayerHUDWidget::GetNexusStatComponent() const
{
	const ARANexus* Nexus = GetRANexus();
	return Nexus ? Nexus->GetStatComponent() : nullptr;
}

void URAPlayerHUDWidget::HandlePlayerStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue)
{
	if (StatType == ERAStatType::Health)
	{
		SetHealthPercent(PlayerHealthBar, CurrentValue, BaseValue);
	}
}

void URAPlayerHUDWidget::HandleNexusStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue)
{
	if (StatType == ERAStatType::Health)
	{
		SetHealthPercent(NexusHealthBar, CurrentValue, BaseValue);
	}
}
