// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MonsterAttack.h"
#include "AIController.h"
#include "BehaviorTree/BlackboardComponent.h"

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

	if (!AIController)
		return EBTNodeResult::Failed;

	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!BlackboardComp)
		return EBTNodeResult::Failed;

	// 타겟이 없으면 공격할 수 없다.
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target")));

	if (!Target)
		return EBTNodeResult::Failed;

	// 공격 시작: 이동을 멈추고 공격 종료 플래그 초기화
	AIController->StopMovement();
	BlackboardComp->SetValueAsBool(TEXT("AttackEnd"), false);

	// TODO : MonsterBase를 만든 뒤 공격 애니메이션 재생 추가

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

	APawn* Pawn = AIController->GetPawn();
	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target")));

	// 타겟을 잃어버리면 종료
	if (!Pawn || !Target)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 공격 애니메이션이 끝났을 때(AttackEnd == true) 거리 검사
	if (BlackboardComp->GetValueAsBool(TEXT("AttackEnd")))
	{
		BlackboardComp->SetValueAsBool(TEXT("AttackEnd"), false);

		float AttackDistance = BlackboardComp->GetValueAsFloat(TEXT("AttackDistance"));
		float Distance = FVector::Dist2D(Pawn->GetActorLocation(),
			Target->GetActorLocation());

		// 공격 거리를 벗어났으면 종료 -> 다시 추적
		if (Distance > AttackDistance)
		{
			FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
			return;
		}
	}

	// 공격 중에는 타겟을 바라본다.
	FVector Dir = Target->GetActorLocation() - Pawn->GetActorLocation();
	Dir.Z = 0.f;

	if (!Dir.IsNearlyZero())
		Pawn->SetActorRotation(Dir.Rotation());
}

void UBTTask_MonsterAttack::OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);
}