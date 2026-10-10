// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/RABaseStatRow.h"
#include "RAStatComponent.generated.h"

USTRUCT(BlueprintType)
struct FRAStat
{
	GENERATED_BODY()

	// BaseStatRowName 에 해당하는 DataTable 값으로 덮어쓴다, 없으면 100
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float BaseValue = 100.f;

	// 게임 중 변하는 현재값, BeginPlay 에서 BaseValue 로 초기화
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float CurrentValue = 100.f;
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FOnUpdateStat, ERAStatType, StatType, float, CurrentValue, float, BaseValue);

UCLASS()
class RA_API URAStatComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URAStatComponent();

	float GetValue(ERAStatType StatType) const;

	float GetBaseValue(ERAStatType StatType) const;

	// CurrentValue 에 Delta 를 더한다, 0 ~ BaseValue 범위로 제한
	void AddValue(ERAStatType StatType, float Delta);

	// CurrentValue 를 BaseValue 로 되돌린다
	void ResetValue(ERAStatType StatType);

	// 능력치 강화, BaseValue 와 CurrentValue 를 함께 Delta 만큼 올린다
	void AddBaseValue(ERAStatType StatType, float Delta);

	FOnUpdateStat OnUpdateStat;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	void LoadBaseStat();

protected:
	// 기본 능력치 DataTable 의 RowName
	UPROPERTY(EditAnywhere, Category = "RA")
	FName BaseStatRowName;

	UPROPERTY(EditAnywhere, Category = "RA")
	TMap<ERAStatType, FRAStat> StatMap;
};
