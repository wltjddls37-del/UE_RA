// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "RAUserWidget.generated.h"

class ARAPlayer;
class ARACharacter;
class ARANexus;
class URAStatComponent;
class UProgressBar;

// RA 위젯 공용 부모, 자주 쓰는 포인터 반환 함수 모음
UCLASS()
class RA_API URAUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	ARAPlayer* GetRAPlayer() const;

	// 플레이어 Pawn 을 ARACharacter 로 캐스팅
	ARACharacter* GetRACharacter() const;

	// 플레이어 Pawn 의 StatComponent
	URAStatComponent* GetStatComponent() const;

	// WaveSubSystem 에 등록된 넥서스
	ARANexus* GetRANexus() const;

protected:
	// 체력 비율(Current / Base)로 ProgressBar 갱신
	static void SetHealthPercent(UProgressBar* HealthBar, float CurrentValue, float BaseValue);
	static void SetHealthPercent(UProgressBar* HealthBar, const URAStatComponent* StatComponent);
};
