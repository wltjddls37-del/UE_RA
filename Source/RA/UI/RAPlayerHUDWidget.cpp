// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAPlayerHUDWidget.h"
#include "Character/RAPlayer.h"
#include "Component/RAStatComponent.h"
#include "Nexus/RANexus.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"         // 크로스헤어
#include "Monster/RAMonster.h"          // 조준 대상이 몬스터인지 확인

void URAPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();

	// NativeConstruct 는 뷰포트에 다시 추가될 때도 호출되므로 중복 바인딩 방지
	if (URAStatComponent* PlayerStatComponent = GetPlayerStatComponent())
	{
		PlayerStatComponent->OnUpdateStat.AddUniqueDynamic(this, &URAPlayerHUDWidget::HandlePlayerStatChanged);
		SetHealthPercent(PlayerHealthBar, PlayerStatComponent);

		// 시작할 때 현재 공격력 표시
		UpdateAttackText(PlayerStatComponent->GetValue(ERAStatType::AttackPower));
	}

	// 넥서스는 BeginPlay 이전에 WaveSubSystem 에 등록되므로 HUD 생성 시점에 항상 존재
	if (URAStatComponent* NexusStatComponent = GetNexusStatComponent())
	{
		NexusStatComponent->OnUpdateStat.AddUniqueDynamic(this, &URAPlayerHUDWidget::HandleNexusStatChanged);
		SetHealthPercent(NexusHealthBar, NexusStatComponent);
	}

	if (URAWaveSubSystem* WaveSubSystem = GetWaveSubSystem())
	{
		WaveSubSystem->OnWaveStarted.AddUniqueDynamic(this, &URAPlayerHUDWidget::HandleWaveStarted);
		WaveSubSystem->OnWaveEnded.AddUniqueDynamic(this, &URAPlayerHUDWidget::HandleWaveEnded);
	}

	UpdateWaveText();

	if (ResultText)
	{
		ResultText->SetVisibility(ESlateVisibility::Collapsed);
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

	if (URAWaveSubSystem* WaveSubSystem = GetWaveSubSystem())
	{
		WaveSubSystem->OnWaveStarted.RemoveDynamic(this, &URAPlayerHUDWidget::HandleWaveStarted);
		WaveSubSystem->OnWaveEnded.RemoveDynamic(this, &URAPlayerHUDWidget::HandleWaveEnded);
	}

	Super::NativeDestruct();
}

void URAPlayerHUDWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	// WBP 에 Crosshair 이미지가 없으면 할 일 없음
	if (Crosshair == nullptr)
	{
		return;
	}

	// 총 쏠 때와 "같은 함수(TraceAim)" 로 검사 → 빨간색이면 실제로 맞는다
	bool bAimingAtEnemy = false;

	if (const ARAPlayer* Player = GetRAPlayer())
	{
		FHitResult AimHit;
		if (Player->TraceAim(AimHit))
		{
			// 맞은 액터가 몬스터이고 살아있으면 → 적 조준 중
			const ARAMonster* Monster = Cast<ARAMonster>(AimHit.GetActor());
			bAimingAtEnemy = Monster && Monster->IsDead() == false;
		}
	}

	Crosshair->SetColorAndOpacity(bAimingAtEnemy ? CrosshairEnemyColor : CrosshairDefaultColor);
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
	// 파워업(AddBaseValue)으로 공격력이 오르면 여기로 들어온다
	else if (StatType == ERAStatType::AttackPower)
	{
		UpdateAttackText(CurrentValue);
	}
}

void URAPlayerHUDWidget::HandleNexusStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue)
{
	if (StatType == ERAStatType::Health)
	{
		SetHealthPercent(NexusHealthBar, CurrentValue, BaseValue);
	}
}

URAWaveSubSystem* URAPlayerHUDWidget::GetWaveSubSystem() const
{
	return GetWorld() ? GetWorld()->GetSubsystem<URAWaveSubSystem>() : nullptr;
}

void URAPlayerHUDWidget::HandleWaveStarted(int32 WaveNumber)
{
	UpdateWaveText();
}

void URAPlayerHUDWidget::HandleWaveEnded(int32 WaveNumber, ERAWaveEndReason Reason)
{
	const URAWaveSubSystem* WaveSubSystem = GetWaveSubSystem();

	// 넥서스 파괴 = 패배, 마지막 웨이브까지 모두 처치 = 승리
	const bool bDefeat = Reason == ERAWaveEndReason::NexusDestroyed;
	const bool bVictory = bDefeat == false && WaveSubSystem && WaveSubSystem->IsGameOver();

	if (ResultText && (bDefeat || bVictory))
	{
		ResultText->SetText(bDefeat ? DefeatText : VictoryText);
		ResultText->SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}

void URAPlayerHUDWidget::UpdateWaveText()
{
	const URAWaveSubSystem* WaveSubSystem = GetWaveSubSystem();
	if (WaveText == nullptr || WaveSubSystem == nullptr)
	{
		return;
	}

	WaveText->SetText(FText::FromString(FString::Printf(TEXT("Wave %d / %d"), WaveSubSystem->GetCurrentWave(), WaveSubSystem->GetTotalWaveCount())));
}

void URAPlayerHUDWidget::UpdateAttackText(float AttackPower)
{
	if (AttackText == nullptr)
	{
		return;
	}

	// %.0f = 소수점 없이 표시 (30.000000 → 30)
	AttackText->SetText(FText::FromString(FString::Printf(TEXT("ATK %.0f"), AttackPower)));
}
