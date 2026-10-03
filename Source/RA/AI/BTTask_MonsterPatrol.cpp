// Fill out your copyright notice in the Description page of Project Settings.

#include "BTTask_MonsterPatrol.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"

UBTTask_MonsterPatrol::UBTTask_MonsterPatrol()
{
	NodeName = TEXT("MonsterPatrol");

	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MonsterPatrol::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
		return EBTNodeResult::Failed;

	APawn* Pawn = AIController->GetPawn();

	if (!Pawn)
		return EBTNodeResult::Failed;

	// 타겟이 있으면 순찰하지 않는다.
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (BlackboardComp && BlackboardComp->GetValueAsObject(TEXT("Target")))
		return EBTNodeResult::Failed;

	// 주변의 이동 가능한 랜덤 위치를 구한다.
	UNavigationSystemV1* NavSystem =
		FNavigationSystem::GetCurrent<UNavigationSystemV1>(GetWorld());

	if (!NavSystem)
		return EBTNodeResult::Failed;

	FNavLocation RandomLocation;

	if (!NavSystem->GetRandomReachablePointInRadius(
		Pawn->GetActorLocation(), mPatrolRadius, RandomLocation))
		return EBTNodeResult::Failed;

	EPathFollowingRequestResult::Type Result =
		AIController->MoveToLocation(RandomLocation.Location, mAcceptanceRadius);

	if (Result == EPathFollowingRequestResult::Failed)
		return EBTNodeResult::Failed;

	// TODO : MonsterBase를 만든 뒤 걷기 애니메이션 추가

	return EBTNodeResult::InProgress;
}

void UBTTask_MonsterPatrol::TickTask(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, float DeltaSeconds)
{
	AAIController* AIController = OwnerComp.GetAIOwner();

	if (!AIController)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 순찰 중 타겟 발견 시 종료
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (BlackboardComp && BlackboardComp->GetValueAsObject(TEXT("Target")))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 이동이 끝났으면 종료 -> 다시 PatrolWait부터 반복
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Succeeded);
		return;
	}
}

void UBTTask_MonsterPatrol::OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (AAIController* AIController = OwnerComp.GetAIOwner())
		AIController->StopMovement();
}