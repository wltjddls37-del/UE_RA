// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MonsterAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/RAMonsterAIUtil.h"
#include "Monster/RAMonster.h"

UBTTask_MonsterAttack::UBTTask_MonsterAttack()
{
	NodeName = TEXT("MonsterAttack");

	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MonsterAttack::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp)
		return EBTNodeResult::Failed;

	ARAMonster* Monster = Cast<ARAMonster>(AIController->GetPawn());

	if (!Monster || Monster->IsDead())
		return EBTNodeResult::Failed;

	// 타겟이 없으면 공격할 수 없다.
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target")));

	if (RAMonsterAI::IsInvalidTarget(Target))
		return EBTNodeResult::Failed;

	// 공격 거리 밖이면 Failed -> 다시 추적
	if (!Monster->IsInAttackRange(Target))
		return EBTNodeResult::Failed;

	// 공격 시작: 이동을 멈추고 공격
	AIController->StopMovement();
	Monster->StartAttack(Target);

	return EBTNodeResult::InProgress;
}

void UBTTask_MonsterAttack::TickTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	ARAMonster* Monster = Cast<ARAMonster>(AIController->GetPawn());
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target")));

	// 죽었거나 타겟을 잃어버리면 종료
	if (!Monster || Monster->IsDead() || RAMonsterAI::IsInvalidTarget(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 공격 중에는 타겟을 바라본다.
	FVector Dir = Target->GetActorLocation() - Monster->GetActorLocation();
	Dir.Z = 0.f;

	if (!Dir.IsNearlyZero())
		Monster->SetActorRotation(Dir.Rotation());

	// 공격 한 번이 끝났을 때 거리 검사
	if (!Monster->IsAttacking())
	{
		// 공격 거리를 벗어났으면 종료 -> 다시 추적
		if (!Monster->IsInAttackRange(Target))
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}

		// 거리 안이면 계속 공격
		Monster->StartAttack(Target);
	}
}

void UBTTask_MonsterAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}