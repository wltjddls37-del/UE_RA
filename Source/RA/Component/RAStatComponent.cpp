// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/RAStatComponent.h"
#include "Engine/GameInstance.h"
#include "Subsystem/RAGameDataSubsystem.h"

// Sets default values for this component's properties
URAStatComponent::URAStatComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	StatMap.Add(ERAStatType::Health, FRAStat());
	StatMap.Add(ERAStatType::AttackPower, FRAStat());
}

// Called when the game starts
void URAStatComponent::BeginPlay()
{
	Super::BeginPlay();

	LoadBaseStat();

	for (TPair<ERAStatType, FRAStat>& Pair : StatMap)
	{
		ResetValue(Pair.Key);
	}
}

void URAStatComponent::LoadBaseStat()
{
	const URAGameDataSubsystem* GameDataSubsystem = UGameInstance::GetSubsystem<URAGameDataSubsystem>(GetWorld()->GetGameInstance());
	if (GameDataSubsystem == nullptr)
	{
		return;
	}

	const FRABaseStatRow* BaseStatRow = GameDataSubsystem->GetBaseStatData(BaseStatRowName);
	if (BaseStatRow == nullptr)
	{
		return;
	}

	for (const TPair<ERAStatType, float>& Pair : BaseStatRow->BaseValueMap)
	{
		StatMap.FindOrAdd(Pair.Key).BaseValue = Pair.Value;
	}
}

float URAStatComponent::GetValue(ERAStatType StatType) const
{
	const FRAStat* Stat = StatMap.Find(StatType);
	return Stat ? Stat->CurrentValue : 0.f;
}

float URAStatComponent::GetBaseValue(ERAStatType StatType) const
{
	const FRAStat* Stat = StatMap.Find(StatType);
	return Stat ? Stat->BaseValue : 0.f;
}

void URAStatComponent::AddValue(ERAStatType StatType, float Delta)
{
	FRAStat* Stat = StatMap.Find(StatType);
	if (Stat == nullptr)
	{
		return;
	}

	const float NewValue = FMath::Clamp(Stat->CurrentValue + Delta, 0.f, Stat->BaseValue);
	if (FMath::IsNearlyEqual(NewValue, Stat->CurrentValue))
	{
		return;
	}

	Stat->CurrentValue = NewValue;
	OnUpdateStat.Broadcast(StatType, Stat->CurrentValue, Stat->BaseValue);
}

void URAStatComponent::ResetValue(ERAStatType StatType)
{
	FRAStat* Stat = StatMap.Find(StatType);
	if (Stat == nullptr)
	{
		return;
	}

	Stat->CurrentValue = Stat->BaseValue;
	OnUpdateStat.Broadcast(StatType, Stat->CurrentValue, Stat->BaseValue);
}

void URAStatComponent::AddBaseValue(ERAStatType StatType, float Delta)
{
	FRAStat& Stat = StatMap.FindOrAdd(StatType);

	Stat.BaseValue = FMath::Max(0.f, Stat.BaseValue + Delta);
	Stat.CurrentValue = FMath::Clamp(Stat.CurrentValue + Delta, 0.f, Stat.BaseValue);

	OnUpdateStat.Broadcast(StatType, Stat.CurrentValue, Stat.BaseValue);
}
