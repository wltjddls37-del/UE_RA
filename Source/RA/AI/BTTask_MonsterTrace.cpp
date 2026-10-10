#include "BTTask_MonsterTrace.h"
#include "AIController.h"
#include "Navigation/PathFollowingComponent.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/RAMonsterAIUtil.h"
#include "Monster/RAMonster.h"

namespace
{
	// 타겟에게 이동 요청
	// 넥서스처럼 움직이지 않는 큰 액터는 중심이 NavMesh 밖이라 경로를 못 찾으므로, 몬스터 쪽 가장자리 지점으로 이동한다
	void MoveToTarget(AAIController* AIController, ARAMonster* Monster, AActor* Target)
	{
		if (!Monster || Cast<APawn>(Target))
		{
			AIController->MoveToActor(Target);
			return;
		}

		float TargetRadius = 0.f;
		float TargetHalfHeight = 0.f;
		Target->GetSimpleCollisionCylinder(TargetRadius, TargetHalfHeight);

		FVector Dir = Monster->GetActorLocation() - Target->GetActorLocation();
		Dir.Z = 0.f;
		Dir = Dir.GetSafeNormal();

		if (Dir.IsNearlyZero())
			Dir = FVector::ForwardVector;

		// 몬스터마다 넥서스 주변의 다른 지점으로 가도록 방향을 랜덤하게 틀어준다 (한 곳에 몰려 서로 막히는 문제 방지)
		Dir = Dir.RotateAngleAxis(FMath::FRandRange(-70.f, 70.f), FVector::UpVector);

		const float MyRadius = Monster->GetSimpleCollisionRadius();
		const float AcceptanceRadius = Monster->GetAttackDistance() * 0.5f;

		const FVector GoalLocation = Target->GetActorLocation()
			+ Dir * (TargetRadius + MyRadius + AcceptanceRadius);

		AIController->MoveToLocation(GoalLocation, AcceptanceRadius);
	}
}

UBTTask_MonsterTrace::UBTTask_MonsterTrace()
{
	NodeName = TEXT("MonsterTrace");
	bNotifyTick = true;
	bNotifyTaskFinished = true;
}

EBTNodeResult::Type UBTTask_MonsterTrace::ExecuteTask(
	UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AAIController* AIController = OwnerComp.GetAIOwner();
	UBlackboardComponent* BlackboardComp = OwnerComp.GetBlackboardComponent();

	if (!AIController || !BlackboardComp)
		return EBTNodeResult::Failed;

	// 타겟이 없으면 넥서스를 타겟으로 지정
	RAMonsterAI::EnsureNexusTarget(OwnerComp);

	AActor* Target = Cast<AActor>(BlackboardComp->GetValueAsObject(TEXT("Target")));

	if (RAMonsterAI::IsInvalidTarget(Target))
		return EBTNodeResult::Failed;

	// 이미 공격 거리 안이면 Failed -> Selector가 Attack 실행
	if (ARAMonster* Monster = Cast<ARAMonster>(AIController->GetPawn()))
	{
		if (Monster->IsInAttackRange(Target))
			return EBTNodeResult::Failed;
	}

	// 타겟을 따라간다.
	MoveToTarget(AIController, Cast<ARAMonster>(AIController->GetPawn()), Target);

	return EBTNodeResult::InProgress;
}

void UBTTask_MonsterTrace::TickTask(UBehaviorTreeComponent& OwnerComp,
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

	if (!Monster || Monster->IsDead() || RAMonsterAI::IsInvalidTarget(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 공격 거리 안에 들어오면 Failed -> Selector가 Attack 실행
	// 거리는 몬스터 BP 의 AttackDistance, 넥서스처럼 큰 액터는 가장자리 기준
	if (Monster->IsInAttackRange(Target))
	{
		FinishLatentTask(OwnerComp, EBTNodeResult::Failed);
		return;
	}

	// 이동이 멈췄는데 아직 거리 밖이면 다시 이동 요청
	if (AIController->GetMoveStatus() == EPathFollowingStatus::Idle)
		MoveToTarget(AIController, Monster, Target);
}

void UBTTask_MonsterTrace::OnTaskFinished(UBehaviorTreeComponent& OwnerComp,
	uint8* NodeMemory, EBTNodeResult::Type TaskResult)
{
	Super::OnTaskFinished(OwnerComp, NodeMemory, TaskResult);

	if (AAIController* AIController = OwnerComp.GetAIOwner())
		AIController->StopMovement();
}