// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/RAUserWidget.h"
#include "Data/RABaseStatRow.h"
#include "RAPlayerHUDWidget.generated.h"

class UProgressBar;

// 플레이어 화면 HUD (플레이어 체력, 넥서스 체력, 웨이브, 승리/패배 등)
UCLASS()
class RA_API URAPlayerHUDWidget : public URAUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

private:
	UFUNCTION()
	void HandlePlayerStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue);

	UFUNCTION()
	void HandleNexusStatChanged(ERAStatType StatType, float CurrentValue, float BaseValue);

	URAStatComponent* GetPlayerStatComponent() const;
	URAStatComponent* GetNexusStatComponent() const;

protected:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> PlayerHealthBar;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> NexusHealthBar;
};
