// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "BTTask_MonsterPatrol.generated.h"

UCLASS()
class RA_API UBTTask_MonsterPatrol : public UBTTaskNode
{
	GENERATED_BODY()

public:
	UBTTask_MonsterPatrol();

protected:
	// 순찰 반경
	UPROPERTY(EditAnywhere, Category = "Patrol")
	float mPatrolRadius = 1000.f;

	// 도착 판정 거리
	UPROPERTY(EditAnywhere, Category = "Patrol")
	float mAcceptanceRadius = 50.f;

public:
	virtual EBTNodeResult::Type ExecuteTask(
		UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	virtual void TickTask(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, float DeltaSeconds) override;

	virtual void OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
		uint8* NodeMemory, EBTNodeResult::Type TaskResult) override;
};