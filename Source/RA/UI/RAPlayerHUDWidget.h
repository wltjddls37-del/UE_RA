// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/RAUserWidget.h"
#include "Data/RABaseStatRow.h"
#include "Subsystem/RAWaveSubSystem.h"
#include "RAPlayerHUDWidget.generated.h"

class UProgressBar;
class UTextBlock;
class UImage;

// 플레이어 화면 HUD (플레이어 체력, 넥서스 체력, 웨이브, 승리/패배 등)
UCLASS()
class RA_API URAPlayerHUDWidget : public URAUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	// 매 프레임 호출 → 크로스헤어가 몬스터를 가리키는지 확인해서 색 변경
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:
	UFUNCTION()
	void HandlePlayerStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue);

	UFUNCTION()
	void HandleNexusStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue);

	UFUNCTION()
	void HandleWaveStarted(int32 WaveNumber);

	UFUNCTION()
	void HandleWaveEnded(int32 WaveNumber, ERAWaveEndReason Reason);

	void UpdateWaveText();

	// 공격력 글자 갱신 (예: ATK 30)
	void UpdateAttackText(float AttackPower);

	URAStatComponent* GetPlayerStatComponent() const;
	URAWaveSubSystem* GetWaveSubSystem() const;
	URAStatComponent* GetNexusStatComponent() const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PlayerHealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> NexusHealthBar;

	// 우측 상단 웨이브 표시 (예: Wave 1 / 3)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> WaveText;

	// 우측 상단 공격력 표시, 파워업 시 자동 갱신
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> AttackText;

	// 화면 중앙 승리 / 패배 표시, 게임 종료 전에는 숨김
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UTextBlock> ResultText;

	// 화면 중앙 크로스헤어 (WBP 에서 이름을 Crosshair 로 만든 Image)
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UImage> Crosshair;

	// 평소 크로스헤어 색
	UPROPERTY(EditAnywhere, Category = "RA|Crosshair")
	FLinearColor CrosshairDefaultColor = FLinearColor::White;

	// 살아있는 몬스터를 조준 중일 때 색
	UPROPERTY(EditAnywhere, Category = "RA|Crosshair")
	FLinearColor CrosshairEnemyColor = FLinearColor::Red;

	UPROPERTY(EditAnywhere, Category = "RA")
	FText VictoryText = FText::FromString(TEXT("VICTORY"));

	UPROPERTY(EditAnywhere, Category = "RA")
	FText DefeatText = FText::FromString(TEXT("DEFEAT"));
};
