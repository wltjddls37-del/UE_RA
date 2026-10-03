// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/RAUserWidget.h"
#include "Character/RAPlayer.h"
#include "Component/RAStatComponent.h"
#include "Components/ProgressBar.h"
#include "Nexus/RANexus.h"
#include "Subsystem/RAWaveSubSystem.h"

ARAPlayer* URAUserWidget::GetRAPlayer() const
{
	return Cast<ARAPlayer>(GetOwningPlayerPawn());
}

ARACharacter* URAUserWidget::GetRACharacter() const
{
	return Cast<ARACharacter>(GetOwningPlayerPawn());
}

URAStatComponent* URAUserWidget::GetStatComponent() const
{
	const APawn* Pawn = GetOwningPlayerPawn();
	return Pawn ? Pawn->FindComponentByClass<URAStatComponent>() : nullptr;
}

ARANexus* URAUserWidget::GetRANexus() const
{
	const URAWaveSubSystem* WaveSubSystem = GetWorld() ? GetWorld()->GetSubsystem<URAWaveSubSystem>() : nullptr;
	return WaveSubSystem ? WaveSubSystem->GetNexus() : nullptr;
}

void URAUserWidget::SetHealthPercent(UProgressBar* HealthBar, float CurrentValue, float BaseValue)
{
	if (HealthBar)
	{
		HealthBar->SetPercent(BaseValue > 0.f ? CurrentValue / BaseValue : 0.f);
	}
}

void URAUserWidget::SetHealthPercent(UProgressBar* HealthBar, const URAStatComponent* StatComponent)
{
	if (StatComponent)
	{
		SetHealthPercent(HealthBar, StatComponent->GetValue(ERAStatType::Health), StatComponent->GetBaseValue(ERAStatType::Health));
	}
}
